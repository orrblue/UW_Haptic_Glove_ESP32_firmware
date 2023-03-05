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
#ifndef MOTIONFUNKS_H
#include "MotionFuncs.h"
#endif

// Start by calibrating the glove with the finger skeleton pieces removed. 
// Change CALIBRATION to 1 below and follow directions in the manualCalibration() function

#define CALIBRATION 1 // Set to 1 to enter manual calibration sequence. 0 for normal operation

void interactOverUART(void){
  /* --------- Interacts over UART sending and receiving 8 bit numbers -------- */

  force_message_reciever(); // Gets in new force values from VR or robot
  V2FollowandForce(); // Applies appropriate force to fingers
  readFingerPositions(); // Checks where the fingers are
  // Sends finger location to VR or robot 
  send_control();
}

void controller() { // state machine takes keyboard inputs 
  static int state = 'b'; //waiting
  static int lastState = 'b'; //waiting

  if (Serial.available() > 0) state = Serial.read(); // enter a letter to switch states

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

    case 'c' : { // Calibrate Glove
      Serial.println("Calibrating Glove");
      calibration();
      delay(200);
      state = 'b';
      break;
    }

    case 'e' : { // Free following 
      followFingersV2();
      lastState = state;
      break;
    }

    case 'f' : { // VR or Robot Control
      interactOverUART();
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

void manualCalibration(){
  int step = 0;
  bool motors_delay = true;
  /* --------------------------- manual calibration --------------------------- */
  // ===STEP ONE=== 
  //  a. DO NOT HAVE THE FINGER SKELTON CONNECTED TO MOTOR OUTPUT SHAFTS
  //  b. Set step = 1 to make sure your motors and encoders work.
  // This will make the motors spin backward and forward to make sure the motor drivers are working correctly.
  //  c. printFingerPosition() function makes sure the encoder in the motor works too. These numbers 
  // should be changing when the motor moves
  if (step == 1) {
    if (motors_delay){
      for(int i = 0; i < 10; i++){
        Serial.println("WARNING, THE MOTORS ARE GOING TO MOVE A LOT!");
        delay(1000);
      }
      motors_delay = false;
    }
    testMotors(2); // function argument = ms delay. 2ms delay time sets a good speed of ramp up
    printFingerPositions();
    delay(100);
  }
  
  // ===STEP TWO===
  //  a. Set step = 2 to enable printForce() function and upload.
  //  b. Push on the motors and make sure the numbers being printed are changing.
  //  c. Stop pushing on the motors and let them sit naturally. Enter the resting values in the calibration header file.
  else if (step == 2) {
    printForce();
    delay(100);
  }

  // ===STEP THREE===
  //  a. Set step = 3
  //  b. Rotate the output shafts of the servos so that the full range of finger motion does not cross the  
  // discontinuity between highest and lowest value. Attach the physical hardware fingers to the glove and 
  // verify that you don't hit zero or 4096 in normal finger ranges
  else if (step == 3 || step == 4) {
    printFingerPositions(); //prints out the location of each finger. 
    delay(100);
  }

  // ===STEP FOUR=== 
  //  a. With the same function running. Put on the glove, and curl each finger into your palm.
  //    i. Record this number in the fingerPosMin[] array starting with pinky, going to thumb
  //  b. Now extend each finger to its maximum open position
  //    i. Record this number in the fingerPosMax[] array starting with pinky, going to thumb

  // NOTE*** MIN can be higher or lower than MAX and vice versa. This is normal.

  // STEP FIVE. Set step = 5 to enable followFingersV2 function, upload, and move your fingers. 
  // The motors should help drive the exoskeleton to follow your fingers
  else if (step == 5) {
    followFingersV2(); //Just move with your finger motion, nothing else
  }

  else {
    Serial.println("Follow the instruction in the manualCalibration() function in main.cpp");
    Serial.println("Make sure to set your user in pin_configs.h");
    Serial.println("");
    delay(2000);
  }
  /* ------------------------- end manual calibration ------------------------- */  

}

void setup() {
	Serial.begin(115200);
  setupMotors();
  delay(1000);
  Serial.println("Setup complete");
}

void loop() {
  /* --------------------------- manual calibration --------------------------- */
  #if CALIBRATION == 1
  manualCalibration();
  #endif
  /* ------------------------- end manual calibration ------------------------- */

  /* ------------------- Connect to VR or a robot over UART ------------------- */
  #if CALIBRATION == 0
  interactOverUART(); // Calls James's simulation interface
  #endif
  /* ------------------------- end VR or robot control ------------------------ */

  /* --------------- Use for access to state machine controller --------------- */
  // controller();
  /* ------------------------- end state machine code ------------------------- */
}