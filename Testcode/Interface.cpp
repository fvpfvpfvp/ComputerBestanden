#include "Interface.h"

#include "Wire.h"

#include <Arduino.h>

bool dirs[60];
bool values[60];

void SetDir(u8 pin, bool dir){

   dirs[pin] = dir;
}

void SetValue(u8 pin, bool value){

   values[pin] = value;
}

bool GetValue(u8 pin){

   return values[pin];
}

void Flush(){

   //Write dir
   for(int i = 0; i < 60; i += 20){

      Wire.beginTransmission(i / 20);
      u8 a = 0, b = 0, c = 0;
      for(int j = 0; j < 20; j ++){
         if (dirs[i + j]){
            if (j < 8){
               a |= 1 << j;
            }
            else if (j < 16){
               b |= 1 << (j - 8);
            }
            else {
               c |= 1 << (j - 16);
            }
         }
      }
      Wire.write(M_SET_DIR_ALL);
      Wire.write(a);
      Wire.write(b);
      Wire.write(c);
      Wire.endTransmission();

      delayMicroseconds(10);
   }

   //Write values
   for(int i = 0; i < 60; i += 20){

      Wire.beginTransmission(i / 20);
      u8 a = 0, b = 0, c = 0;
      for(int j = 0; j < 20; j ++){
         if (values[i + j]){
            if (j < 8){
               a |= 1 << j;
            }
            else if (j < 16){
               b |= 1 << (j - 8);
            }
            else {
               c |= 1 << (j - 16);
            }
         }
      }
      Wire.write(M_SET_VALUE_ALL);
      Wire.write(a);
      Wire.write(b);
      Wire.write(c);
      Wire.endTransmission();

      delayMicroseconds(10);
   }

   //Get values
   for(int i = 0; i < 60; i += 20){

      Wire.beginTransmission(i / 20);
      Wire.write(M_GET_VALUE_ALL);
      Wire.endTransmission();

      delayMicroseconds(10);

      Wire.requestFrom(i / 20, 3);
      u8 a = Wire.read();
      u8 b = Wire.read();
      u8 c = Wire.read();

      for(int j = 0; j < 8; j ++){
         values[i + j] = (a & (1 << j)) ? 1 : 0;
      }

      for(int j = 0; j < 8; j ++){
         values[i + 8 + j] = (b & (1 << j)) ? 1 : 0;
      }

      for(int j = 0; j < 4; j ++){
         values[i + 16 + j] = (c & (1 << j)) ? 1 : 0;
      }
   }
}