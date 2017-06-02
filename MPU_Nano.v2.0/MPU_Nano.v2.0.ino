/*
Author: Issa G Malke
Date: June, 18, 2015
*/
#include <Wire.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(3,5); //RX, TX
// Global Variables

volatile int Gyro_X =0;
volatile int Gyro_Y =0;
volatile int Gyro_Z =0;

int8_t Gyro_buffer[3]; // The Buffer, which will store the Incoming Data
const int I2C_DebugI = 3;
const int I2C_DebugII = 4;
const int I2C_DebugIII = 5;
void setup() {
  
  Serial.begin(9600);
  mySerial.begin(115200);
  //setup i2c
 // Wire.begin(7); //slave with address of 7
 // Wire.onReceive();
 // Wire.onRequest(returnGyro);  
    
  pinMode(I2C_DebugI, OUTPUT);
  pinMode(I2C_DebugII, OUTPUT);
  pinMode(I2C_DebugIII, OUTPUT);
}

void loop() {
  
  // put your main code here, to run repeatedly: 
  digitalWrite(I2C_DebugI, LOW);
  digitalWrite(I2C_DebugII, LOW);
  digitalWrite(I2C_DebugII, LOW);
  
  while(mySerial.available()){
   /*
    Read INcoming Data Byte by Byte 
    U may Use Serial.parseInt() to read Int(s), but on the sender use Serial.println();
    USING A BUFFER (in both sender, and receiver)IS WAY FASTER! & BETTER!, but use high freq. "115200"
   */   
   int bytes = mySerial.available();
   for(int i = 0; i <= bytes; i++){
   Gyro_buffer[i] = mySerial.read();
  // parse buffer for your string/command
   }    
    // Read INcoming Data As a Buffer        
   Gyro_X = Gyro_buffer[0];
   Gyro_Y = Gyro_buffer[1];         
   Gyro_Z = Gyro_buffer[2];
  }   
  Serial.println(Gyro_X);
  Serial.println(Gyro_Y);
  Serial.println(Gyro_Z);
  Serial.println("---");
  delay(150);

  delay(15);
 // checkValues();
}
void checkValues(){

  if(Gyro_X >= 0 && Gyro_X <= 100) digitalWrite(I2C_DebugI, HIGH);
  if(Gyro_X >= 101 && Gyro_X <= 200) digitalWrite(I2C_DebugII, HIGH);
  if(Gyro_X >= 201 && Gyro_X <= 255) digitalWrite(I2C_DebugIII, HIGH);
}
void returnGyro(){
      
     // uint8_t Gyro_buffer[3] = {Gyro_X, Gyro_Y, Gyro_Z};
//     digitalWrite(I2C_Debug, HIGH);
        uint8_t Gyro_buffer[3];
         
            Gyro_buffer[0] = Gyro_X ;
            Gyro_buffer[1] = Gyro_Y;
            Gyro_buffer[2] = Gyro_Z ;                     
           
      Wire.write(Gyro_buffer, 3);
    }
