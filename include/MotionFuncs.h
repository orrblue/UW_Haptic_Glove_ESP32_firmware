/*
 * Remote Feelings: Adam Curtis, Aymeric Wang, Xinying Hu
 * 11/30/21
 * Prototype version. Author: Adam Curtis
 */

#include <Arduino.h>
#include <ESP32Servo.h>
#include "calibration.h"
#include "pin_config.h"
#include "ForceFuncs.h"
#include "reciever.h"

float forceAverage[5];
int driveSpeed[5];
// int driveSpeed[numOfFingers] = {1,1,1,1,1}; //upgrade to differnt drive speeds after finger calibration


int thumbHoldDown = 200;

int robotForceThreshold = 1;
float Kp = 1.7;
float Ki = 0.0;
uint delay_time = 1000;

//-------------------------Motor Variables-------------------------
int dutyCycle = 0;
int dutyCycle1 = 0;
/* Setting PWM Properties */
const int PWMFreq = 10000; /* 10 KHz */
const int PWMChannel[] = {0,1,2,3,4,5,6,7,8,9};
//const int PWMChannel1 = 1;
const int PWMResolution = 10;
const int MAX_DUTY_CYCLE = (int)(pow(2, PWMResolution) - 1);

int fingerPos[5];


void setupMotors(void){

  for(int i = 0; i < (numOfFingers * 2); i++){
    ledcSetup(PWMChannel[i], PWMFreq, PWMResolution);
    ledcAttachPin(motorPins[i], PWMChannel[i]);
  }
    
  //ledcSetup(PWMChannel1, PWMFreq, PWMResolution);
  /* Attach the LED PWM Channel to the GPIO Pin */
  
  //ledcAttachPin(motorPins[1], PWMChannel1);

  // for(int i = 0; i < (numOfFingers * 2); i++){
  //   ledcSetup(PWMChannel[i], PWMFreq, PWMResolution);
  // }
  // // Attach the LED PWM Channel to the GPIO Pin 
  // for(int i = 0; i < (numOfFingers * 2); i++){
  //   ledcAttachPin(motorPins[i], PWMChannel[i]);
  // }
}

void printFingerPositions(){
  for(int i = 0; i < numOfFingers; i++){
    Serial.print(Fingers[i]);
    Serial.println(analogRead(EncPins[i]));
  }
}

void readFingerPositions(){
  for(int i = 0; i < numOfFingers; i++){
    fingerPos[i] = analogRead(EncPins[i]);
  } 
}

void driveMotorsV2(){
  int motorSpeed[numOfFingers];
  for(int i = 0; i < numOfFingers; i++){
    bool direction = 0;
    if(driveSpeed[i] < 0){ // set direction
      direction = 0;
      motorSpeed[i] = -1 * driveSpeed[i];
    } else{
      direction = 1;
      motorSpeed[i] = driveSpeed[i];
    }
    if(motorSpeed[i] > MAX_DUTY_CYCLE)
      motorSpeed[i] = MAX_DUTY_CYCLE;
    //Serial.println(driveSpeed[i]);
    if(direction){ // drive forward
      ledcWrite(PWMChannel[(2*i)], motorSpeed[i]);
      ledcWrite(PWMChannel[(2*i + 1)], 0);
    }else{ // drive in reverse
      ledcWrite(PWMChannel[(2*i)], 0);
      ledcWrite(PWMChannel[(2*i + 1)], motorSpeed[i]);
    }
  }
}

void testMotors(int delay_time){
  int max = 1000;
  for(int i = -max; i < max; i++){
    for(int j = 0; j < numOfFingers; j++){
      driveSpeed[j] = i;  
    }
    driveMotorsV2();
    delay(delay_time);
  }
  for(int i = max; i > -max; i--){
    for(int j = 0; j < numOfFingers; j++){
      driveSpeed[j] = i;  
    }
    driveMotorsV2();
    delay(delay_time);
  }
}

