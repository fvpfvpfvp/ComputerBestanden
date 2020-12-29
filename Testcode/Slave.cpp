#ifndef MASTER

#include <Arduino.h>
#include <Wire.h>

#include "Interface.h"

u8 nextRequestResultSize = 0;
u8 nextRequestResult[3];

void TWIReceive(int byteCount){

   if (byteCount == 0) {
      return;
   }

   u8 message = Wire.read();

   switch(message){
      case M_SET_DIR_ALL: {
         u8 dirD = Wire.read();
         u8 dirB = Wire.read();
         u8 dirC = Wire.read();

         DDRB = dirB;
         DDRC = dirC;
         DDRD = dirD;
      } break;
      case M_SET_DIR_SINGLE: {
         u8 bit = Wire.read();
         u8 value = Wire.read() ? 1 : 0;
         if (bit >= 0 && bit <= 7){
            DDRD = (DDRD & ~(1 << bit)) | (value << bit);
         }
         else if (bit >= 8 && bit <= 15){
            DDRB = (DDRB & ~(1 << (bit - 8))) | (value << (bit - 8));
         }
         else if (bit >= 16 && bit <= 19){
            DDRC = (DDRC & ~(1 << (bit - 16))) | (value << (bit - 16));
         }
      } break;
      case M_SET_VALUE_ALL: {
         u8 valD = Wire.read();
         u8 valB = Wire.read();
         u8 valC = Wire.read();

         PORTB = valB;
         PORTC = valC;
         PORTD = valD;
      } break;
      case M_SET_VALUE_SINGLE: {
         u8 bit = Wire.read();
         u8 value = Wire.read() ? 1 : 0;
         if (bit >= 0 && bit <= 7){
            PORTD = (PORTD & ~(1 << bit)) | (value << bit);
         }
         else if (bit >= 8 && bit <= 15){
            PORTB = (PORTB & ~(1 << (bit - 8))) | (value << (bit - 8));
         }
         else if (bit >= 16 && bit <= 19){
            PORTC = (PORTC & ~(1 << (bit - 16))) | (value << (bit - 16));
         }
      } break;
      case M_GET_VALUE_ALL: {
         nextRequestResult[0] = PIND;
         nextRequestResult[1] = PINB;
         nextRequestResult[2] = PINC;
         nextRequestResultSize = 3;
      } break;
      case M_GET_VALUE_SINGLE: {
         u8 bit = Wire.read();
         if (bit >= 0 && bit <= 7){
            nextRequestResult[0] = (PIND & (1 << bit)) ? 1 : 0;
         }
         else if (bit >= 8 && bit <= 15){
            nextRequestResult[0] = (PINB & (1 << (bit - 8))) ? 1 : 0;
         }
         else if (bit >= 16 && bit <= 19){
            nextRequestResult[0] = (PINC & (1 << (bit - 16))) ? 1 : 0;
         }
         nextRequestResultSize = 1;
      } break;
      default: {
         for(int i = 1; i < byteCount; i ++) Wire.read();
      } break;
   }
}

void TWIRequest(){

   for(int i = 0; i < nextRequestResultSize; i ++){

      Wire.write(nextRequestResult[i]);
   }
}

void setup(){

   Wire.begin(SLAVE_ID);
   Wire.onReceive(TWIReceive);
   Wire.onRequest(TWIRequest);

   while(1) {
      delay(100);
   }
}

void loop();

#endif //!MASTER