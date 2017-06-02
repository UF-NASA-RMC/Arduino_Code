/*
Author: Issa G Malke
Date: April, 4, 2016
Modified on: May, 23, 2017
*/
#include <Wire.h>

// Global Variables
volatile uint8_t Direction(-1), Speed(-1), valueI(-1), valueII(-1);
unsigned long last_motor_write(0), last_ladder_write(0),last_lift_write(0),last_dump_write(0);

// Debug LEDs
// const int I2C_Debug = 52;

//M1 
const int Motor1En = 22;
const int right_pwm1 = 2;
const int right_dira1 = 24;
const int right_dirb1 = 26;

//M2
const int Motor2En = 23;
const int right_pwm2 = 2; 
const int right_dira2 = 25;
const int right_dirb2 = 27;

//M3 
const int Motor3En = 28;
const int left_pwm1 = 2;
const int left_dira1 = 30;
const int left_dirb1 = 32;

//M4
const int Motor4En = 29;
const int left_pwm2 = 2; 
const int left_dira2 = 31;
const int left_dirb2 = 33;

//Ladder
const int LadderEn = 35;
const int Ladder_pwm = 4; 
const int Ladder_dirA = 37;
const int Ladder_dirB = 39;

//Lift the Ladder
const int LiftEn = 34;
const int Lift_pwm = 5; 
const int Lift_dirA = 36;
const int Lift_dirB = 38;

//Dump
const int DumpEnA = 40;
//const int DumpEnB = 47;   // Fucked up Motor Driver!
const int Dump_pwm = 3;     // 5 on UNO Arduino 
const int Dump_dirA = 42;
const int Dump_dirB = 44;

void DumpUp(){
  digitalWrite(Dump_dirA, HIGH);
  digitalWrite(Dump_dirB, LOW);
}
void DumpDown(){
  digitalWrite(Dump_dirA, LOW);
  digitalWrite(Dump_dirB, HIGH);
}
void Lift(){
  digitalWrite(Lift_dirA, HIGH);
  digitalWrite(Lift_dirB, LOW);
}
void Lower(){
  digitalWrite(Lift_dirA, LOW);
  digitalWrite(Lift_dirB, HIGH);
}
void Ladder_F(){
  digitalWrite(Ladder_dirA, HIGH);
  digitalWrite(Ladder_dirB, LOW);
}
void Ladder_B(){
  digitalWrite(Ladder_dirA, LOW);
  digitalWrite(Ladder_dirB, HIGH);
}
void Move_Forward(){
  digitalWrite(right_dira1, HIGH);
  digitalWrite(right_dirb1, LOW);
  digitalWrite(right_dira2, HIGH);
  digitalWrite(right_dirb2, LOW);
  digitalWrite(left_dira1, HIGH);
  digitalWrite(left_dirb1, LOW);
  digitalWrite(left_dira2, HIGH);
  digitalWrite(left_dirb2, LOW);
}
void Turn_Left(){
  digitalWrite(right_dira1, LOW);
  digitalWrite(right_dirb1, HIGH);
  digitalWrite(right_dira2, LOW);
  digitalWrite(right_dirb2, HIGH);
  digitalWrite(left_dira1, HIGH);
  digitalWrite(left_dirb1, LOW);
  digitalWrite(left_dira2, HIGH);
  digitalWrite(left_dirb2, LOW);
}
void Turn_Right(){
  digitalWrite(right_dira1, HIGH);
  digitalWrite(right_dirb1, LOW);
  digitalWrite(right_dira2, HIGH);
  digitalWrite(right_dirb2, LOW);
  digitalWrite(left_dira1, LOW);
  digitalWrite(left_dirb1, HIGH);
  digitalWrite(left_dira2, LOW);
  digitalWrite(left_dirb2, HIGH);
}
void Move_Backward(){
  digitalWrite(right_dira1, LOW);
  digitalWrite(right_dirb1, HIGH);
  digitalWrite(right_dira2, LOW);
  digitalWrite(right_dirb2, HIGH);
  digitalWrite(left_dira1, LOW);
  digitalWrite(left_dirb1, HIGH);
  digitalWrite(left_dira2, LOW);
  digitalWrite(left_dirb2, HIGH);
}
void setSpeedManually(int x){  // Set Up PWM/Speed Values
    analogWrite(right_pwm1, x);
    analogWrite(right_pwm2, x);
    analogWrite(left_pwm1, x);
    analogWrite(left_pwm2, x); 
}
void setup() {
  
// Serial.begin(9600);
//setup i2c
  Wire.begin(4); //slave with address of 4
  Wire.onReceive(Move);
//Wire.onRequest();  
//  pinMode(I2C_Debug, OUTPUT);

  //setup motor pins
  pinMode(LadderEn, OUTPUT); //----Ladder
  pinMode(Ladder_dirA, OUTPUT);
  pinMode(Ladder_dirB, OUTPUT);
  pinMode(LiftEn, OUTPUT);  //----Lift
  pinMode(Lift_dirA, OUTPUT);
  pinMode(Lift_dirB, OUTPUT);
  pinMode(DumpEnA, OUTPUT);  //----Dump 
//  pinMode(DumpEnB, OUTPUT);
  pinMode(Dump_dirA, OUTPUT);
  pinMode(Dump_dirB, OUTPUT);
  
  pinMode(Motor1En, OUTPUT);
  pinMode(Motor2En, OUTPUT);
  pinMode(Motor3En, OUTPUT);
  pinMode(Motor4En, OUTPUT); 
  pinMode(left_dira1, OUTPUT);
  pinMode(left_dirb1, OUTPUT);
  pinMode(right_dira1, OUTPUT);
  pinMode(right_dirb1, OUTPUT);
  pinMode(left_dira2, OUTPUT);
  pinMode(left_dirb2, OUTPUT);
  pinMode(right_dira2, OUTPUT);
  pinMode(right_dirb2, OUTPUT);

//  digitalWrite(I2C_Debug, LOW);
}

