/*
 * Remote Feelings: Adam Curtis, Aymeric Wang, Xinying Hu
 * 11/30/21
 * Prototype version. Author: Adam Curtis, Aymeric Wang
 */


/*
  see how pwm is working
    - directky or through another component
  filter out?
  is it every finger?
  
*/

#include <Arduino.h>
#include "calibration.h"
#include "pin_config.h"
#include "sender.h"
#include "glove_api.h"
#ifndef SENDER_H
#include "reciever.h"
#endif
#ifndef MOTIONFUNKS_H
#include "MotionFuncs.h"
#endif

// Start by calibrating the glove with the finger skeleton pieces removed. 
// Change CALIBRATION to 1 below and follow directions in the manualCalibration() function

#define CALIBRATION 1// Set to 1 to enter manual calibration sequence. 0 for normal operation
int* return_force;
int* finger_location_val;
int step = 4;
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
  int step = 5;
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
    //followFingersV2(); //Just move with your finger motion, nothing else
    receive_ros_message();
  }

  else {
    Serial.println("Follow the instruction in the manualCalibration() function in main.cpp");
    Serial.println("Make sure to set your user in pin_configs.h");
    Serial.println("");
    delay(2000);
  }
  /* ------------------------- end manual calibration ------------------------- */  

}
void interactiveCalibration(){
  bool motors_delay = true;
  char response;
  bool enter = 0;
  //step = 4;
  /* --------------------------- autiomatic calibration --------------------------- */
  // ===STEP ONE=== 
  //  a. DO NOT HAVE THE FINGER SKELTON CONNECTED TO MOTOR OUTPUT SHAFTS
  //  b. Set step = 1 to make sure your motors and encoders work.
  // This will make the motors spin backward and forward to make sure the motor drivers are working correctly.
  //  c. printFingerPosition() function makes sure the encoder in the motor works too. These numbers 
  // should be changing when the motor moves
  
  //Start interactive calibration
  switch(step){
    case 0:{
      Serial.println("Calibration is ready");
      Serial.println("Press Enter to begin");
    
      while(!enter){
        while(!Serial.available()){} // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 1;
          enter = 1;
        }
      }
    }
    case 1:{
      if (motors_delay){
        for(int i = 0; i < 10; i++){
          Serial.println("WARNING, THE MOTORS ARE GOING TO MOVE A LOT!");
          delay(1000);
        }
        motors_delay = false;
      }
      //testMotors(2); // function argument = ms delay. 2ms delay time sets a good speed of ramp up
      printFingerPositions();
      delay(100);
      step = 2;
    
      //   -----------------------------------------------------------------------------------
      enter = 0;
      Serial.println("Step 1 Complete...Press Enter to Start Step 2");
      while(!enter){
        while(!Serial.available()){} // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 2;
          enter = 1;
        }
      }
    }

    // ===STEP TWO===
    //  a. Set step = 2 to enable printForce() function and upload.
    //  b. Push on the motors and make sure the numbers being printed are changing.
    //  c. Stop pushing on the motors and let them sit naturally. Enter the resting values in the calibration header file.
    case 2:{
      Serial.println("Starting Step 2...Push on the motors and make sure the numbers being printed are changing");
      Serial.println("Press Enter when complete");
      enter = 0;
      delay(1000);
      while(!enter){
        while(!Serial.available()){
          for(int i = 0; i < 5; i++){
            return_force = readForce();
            delay(100);
          }
        } // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          //step = 2;
          enter = 1;
        }
      }
      enter = 0;
      Serial.println("Stop pushing on the motors and let them sit naturally");
      Serial.println("Press Enter when complete");
      // busy wait until the user presses Enter
      while(!enter){
        while(!Serial.available()){}
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 3;
          enter = 1;
        }
      }
      return_force = readForce(); //get the rest force once the user is ready
      Serial.println("Enter the following numbers under restForce into your profile (ie. index 0 = below )");
      Serial.println(return_force[0]);
      Serial.println(return_force[1]);
      Serial.println(return_force[2]);
      Serial.println(return_force[3]);
      Serial.println(return_force[4]);
    }   
  

    //   ----------------------------------------------------------------------------
    // ===STEP THREE===
    //  a. Set step = 3
    //  b. Rotate the output shafts of the servos so that the full range of finger motion does not cross the  
    // discontinuity between highest and lowest value. Attach the physical hardware fingers to the glove and 
    // verify that you don't hit zero or 4096 in normal finger ranges
    case 3:{
      enter = 0;
      Serial.println("Step 2 Complete...Press Enter to Start Step 3");
      while(!enter){
        while(!Serial.available()){} // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 3;
          enter = 1;
        }
      }
      Serial.println("Rotate the output shafts of the servos so that the full range of finger motion does not cross the  discontinuity between highest and lowest value. Attach the physical hardware fingers to the glove and verify that you don't hit zero or 4096 in normal finger ranges");
      delay(5000);
      Serial.println("Press Enter when complete");
      enter = 0;
      while(!enter){
        while(!Serial.available()){
          printFingerPositions(); //prints out the location of each finger. 
          delay(200);
        } // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 3;
          enter = 1;
        }
      }
    }
    case 4:{
      enter = 0;
      Serial.println("Step 3 Complete...Press Enter to Start Step 4");
      while(!enter){
        while(!Serial.available()){} // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 4;
          enter = 1;
        }
      }
      Serial.println("Step 4: Put on the glove, and make a fist");
      Serial.println("Press Enter when complete");
      delay(2000);
      enter = 0;
      //if (step == 4) {
      while(!enter){
        while(!Serial.available()){
          //printFingerPositions(); 
          //delay(100);
        } // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 3;
          enter = 1;
        }
      }
    
      Serial.println("Enter the following numbers under fingerPosMin into your profile (ie. index 0 = below )");
      printFingerPositions();
      enter = 0;
      Serial.println("Press Enter to continue");
      while(!enter){
        while(!Serial.available()){} // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 4;
          enter = 1;
        }
      }
      Serial.println("Step 4: Now open your hand up all the way so your hand is flat");
      Serial.println("Press Enter when complete");
      enter = 0;
      while(!enter){
        while(!Serial.available()){} // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 4;
          enter = 1;
        }
      }
      Serial.println("Enter the following numbers under finerPosMax into your profile (ie. index 0 = below )");
      printFingerPositions();

      // ===STEP FOUR=== 
      //  a. With the same function running. Put on the glove, and curl each finger into your palm.
      //    i. Record this number in the fingerPosMin[] array starting with pinky, going to thumb
      //  b. Now extend each finger to its maximum open position
      //    i. Record this number in the fingerPosMax[] array starting with pinky, going to thumb

      // NOTE*** MIN can be higher or lower than MAX and vice versa. This is normal.

      // STEP FIVE. Set step = 5 to enable followFingersV2 function, upload, and move your fingers. 
      // The motors should help drive the exoskeleton to follow your fingers
      enter = 0;
      Serial.println("Press Enter to continue");
      while(!enter){
        while(!Serial.available()){} // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 5;
          enter = 1;
        }
      }
    } 
    case 5:{
      enter = 0;
      Serial.println("Recompile code with values from step 2,3,4 in calibration profile");
      Serial.println("Press Enter if code has been recompiled");
      while(!enter){
        while(!Serial.available()){} // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 5;
          enter = 1;
        }
      }
      Serial.println("Step 5: Move your fingers and the glove should follow your movements");
      Serial.println("Press Enter to finish after testing movements");
      enter = 0;
      while(!enter){
        while(!Serial.available()){
          followFingersV2();
        } // wait for user to press a button
        response = Serial.read();
        // enter is 13 decimal
        if(response == 13){
          step = 5;
          enter = 1;
        }
      }
      step = 5;
      Serial.println("Calibration is Complete");
    }
  }
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
  interactiveCalibration(); // new interactive calibration technique
  #endif

  #if CALIBRATION == 2
  manualCalibration(); // original method of calibration
  #endif
  /* ------------------------- end manual calibration ------------------------- */

  /* ------------------- Connect to VR or a robot over UART ------------------- */
  #if CALIBRATION == 0
  //receive_ros_message(); //listens for ros message -> used during testing on 5/8 
  #endif
  /* ------------------------- end VR or robot control ------------------------ */

  /* --------------- Use for access to state machine controller --------------- */
  // controller();
  /* ------------------------- end state machine code ------------------------- */
}