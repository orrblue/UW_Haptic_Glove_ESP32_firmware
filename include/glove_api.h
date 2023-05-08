#ifndef GLOVE_API_H
#define GLOVE_API_H


#include <Arduino.h>
#include <ESP32Servo.h>
#include "MotionFuncs.h"
//#include "reciever.h"

/* 
   Takes the string and parses it into forces for each finger
   Parameter:
        message : the input string from ros in the format of <1,2,3,4,5>

   Output:
      int array of index 5 where each index contains the force to apply on each finger



*/
int* force_message_parser_fingers(String message){
    char force_message[50]; //holds the numerical value for force -> gets overwritten for each finger...used as a temp array to use in atoi()
    char character;
    char commas_delimit = ',';
    char msg_delimiter_init = '<'; //start of packet
    char msg_delimiter_end = '>'; //end of packet
    int i = 0;
    bool comma = 0;
    bool startDelim = 0;
    bool sendMessage = 0; //reached delimiter
    int* force_values = (int*)(malloc(sizeof(int) * 5)); //use malloc to return the force values since it is important data
    int force_int = 0;
    for(int finger = 0; finger < 5; finger++){
        comma = 0;
        force_int = 0;
        while(!comma & !sendMessage){
          character = message[i]; //get each character in the message
          //look for the starting <
          if (character == msg_delimiter_init){ 
              startDelim = 1;
          }
          //look for the ending >
          else if (character == msg_delimiter_end){
              sendMessage = 1;
              force_message[force_int] = '\0'; //add null terminator to the end of the integer
          }
          //look for commas separating the forces
          else if (character == commas_delimit){
              comma = 1;
              force_message[force_int] = '\0';
          }
          else { //not any delimiters
              force_message[force_int] = character;
              force_int++; //iterator for the finger force value in characters -> starts at 0 for each finger
          }
          i++; //iterator for the message itself
        }
        force_values[finger] = atoi(force_message); //turn the string number to int number
    }
    return(force_values);
}

//receive topic message from ros 
// waits for there to be a ros message and parses the message into its respective packet
void receive_ros_message(){
  //Serial.println("Listening to force messages...");
  String ros_message;
  char message_array[50];
  int* received_force;
  bool received_end = false;
  bool received_start = false;
  int i = 0;
  char character;
  /*
    wait for the end < then send the string to force_message_parser_fingers()
  */
  while(!received_end){
    if(Serial.available()){
      character = Serial.read();
      if(!received_start){
        i = 0;
        if(character == '<'){
          received_start = true;
          message_array[i] = character;
          i++;
        }
      }
      else{
        message_array[i] = character;
        if(character == '>' ){
          received_end = true;
          message_array[i+1] = '\0';
        }
        else{
          i++;
        }
      }      
    }
  }

    ros_message = message_array; //change of names
    Serial.println(ros_message);
    // force_message_parser_fingers returns a int 5 index array
    received_force = force_message_parser_fingers(ros_message);
    applyTorque(received_force); //send forces to glove to act on the user
    while(Serial.available()){
      Serial.read();
    }
}



#endif