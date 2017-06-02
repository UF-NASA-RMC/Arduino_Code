/*
Author: Issa G Malke
Date: Oct. 21, 2016

*/
#include <Wire.h>

int Rightencoder_pin = 2;  // The pin the Right Side encoder is connected           
int Leftencoder_pin = 3;  // The pin the Left Side encoder is connected  

volatile uint8_t RightPulses, LeftPulses;
unsigned long lastRightPulse, lastLeftPulse;
static uint8_t dataArray[3];
 void Rightcounter() //Update count
 {  
   detachInterrupt(1); 
   RightPulses++; 
   dataArray[0] = RightPulses;
   attachInterrupt(1, Leftcounter, FALLING);
   lastRightPulse = millis(); 
 }
 void Leftcounter() //Update count
 {
   detachInterrupt(0);
   LeftPulses++; 
   dataArray[1] = LeftPulses;
   attachInterrupt(0, Rightcounter, FALLING);   
   lastLeftPulse = millis(); 
 }
void setup()
 {
  Wire.begin(8);
  Wire.onRequest(returnData);
  // Serial.begin(115200);
   pinMode(Rightencoder_pin, INPUT);
   pinMode(Leftencoder_pin, INPUT);
   
   //Interrupt 0 is digital pin 2, so that is where the IR detector is connected
   //Triggers on FALLING (change from HIGH to LOW)
   
   attachInterrupt(0, Rightcounter, FALLING);
   attachInterrupt(1, Leftcounter, FALLING);
   // Initialize
   dataArray[3] = {0}; // Init. w/ ZEROs
   lastRightPulse = 0;
   lastLeftPulse = 0;
   RightPulses = 0;
   LeftPulses = 0;
 }

 void loop()
 {
//  if( digitalRead( Rightencoder_pin) == 0) Rightcounter();
//  if( digitalRead( Leftencoder_pin) == 0) Leftcounter();
  
    if (millis() - lastRightPulse >= 1000){ // If more than 1 second pass zero the pulse counter

        RightPulses = 0;
        dataArray[0] = 0;
    }
    if (millis() - lastLeftPulse >= 1000){ // If more than 1 second pass zero the pulse counter

        LeftPulses = 0;
        dataArray[1] = 0;
    }
// >>>>>>>>>>>>>> DEBUG <<<<<<<<<<<<<
/*
Serial.print(RightPulses,DEC);  
Serial.print("\t"); 
Serial.print(LeftPulses, DEC);
Serial.println();
*/
  }
  void returnData(){
    detachInterrupt(0);
    detachInterrupt(1);
   // uint8_t dataArray[2] = {RightPulses, LeftPulses};
    Wire.write(dataArray, 2);
    attachInterrupt(0, Rightcounter, FALLING );
    attachInterrupt(1, Leftcounter, FALLING );
  }

