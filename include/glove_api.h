#ifndef GLOVE_API_H
#define GLOVE_API_H


#include <Arduino.h>
#include <ESP32Servo.h>
//#include "reciever.h"

int new_finger_forces[5];
/*void setup() {
  Serial.begin(115200);
  setupMotors();
  delay(1000);
  Serial.println("Setup complete");
}*/

//might need this for sending glove encoder positions
void send_ros_message(){



}
int* force_message_parser_fingers(String message){
    char force_message[50];
    char character;
    char commas_delimit = ',';
    char msg_delimiter_init = '<';
    char msg_delimiter_end = '>';
    int i = 0;
    bool comma = 0;
    bool startDelim = 0;
    bool sendMessage = 0; //reached delimiter
    static int robotForce = 0;
    static int robotForceTemp = 0;
    int* force_values = (int*)(malloc(sizeof(int) * 5));
    int force_int = 0;
    for(int finger = 0; finger < 5; finger++){
        comma = 0;
        force_int = 0;
        while(!comma & !sendMessage){
          character = message[i];
          //Serial.println(character);
          //Serial.println(i);
          //Serial.println();
          if (character == msg_delimiter_init){
              startDelim = 1;
              //Serial.println("here1");
          }
          else if (character == msg_delimiter_end){
              sendMessage = 1;
              force_message[force_int] = '\0';
          }
          else if (character == commas_delimit){
             // Serial.println("comma");
              comma = 1;
              force_message[force_int] = '\0';
              //i = 0;//reset to get next finger values
          }
          else { //not any delimiters
              force_message[force_int] = character;
              force_int++;
              //Serial.println("here2");
          }
          i++;
            //while(!Serial2.available()); //wait until there is more serial data available. 
            //Without this, sometimes the processor clears the UART buffer before all contents have arrived. 
        }
        force_values[finger] = atoi(force_message);
        //Serial.println(atoi(force_message));
    }/*
    if (robotForceTemp == 0){
        robotForce--;
        if(robotForce<0)
            robotForce = 0;
    }
    else
        robotForce = robotForceTemp;*/
    //Serial.println(robotForce);
    return(force_values);
}

//receive topic message from ros 

void receive_ros_message(){
  //Serial.println("Listening to force messages...");
  String ros_message;
  char message_array[50];
  int* received_force;
  bool received_end = false;
  bool received_start = false;
  int i = 0;
  char character;
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

    ros_message = message_array;
    Serial.println(ros_message);
    // force_message_parser_fingers returns a 5 index array
    received_force = force_message_parser_fingers(ros_message);
    //force_message_parser_fingers(ros_message);
    for(int i = 0; i < 5; i++)
      Serial.println(received_force[i]);
      new_finger_forces[i] = received_force[i]; 
      // Clear the UART buffer
    while(Serial.available()){
      Serial.read();
    }
}



#endif