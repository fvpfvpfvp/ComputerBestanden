#ifdef MASTER

#include <Arduino.h>
#include <Wire.h>

#include "Interface.h"
#include "Tester.h"

void setup(){

   Wire.begin();
   Serial.begin(1000000);

   Serial.println("Started");

   //TestALU();
   //TestMemory();
   TestCE();

   Serial.println("Done");
   
   while(1);
}

void loop(){}

#endif //MASTER