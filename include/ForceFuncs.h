#include <Arduino.h>
#include "calibration.h"
#include "pin_config.h"


void calibrateForceZero(void){
  for(int i = 0; i < 5; i++){
    restForce[i] = analogRead(FFPins[i]);
  }
}

void printForce(void){
  for(int i = 0; i < 5; i++){
    Serial.println(Fingers[i]);
    Serial.println(force[i]);
  }
}

void readForce(void){
  for(int i = 0; i < 5; i++){
    force[i] = analogRead(FFPins[i]);
  }
}