void stopMotors(){
  for(int i = 0; i < numOfFingers; i++)
    driveSpeed[i] = 0;
  driveMotorsV2(); // zero speed
}

float smooth_force(int force){
  static int avg_force[10];
  long long longForce = 0;
  static unsigned int i = 0;
  avg_force[i] = force;
  for(int j = 0; j < 10; j++){
    longForce += avg_force[j];
  }
  float output = (float)longForce / 10.0;
  i++;
  if(i > 9)
    i = 0;
  return output;
}

void V2FollowandForce(){
  static float Kr = 8;
  float smoothed_force = smooth_force(robotForce);
  readForce();
  for(int i = 0; i < (numOfFingers); i++){
    if(i > 2){
      driveSpeed[i] =  (-Kp * (force[i] - restForce[i]) + Kr * smoothed_force);
    }
    else{
      driveSpeed[i] =  (-Kp * (force[i] - restForce[i]));
    }
  }
  driveMotorsV2();
}

void followFingersV2(){ // barebones function. Needs work to correctly impliment the commented lines
  readForce();
  for(int i = 0; i < (numOfFingers); i++){
    // readFingerPositions();
    // if(fingerPos[i] > fingerPosMin[i] && fingerPos[i] < fingerPosMax[i]){ // check that finger is in safe place
        //if(force[i] > (restForce[i] + Pos_offset)) {
          //driveSpeed[0] += (force[i] - (restForce[i] + Pos_offset)) / accelerationScale; //integralish
          driveSpeed[i] =  -Kp * (force[i] - restForce[i]); // Ki*driveSpeed[i]
          //driveSpeed[i] = Pos_offset;
          // Serial.println(force[i]);
        //}
        // if(force[i] < (restForce[i] - Neg_offset)) {
        //   driveSpeed += (force[i] - restForce[i]) / accelerationScale;
        // }
    // } else{
    //   driveSpeed[i] = 0;
    //    // finger out of safe range stop motor
    //   Serial.print(Fingers[i]);
    //   Serial.println(" out of range");
    //}
  }
  //driveSpeed[0] = 0;
  //driveSpeed[4] += thumbHoldDown;
  driveMotorsV2();
}


// void applyRobotForce(){
//   if(robotForce < 100)
//       followFingersV2();
//   else{
//     for(int i = 0; i < (numOfFingers); i++){
//       driveSpeed[i] = robotForce;
//     }
//   }
//   driveMotorsV2();
// }

// void setupServos(void){
//   // Allow allocation of all timers
//   ESP32PWM::allocateTimer(0);
//   ESP32PWM::allocateTimer(1);
//   ESP32PWM::allocateTimer(2);
//   ESP32PWM::allocateTimer(3);
//   for (int i = 0; i < 5; i++){
//     Servos[i].setPeriodHertz(50);
//     Servos[i].attach(SPins[i], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
//   }
// }

// void disableServos(void){
//   for (int i = 0; i<5; i++){
//     Servos[i].detach();
//   }
// }

void checkDelay(){
    if(delay_time < 1) {
    delay_time = 0;
    Serial.println("Zero delay");
  }
}

// void driveServos(){
//   //checkDelay();
//   delayMicroseconds(delay_time);
//   for(int i = 0; i < numOfFingers; i++){
//     if(servoPosition[i] > 2000) servoPosition[i] = 2000;
//     if(servoPosition[i] < 1000) servoPosition[i] = 1000;
//   }
//   for(int i = 0; i < 2; i++){
//     Servos[i].write(3000 - servoPosition[i]);
//   }
//   for(int i = 2; i < 5; i++){
//     Servos[i].write(servoPosition[i]);
//   }
// }

