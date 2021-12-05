#include <Arduino.h>
#include <ESP32Servo.h>
#include "calibration.h"
#include "pin_config.h"
#include "ForceFuncs.h"

int Pos_offset = 10;
int Neg_offset = 10;
int driveSpeed = 1;
uint delay_time = 1;

void setupServos(void){
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  for (int i = 0; i < 5; i++){
    Servos[i].setPeriodHertz(50);
    Servos[i].attach(SPins[i], MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  }
}

void disableServos(void){
  for (int i = 0; i<5; i++){
    Servos[i].detach();
  }
}

void checkDelay(){
    if(delay_time < 1) {
    delay_time = 1;
    Serial.println("Minimum time delay");
  }
}

void driveServos(){
  checkDelay();
  delay(delay_time);
  for(int i = 0; i < numOfFingers; i++){
    if(sPos[i] > 2000) sPos[i] = 2000;
    if(sPos[i] < 1000) sPos[i] = 1000;
  }
  for(int i = 0; i < 2; i++){
    Servos[i].write(3000 - sPos[i]);
  }
  for(int i = 2; i < 5; i++){
    Servos[i].write(sPos[i]);
  }
}

void followFingers(void){
  readForce();
  for(int i = 0; i < 5; i++){
    if(force[i] > (restForce[i] + Pos_offset)) {
      sPos[i] += driveSpeed;
    }
    if(force[i] < (restForce[i] - Neg_offset)) {
      sPos[i] -= driveSpeed;
    }

  }
  driveServos();
}

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

//   }
//   driveServos();
// }

// void followFingersAverage(void){
//   int maxSpeed = 5;
//   //readForce();
//   for(int i = 0; i < 5; i++){
//     if(forceAverage[i] > (Rest_array[i] + Pos_offset)) {
//       int temp = sPos[i] + (forceAverage[i]- Rest_array[i])/Pos_offset;
//       if (temp > (sPos[i] + maxSpeed)) temp = sPos[i] + maxSpeed;
//       sPos[i] = temp;
//     }
//     if(forceAverage[i] < (Rest_array[i] - Neg_offset)) {
//       int temp = sPos[i] - (Rest_array[i] - forceAverage[i])/Neg_offset;
//       if (temp < (sPos[i] - maxSpeed)) temp = sPos[i] - maxSpeed;
//       sPos[i] = temp;
//     }

//   }
//   driveServos();
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