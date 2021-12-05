/*
 * Remote Feelings: Adam Curtis, Aymeric Wang, Xinying Hu
 * 11/30/21
 * Prototype version. Author: Adam Curtis, Aymeric Wang
 */
//adam made a change
#include <ESP32Servo.h>
#include <Arduino.h>
#include "calibration.h"
#include "pin_config.h"
#include "adc_to_newtons.h"
#include "sender.h"
#include "reciever.h"
#include "MovingAvg.h"
#include "MotionFuncs.h"

  // an array to store the received data
int state = 'b'; //waiting
// int Pos_offset = 10;
// int Neg_offset = 10;
// uint delay_time = 1;
int listSize = 10;
// int driveSpeed = 1;

MovingAverage* avg_force[5];
float forceAverage[5];

#define MAXFORCE 3000

bool calibrated = false;

void robotControl(){
 // Pos_offset = 20 + scaleFactor();
  followFingers();
  send_control(averageFingerPos());
}

void controller() {
  switch(state) {
    
    case 'a' : { // STOP!!
      disableServos();
      state = 'b';
      Serial.println("Stopped");
      break;
    }
    case 'b' : { //Waiting 
      Serial.println("Waiting");
      delay(1000);
      break;
    }
    case 'c' : { // Calibrate Glove
      Serial.println("Calibrating Glove");
      setupServos();
      calibration();
      setupServos();
      delay(200);
      state = 'b';
      break;
    }
    case 'd' : { // Calculate Range
      Serial.println("Calculating Force range");
      calcForceRange();
      calcOffsetScaler();
      state = 'b';
      break;
    }
    case 'e' : { // Free following 
      followFingers();
      Serial.println("Following");
      break;
    }
    case 'f' : { // Robot Control
     // Serial.println("controlling Robot");
      robotControl();
      break;
    }
    case 'p' : { // Increase Force
      Serial.print("increasing force to ");
      Pos_offset += 1;
      Neg_offset += 1;
      Serial.println(Pos_offset);
      state = 'f';
      break;
    }
    case ';' : { // Decrease Force
      Serial.print("Decreasing force to ");
      Pos_offset -= 1;
      Neg_offset -= 1;
      Serial.println(Pos_offset);
      state = 'f';
      break;
    }
    case 'i' : { // Increase Time Delay
      Serial.println("increasing time delay");
      delay_time += 1;
      state = 'f';
      break;
    }  
    case 'o' : { // Decrease Time Delay
      Serial.println("decreasing time delay");
      delay_time -= 1;
      state = 'f';
      break;
    } 
    case 'w' : { // Increasing speed
      Serial.print("Increasing speed to ");
      driveSpeed += 1;
      Serial.println(driveSpeed);
      state = 'f';
      break;
    }
    case 's' : { // Decreasing list
      Serial.print("decreasing speed to ");
      driveSpeed -= 1;
      if(driveSpeed < 1){
        driveSpeed = 0;
        Serial.println("Zero drive speed")
      }
      Serial.println(driveSpeed);
      state = 'f';
      break;
    }
    case 'l' : { // Decreasing list
      Serial.print("Decreasing list length");
      Serial.println(listSize);
      listSize -= 1;
      state = 'f';
      break;
    }
    case 'k' : { // Increasing list
      Serial.print("Increasing list length");
      Serial.println(listSize);
      listSize += 1;
      state = 'f';
      break;
    }    
    default : { // Default Case
      Serial.println("Invalid state");
      state = 'a';
    }
  }
}

void setup() {
	Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1,RXp2,TXp2);
  
  for(int i = 0; i < 5; i++){
    avg_force[i] = construct_moving_average(listSize);
  }
  for(int j = 0; j < listSize; j++){
    for(int i = 0; i < 5; i++){
    forceAverage[i] = update_moving_average_value(avg_force[i], analogRead(FFPins[i]));
  }
}
  setupServos();
  delay(200);
  calibrateForceZero();
  delay(1000);
}


void loop() {

  // for(int i = 0; i < 5; i++){
  //   forceAverage[i] = update_moving_average_value(avg_force[i], analogRead(FFPins[i]));
  // }
  
  // Serial.println(forceAverage[0]);

  if (Serial.available() > 0) {
    state = Serial.read();
  }
  controller();
  // Serial.print("Averaged ");
  // Serial.println(forceAverage[2]);
  // Serial.println("");
  // Serial.print("NOT ");
  // Serial.println(analogRead(FFPins[2]));
  // delay(100);

  // followFingers();
  // driveServos();
  // readForce();
  // printForce();
}