// integral control to follow fingers
// void followFingersIntegral(void){
//   readForce();
//   for(int i = 0; i < 5; i++){
//     if(force[i] > (restForce[i] + Pos_offset)) {
//       if(forward[i]){
//           driveSpeed[i]++;
//       } else {
//           driveSpeed[i] = 0;
//       }
//       forward[i] = true;
//       sPos[i] += driveSpeed[i];
//     }
//     if(force[i] < (restForce[i] - Neg_offset)) {
//       if(!forward[i]){
//           driveSpeed[i]++;
//       } else {
//           driveSpeed[i] = 0;
//       }
//       forward[i] = false;
//       sPos[i] -= driveSpeed[i];
//     }
//   }
//   driveServos();
// }

// void followFingers(void){ //Working but performance not amazing
//   readForce();
//   for(int i = 0; i < 5; i++){
//     if(force[i] > (restForce[i] + Pos_offset)) {
//       servoPosition[i] += driveSpeed;
//     }
//     if(force[i] < (restForce[i] - Neg_offset)) {
//       servoPosition[i] -= driveSpeed;
//     }
//   }
//   driveServos();
// }

// Proportional control to follow fingers
// void followFingers(void){
//   int maxSpeed = 5;
//   readForce();
//   for(int i = 0; i < 5; i++){
//     if(force[i] > (restForce[i] + Pos_offset * forceScaler[i])) {
//       int temp = sPos[i] + (force[i]- restForce[i])/Pos_offset;
//       if (temp > (sPos[i] + maxSpeed)) temp = sPos[i] + maxSpeed;
//       sPos[i] = temp;
//     }
//     if(force[i] < (restForce[i] - Neg_offset * forceScaler[i])) {
//       int temp = sPos[i] - (restForce[i] - force[i])/Neg_offset;
//       if (temp < (sPos[i] - maxSpeed)) temp = sPos[i] - maxSpeed;
//       sPos[i] = temp;
//     }

// void followFingersAverage(void){
//   //int maxSpeed = 5;
//   //readForce();
//   for(int i = 0; i < 5; i++){
//     // if(forceAverage[i] > (restForce[i] + Pos_offset)) {
//     //   int temp = sPos[i] + (forceAverage[i]- restForce[i])/Pos_offset;
//     //   if (temp > (sPos[i] + driveSpeed)) temp = sPos[i] + driveSpeed;
//     //   sPos[i] = temp;
//     // }
//     // if(forceAverage[i] < (restForce[i] - Neg_offset)) {
//     //   int temp = sPos[i] - (restForce[i] - forceAverage[i])/Neg_offset;
//     //   if (temp < (sPos[i] - driveSpeed)) temp = sPos[i] - driveSpeed;
//     //   sPos[i] = temp;
//     // }
//     if(forceAverage[i] > (restForce[i] + Pos_offset)) {
//       servoPosition[i] += driveSpeed;
//     }
//     if(forceAverage[i] < (restForce[i] - Neg_offset)) {
//       servoPosition[i] -= driveSpeed;
//     }
//   }
//   driveServos();
// }

// void driveHackedServoDeprecated(void){
// digitalWrite(INA, LOW);
//   while(motorTest < 254)
//   { 
//     int AnalogValue = analogRead(FFPins[1]);
//     Serial.print("The direction of output shaft is ");
//     Serial.println(AnalogValue);
//     Serial.print("The 8bit motor PWM signal is ");
//     Serial.println(motorTest);
//     analogWrite(INB, motorTest);
//     motorTest ++ ;
//     delay(20);
//   }
//   while(motorTest > 30)
//   {
//     int AnalogValue = analogRead(FFPins[1]);
//     Serial.print("The direction of output shaft is ");
//     Serial.println(AnalogValue);
//     Serial.print("The 8bit motor PWM signal is ");
//     Serial.println(motorTest);
//     analogWrite(INB, motorTest);
//     motorTest -- ;
//     delay(20);
//   }
// }

//--------Function to close robot arm until it feels an object then open in a loop
// int crunch(void){
//   for(int i = 1300; i<2000; i++){
//     sPos5 = i;
//     readForce();
//     printForce();
//     if(force1 > 3000) {
//       return 1;
//     }
//     driveServos();
//     delay(2);
//   }
//   return 0;
// }
