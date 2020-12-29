#include "ALU.h"

#include <Arduino.h>

namespace ALU {

   u8 leftValue, rightValue;
   Operator op;
   LeftRightSelector rlSel;
   u8 setBus;
   bool specifiedCarry;
   bool useSpecifiedCarry;
   bool latchOne, latchTwo;
   RegisterSelector regSel;
   InputSelector inputSel;
   bool outputToBus;
   u8 getBus;
   u8 result;
   bool carry;
   bool zero;
   bool sign;
   bool interrupt;

   void SetLeftValue(u8 value){
      leftValue = value;
   }

   void SetRightValue(u8 value){
      rightValue = value;
   }

   void SetOperator(Operator aOp){
      op = aOp;
   }

   void SetLeftRightSelector(LeftRightSelector selector){
      rlSel = selector;
   }

   void SetBus(u8 value){
      setBus = value;
   }

   void SetSpecifiedCarry(bool carry){
      specifiedCarry = carry;
   }

   void SetUseSpecifiedCarry(bool use){
      useSpecifiedCarry = use;
   }

   void SetLatchOne(bool latch){
      latchOne = latch;
   }

   void SetLatchTwo(bool latch){
      latchTwo = latch;
   }

   void SetRegisterSelector(RegisterSelector selector){
      regSel = selector;
   }

   void SetInputSelector(InputSelector selector){
      inputSel = selector;
   }

   void SetOutputToBus(bool output){
      outputToBus = output;
   }

   u8 GetResult(){
      return result;
   }

   u8 GetBus(){
      return getBus;
   }

   bool GetCarry(){
      return carry;
   }

   bool GetZero(){
      return zero;
   }

   bool GetSign(){
      return sign;
   }

   bool GetInterrupt(){
      return interrupt;
   }

   void Sample(){

      constexpr u8 busPins[8] = { 28, 27, 26, 25, 24, 23, 22, 21 };
      constexpr u8 leftPins[8] = { 19, 18, 17, 16, 15, 14, 13, 12 };
      constexpr u8 rightPins[8] = { 11, 10, 9, 8, 7, 6, 5, 4 };
      constexpr u8 opPins[3] = { 2, 1, 0 };
      constexpr u8 rlSelPin = 3;
      constexpr u8 specifiedCarryPin = 29;
      constexpr u8 useSpecifiedCarryPin = 30;
      constexpr u8 latchOnePin = 34;
      constexpr u8 latchTwoPin = 33;
      constexpr u8 regSelPin = 35;
      constexpr u8 inputSelPin = 31;
      constexpr u8 outputToBusPin = 32;
      constexpr u8 resultPins[8] = { 47, 46, 45, 44, 43, 42, 41, 40 };
      constexpr u8 carryPin = 39;
      constexpr u8 zeroPin = 38;
      constexpr u8 signPin = 37;
      constexpr u8 interruptPin = 36;

      //Write the dirs
      bool theBoardWillWriteToTheBus = outputToBus;
      //bool theBoardWillWriteToTheBus = true;
      if (theBoardWillWriteToTheBus){
         for(int i = 0; i < 8; i ++) SetDir(busPins[i], INPUT);
      }
      for(int i = 0; i < 8; i ++) SetDir(leftPins[i], OUTPUT);
      for(int i = 0; i < 8; i ++) SetDir(rightPins[i], OUTPUT);
      for(int i = 0; i < 8; i ++) SetDir(resultPins[i], INPUT);
      for(int i = 0; i < 3; i ++) SetDir(opPins[i], OUTPUT);
      SetDir(rlSelPin, OUTPUT);
      SetDir(specifiedCarryPin, OUTPUT);
      SetDir(useSpecifiedCarryPin, OUTPUT);
      SetDir(regSelPin, OUTPUT);
      SetDir(inputSelPin, OUTPUT);
      SetDir(outputToBusPin, OUTPUT);
      SetDir(carryPin, INPUT);
      SetDir(zeroPin, INPUT);
      SetDir(signPin, INPUT);
      SetDir(interruptPin, INPUT);
      SetDir(latchOnePin, OUTPUT);
      SetDir(latchTwoPin, OUTPUT);

      //Write the values
      for(int i = 0; i < 8; i ++) SetValue(leftPins[i], (leftValue & (1 << i)) ? 1 : 0);
      for(int i = 0; i < 8; i ++) SetValue(rightPins[i], (rightValue & (1 << i)) ? 1 : 0);
      for(int i = 0; i < 3; i ++) SetValue(opPins[i], (op & (1 << i)) ? 1 : 0);
      SetValue(rlSelPin, rlSel == RIGHT);
      SetValue(specifiedCarryPin, specifiedCarry);
      SetValue(useSpecifiedCarryPin, useSpecifiedCarry);
      SetValue(regSelPin, regSel == TWO);
      SetValue(inputSelPin, inputSel == BUS);
      SetValue(outputToBusPin, !outputToBus);
      SetValue(latchOnePin, latchOne);
      SetValue(latchTwoPin, latchTwo);

      Flush();

      if (!theBoardWillWriteToTheBus){
         for(int i = 0; i < 8; i ++) SetDir(busPins[i], OUTPUT);
         for(int i = 0; i < 8; i ++) SetValue(busPins[i], (setBus & (1 << i)) ? 1 : 0);
      }

      Flush();

      //Get results
      result = 0;
      for(int i = 0; i < 8; i ++) result |= GetValue(resultPins[i]) << i;
      if (theBoardWillWriteToTheBus){
         getBus = 0;
         for(int i = 0; i < 8; i ++) getBus |= GetValue(busPins[i]) << i;
      }
      carry = GetValue(carryPin);
      zero = GetValue(zeroPin);
      sign = GetValue(signPin);
      interrupt = GetValue(interruptPin);
   }
}