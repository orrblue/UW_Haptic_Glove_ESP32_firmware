/*
 * Remote Feelings: Adam Curtis, Aymeric Wang, Xinying Hu
 * 11/30/21
 * Prototype version. Author: Aymeric Wang
 */

#ifndef SENDER_H
#define SENDER_H

#include <Arduino.h>
#include <Wire.h>


void message_send(String message){
  String formatted_message;
  formatted_message ="<"+message+">";
  Serial.print(formatted_message);
  //Serial.println(formatted_message);
  //Serial.println("I sent message:\t"+String(formatted_message));
}

void send_control(int little, int ring, int middle, int index, int thumb) {
  little = map(little, 400, 1200, 0, 255);
  ring = map(ring, 650, 1500, 0, 255);
  middle = map(middle, 2200, 1000, 0, 255);
  index = map(index, 1300, 275, 0, 255);
  thumb = map(thumb, 1600, 550, 0, 255);
  if(little > 255) little = 255;
  if(ring > 255) ring = 255;
  if(middle > 255) middle = 255;
  if(index > 255) index = 255;
  if(thumb > 255) thumb = 255;

  if(little < 0) little = 0;
  if(ring < 0) ring = 0;
  if(middle < 0) middle = 0;
  if(index < 0) index = 0;
  if(thumb < 0) thumb = 0;


  Serial.print(index);
  Serial.print(",");
  Serial.print(middle);
  Serial.print(",");
  Serial.print(ring);
  Serial.print(",");
  Serial.print(little);
  Serial.print(",");
  Serial.print(thumb);
  Serial.println("");
}

#endif