void loop() {  // put your main code here, to run repeatedly:

  unsigned long curr_time = millis(); 
  if(curr_time - last_motor_write > 500){ // if no I2C/Moving signal for one second, Turn Off/ STOP!
    digitalWrite(Motor1En, LOW);
    digitalWrite(Motor2En, LOW);
    digitalWrite(Motor3En, LOW);
    digitalWrite(Motor4En, LOW);
    analogWrite(right_pwm1, 0);
    analogWrite(right_pwm2, 0);
    analogWrite(left_pwm1, 0);
    analogWrite(left_pwm2, 0); 
//    digitalWrite(I2C_Debug, LOW);
    }  
 else{
  digitalWrite(Motor1En, HIGH);
  digitalWrite(Motor2En, HIGH);
  digitalWrite(Motor3En, HIGH);
  digitalWrite(Motor4En, HIGH);
 }

 curr_time = millis(); 
  if(curr_time - last_ladder_write > 500){ // if no I2C/Moving signal for one second, Turn Off/ STOP!
    digitalWrite(LadderEn, LOW);
    analogWrite(Ladder_pwm, 0);
    }  
 else{
  digitalWrite(LadderEn, HIGH);
 }
 curr_time = millis(); 
  if(curr_time - last_lift_write > 500){ // if no I2C/Moving signal for one second, Turn Off/ STOP!
    digitalWrite(LiftEn, LOW);
    analogWrite(Lift_pwm, 0);
    }  
 else{
  digitalWrite(LiftEn, HIGH);
 }
  curr_time = millis(); 
  if(curr_time - last_dump_write > 500){ // if no I2C/Moving signal for one second, Turn Off/ STOP!
//    digitalWrite(Dump_dirA, LOW);   //NOT Polo Driver, Different Module.
//    digitalWrite(Dump_dirB, LOW);   //NOT Polo Driver, Different Module.
    digitalWrite(DumpEnA, LOW);
//    digitalWrite(DumpEnB, LOW);
    analogWrite(Dump_pwm, 0);
    }  
 else{
  digitalWrite(DumpEnA, HIGH);
 }
}
void Move(int howMany){
//  char temp;
      Direction = Wire.read(); 
      Speed = Wire.read();
      valueI = Wire.read(); 
      valueII = Wire.read();
  if(Direction == 1 || Direction == 2 || Direction == 3 || Direction == 4){
    digitalWrite(Motor1En, HIGH);
    digitalWrite(Motor2En, HIGH);
    digitalWrite(Motor3En, HIGH);
    digitalWrite(Motor4En, HIGH);
    digitalWrite(LadderEn, HIGH);

//  digitalWrite(I2C_Debug, HIGH);
    last_motor_write = millis();
  } 
  if(Direction == 5){
//  digitalWrite(I2C_Debug, HIGH);
    digitalWrite(DumpEnA, HIGH);
//    digitalWrite(DumpEnB, LOW);
    DumpUp();
    analogWrite(Dump_pwm, Speed);
    last_dump_write = millis();
  } 
    if(Direction == 6){
//  digitalWrite(I2C_Debug, HIGH);
    digitalWrite(DumpEnA, HIGH);
//    digitalWrite(DumpEnB, HIGH);
    DumpDown();
    analogWrite(Dump_pwm, Speed);
    last_dump_write = millis();
  } 
  if(Direction == 7 || Direction == 8 ){
//  digitalWrite(I2C_Debug, HIGH);
    digitalWrite(LiftEn, HIGH);
    last_lift_write = millis();
  }
  if(Direction == 9 || Direction == 10 ){
//  digitalWrite(I2C_Debug, HIGH);
    digitalWrite(LadderEn, HIGH);
    last_ladder_write = millis();
  }      
 if(Direction == 4){  //Move Backward               
    Move_Backward();        
    analogWrite(right_pwm1, Speed);
    analogWrite(right_pwm2, Speed);
    analogWrite(left_pwm1, Speed);
    analogWrite(left_pwm2, Speed);
  }
 if(Direction == 1){            // Move Forward 
    Move_Forward();
//  digitalWrite(I2C_F, HIGH);
    analogWrite(right_pwm1, Speed);
    analogWrite(right_pwm2, Speed);
    analogWrite(left_pwm1, Speed);
    analogWrite(left_pwm2, Speed);
  }
   if(Direction == 2){          // Turn Right       
    Turn_Right();
    analogWrite(right_pwm1, Speed);
    analogWrite(right_pwm2, Speed);
    analogWrite(left_pwm1, Speed);
    analogWrite(left_pwm2, Speed);
  }
 if(Direction == 3){           // Turn Left 
    Turn_Left();
    analogWrite(right_pwm1, Speed);
    analogWrite(right_pwm2, Speed);
    analogWrite(left_pwm1, Speed);
    analogWrite(left_pwm2, Speed);
 }
 /*
   if(Direction == 5){         // Lift the Ladder Up!
    DumpUp();
    analogWrite(Dump_pwm, Speed);
 }
   if(Direction == 6){         // Lower the Hopper
    DumpDown();
    analogWrite(Dump_pwm, Speed);
 }
 */
   if(Direction == 7){         // Lift the Ladder Up!
    Lift();
    analogWrite(Lift_pwm, Speed);
 }
   if(Direction == 8){         // Lower the Ladder
    Lower();
    analogWrite(Lift_pwm, Speed);
 }
  if(Direction == 9){           // Mine F (+)
    Ladder_F();
    analogWrite(Ladder_pwm, valueI);
 }
   if(Direction == 10){         // Mine B (-)
    Ladder_B();
    analogWrite(Ladder_pwm, valueI);
 }
}

