/*
 * Remote Feelings: Adam Curtis, Aymeric Wang, Xinying Hu
 * 11/30/21
 * Prototype version. Author: Adam Curtis, Aymeric Wang
 */
#include <Arduino.h>
#include "calibration.h"
#include "pin_config.h"
#include "sender.h"
#ifndef SENDER_H
#include "reciever.h"
#endif
#include "MotionFuncs.h"

int state = 'b'; //waiting
int lastState = 'b'; //waiting
int listSize = 100;
int robotMin = 1800;

#define MAXFORCE 3000

bool calibrated = false;

void simulate(void){

  //===============James's simulator==================
  // static long int lastTime = 0;
  //int update = 20;
  V2FollowandForce();
  //applyRobotForce();  
  //scale value before sending        
  readFingerPositions();
  send_control(fingerPos[0],fingerPos[1],fingerPos[2],fingerPos[3],fingerPos[4]);

}

int averageFingerPos(void){
  long int Pos_total = 0;
  readFingerPositions();
  for (int i = 3; i < 5; i++){
     Pos_total += fingerPos[i];
  }
  int Pos_AVG = Pos_total / 2;
  //Serial.println(Pos_AVG);
  return Pos_AVG;
}

void robotControl(){
  //Gets force from robot and scales it
  // robotForce = scaleFactor(); 

  //Track finger position
  V2FollowandForce();
  
  //scale value before sending        
  //int corrected_value = robotMin - 0.8 * (2*averageFingerPos() - (pinch[3] + pinch[4])); 
  //controls robot gripper position with just thumb and index finger

  //send_control(corrected_value); 
  //delayMicroseconds(delay_time); //tunable parameter to synchronize uart communication
}

void controller() { // state machine takes keyboard inputs 
  switch(state) {
    
    case 'a' : { // STOP!!
      stopMotors();
      Serial.println("Stopped");
      state = 'b';
      break;
    }

    case 'b' : { //Waiting 
      Serial.println("Waiting");
      printFingerPositions();
      printForce();
      delay(1000);
      lastState = state;
      break;
    }

    case 'c' : { // Calibrte Glove
      Serial.println("Calibrating Glove");
      //setupServos();
      calibration();
      //setupServos();
      delay(200);
      state = 'b';
      break;
    }

    case 'e' : { // Free following 
      
      V2FollowandForce();
      //Serial.println("Following");
      lastState = state;
      break;
    }

    case 'f' : { // Robot Control
      robotControl();
      lastState = state;
      break;
    }

    case 'i' : { // Increase Time Delay
      delay_time += 10;
      Serial.print("increasing time delay to ");
      Serial.print(delay_time);
      Serial.println(" microseconds");
      state = lastState;;
      break;
    }  

    case 'o' : { // Decrease Time Delay
      delay_time -= 10;
      if(delay_time < 11){
        delay_time = 10;
        Serial.println("Time delay is ~ Zero");
      }
      else {
        Serial.print("decreasing time delay to ");
        Serial.print(delay_time);
        Serial.println(" microseconds");
      }
      state = lastState;;
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
  //Serial2.begin(115200, SERIAL_8N1,RXp2,TXp2);
  setupMotors();
  //initializeMovingAvg();
  delay(1000);
}

void loop() {

  //printForce();
  // testMotors(2); // 2ms delay time sets speed of ramp up
  // printFingerPositions();
  // delay(100);
  
  
  //followFingersV2(); //Just move with finger motion, nothing else
  // printForce();
  // delay(100);
  simulate(); // Calls James's simulation interface
  force_message_reciever_two();
  // delay(10);

  
  /************** Use for access to state machine controller *********
  if (Serial.available() > 0) {                                      
    state = Serial.read();                                  
  }
  controller();
  ********************************************************************/
}