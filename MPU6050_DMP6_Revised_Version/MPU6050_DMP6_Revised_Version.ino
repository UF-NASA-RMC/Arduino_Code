/*
 Modified by: Issa G Malke
 Date: June, 18, 2015 
 */
// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "Wire.h"
#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high

// uncomment "OUTPUT_READABLE_YAWPITCHROLL" if you want to see the yaw/
// pitch/roll angles (in degrees) calculated from the quaternions coming
// from the FIFO. Note this also requires gravity vector calculations.
// Also note that yaw/pitch/roll angles suffer from gimbal lock (for
// more info, see: http://en.wikipedia.org/wiki/Gimbal_lock)

#define OUTPUT_READABLE_YAWPITCHROLL

// uncomment "OUTPUT_READABLE_WORLDACCEL" if you want to see acceleration
// components with gravity removed and adjusted for the world frame of
// reference (yaw is relative to initial orientation, since no magnetometer
// is present in this case). Could be quite handy in some cases.
//#define OUTPUT_READABLE_WORLDACCEL

/*
 #define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
 bool blinkState = false;
*/
// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
/*
float Gyro_x = A0;
float Gyro_y = A1;
float Gyro_z = A2;
*/
const int Serial_Debug = 3;
// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

void dmpDataReady() {
    mpuInterrupt = true;
}
int Gyro_X;
int Gyro_Y;
int Gyro_Z;

void sendData(){
  uint8_t Gyro_buffer[3];
  
  int recent_x = Gyro_X;
  int recent_y = Gyro_Y;
  int recent_z = Gyro_Z;
  Gyro_X = 0;
  Gyro_Y = 0;
  Gyro_Z = 0;
  
  Gyro_buffer[0] = (recent_x);
  Gyro_buffer[1] = (recent_y);
  Gyro_buffer[2] = (recent_z);
  /*
  Gyro_buffer[0] = (recent_x) >> 8;
  Gyro_buffer[1] = (recent_x & 0xFF);
  Gyro_buffer[2] = (recent_y) >> 8;
  Gyro_buffer[3] = (recent_y & 0xFF);
  Gyro_buffer[4] = (recent_z) >> 8;
  Gyro_buffer[5] = (recent_z & 0xFF);
  */
  Serial.write(Gyro_buffer, 3);
  digitalWrite(Serial_Debug, HIGH);
  Serial.flush();
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
   /*
    pinMode(pwm_x, OUTPUT);
    pinMode(pwm_y, OUTPUT);
    pinMode(pwm_z, OUTPUT);
   */
   pinMode(Serial_Debug, OUTPUT);
    // join I2C bus (I2Cdev library doesn't do this automatically), with Address = 5
    //  Wire.begin();
        
        TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    Serial.begin(115200);
    // initialize device
    mpu.initialize();

    // verify connection
    //Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } 
    else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
       // Serial.print(devStatus);
       // Serial.println(F(")"));
    }
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    // if programming failed, don't try to do anything
    digitalWrite(Serial_Debug, LOW);
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        // other program behavior stuff here
        // .
        // .
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
        // .
        // .
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
      //  Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } 
    else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
           
                Gyro_X = (int)(ypr[0] * 180/M_PI);
                      
                Gyro_Y = (int)(ypr[1] * 180/M_PI);
                        
                Gyro_Z = (int)(ypr[2] * 180/M_PI);
          
          sendData();  // >>>>>>> SEND DATA <<<<<<<<<
 
        #endif
    }
    
}


