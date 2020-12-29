#include "CE.h"

#include <Arduino.h>

namespace CE {

   u8 aluResult;
   OESelector oeSel;
   LESelector leSel;
   RBusSelector rSel;
   u8 flagImmediate, immediate;
   bool linkOE;
   LinkDir linkDir;
   bool tempLE;
   u8 setALUBus;

   u8 lBus, rBus;
   bool pcLowOE, pcHighOE, pcLowLE, pcHighLE;
   u8 getALUBus;
   bool aluFlagsOE;
   InputSelector aluFlagsInputSelector;

   void SetALUResult(u8 value){
      aluResult = value;
   }

   void SetOESelector(OESelector selector){
      oeSel = selector;
   }

   void SetLESelector(LESelector selector){
      leSel = selector;
   }

   void SetRBusSelector(RBusSelector selector){
      rSel = selector;
   }

   void SetFlagImmediate(u8 flagIndex){
      flagImmediate = flagIndex;
   }

   void SetImmediate(u8 value){
      immediate = value;
   }

   void SetLinkOE(bool output){
      linkOE = output;
   }

   void SetLinkDir(LinkDir dir){
      linkDir = dir;
   }

   void SetTempLE(bool latch){
      tempLE = latch;
   }

   void SetALUBus(u8 value){
      setALUBus = value;
   }

   u8 GetLBus(){
      return lBus;
   }

   u8 GetRBus(){
      return rBus;
   }

   bool GetPCLowOE(){
      return pcLowOE;
   }

   bool GetPCHighOE(){
      return pcHighOE;
   }

   bool GetPCLowLE(){
      return pcLowLE;
   }

   bool GetPCHighLE(){
      return pcHighLE;
   }

   u8 GetALUBus(){
      return getALUBus;
   }

   bool GetALUFlagsOE(){
      return aluFlagsOE;
   }

   InputSelector GetALUFlagInputSelector(){
      return aluFlagsInputSelector;
   }

   void Sample(){

      constexpr u8 aluResultPins[8] = { 47, 46, 45, 44, 43, 42, 41, 40 };
      constexpr u8 oeSelPins[4] = { 26, 25, 24, 23 };
      constexpr u8 leSelPins[4] = { 30, 29, 28, 27 };
      constexpr u8 rSelPins[2] = { 52, 51 };
      constexpr u8 flagImmediatePins[3] = { 55, 54, 53 };
      constexpr u8 immediatePins[2] = { 20, 21 };
      constexpr u8 linkOEPin = 49;
      constexpr u8 linkDirPin = 50;
      constexpr u8 tempLEPin = 48;
      constexpr u8 aluBusPins[8] = { 38, 37, 36, 35, 34, 33, 32, 31 };
      constexpr u8 lBusPins[8] = { 19, 18, 17, 16, 15, 14, 13, 12 };
      constexpr u8 rBusPins[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };
      constexpr u8 pcLowOEPin = 11;
      constexpr u8 pcHighOEPin = 10;
      constexpr u8 pcLowLEPin = 9;
      constexpr u8 pcHighLEPin = 8;
      constexpr u8 aluFlagsOEPin = 39;
      constexpr u8 aluFlagsInputSelectorPin = 0;

      constexpr u8 clockPin = 22;

      bool theBoardWillWriteToTheALUBus = !aluFlagsOE;
      
      //Write dirs
      if (theBoardWillWriteToTheALUBus){
         for(int i = 0; i < 8; i ++) SetDir(aluBusPins[i], INPUT);
      }
      for(int i = 0; i < 8; i ++) SetDir(aluResultPins[i], OUTPUT);
      for(int i = 0; i < 4; i ++) SetDir(oeSelPins[i], OUTPUT);
      for(int i = 0; i < 4; i ++) SetDir(leSelPins[i], OUTPUT);
      for(int i = 0; i < 2; i ++) SetDir(rSelPins[i], OUTPUT);
      for(int i = 0; i < 3; i ++) SetDir(flagImmediatePins[i], OUTPUT);
      for(int i = 0; i < 2; i ++) SetDir(immediatePins[i], OUTPUT);
      SetDir(linkOEPin, OUTPUT);
      SetDir(linkDirPin, OUTPUT);
      SetDir(tempLEPin, OUTPUT);
      SetDir(clockPin, OUTPUT);

      for(int i = 0; i < 8; i ++) SetDir(lBusPins[i], INPUT);
      for(int i = 0; i < 8; i ++) SetDir(rBusPins[i], INPUT);
      SetDir(pcLowOEPin, INPUT);
      SetDir(pcHighOEPin, INPUT);
      SetDir(pcLowLEPin, INPUT);
      SetDir(pcHighLEPin, INPUT);
      SetDir(aluFlagsOEPin, INPUT);
      SetDir(aluFlagsInputSelectorPin, INPUT);
      
      //Write values
      for(int i = 0; i < 8; i ++) SetValue(aluResultPins[i], (aluResult & (1 << i)) ? 1 : 0);
      for(int i = 0; i < 4; i ++) SetValue(oeSelPins[i], ((int)oeSel & (1 << i)) ? 1 : 0);
      for(int i = 0; i < 4; i ++) SetValue(leSelPins[i], ((int)leSel & (1 << i)) ? 1 : 0);
      for(int i = 0; i < 2; i ++) SetValue(rSelPins[i], (rSel & (1 << i)) ? 1 : 0);
      for(int i = 0; i < 3; i ++) SetValue(flagImmediatePins[i], (flagImmediate & (1 << i)) ? 1 : 0);
      for(int i = 0; i < 2; i ++) SetValue(immediatePins[i], (i == 0 ? (immediate & (1 << 0)) : (immediate & (1 << 7))) ? 1 : 0);
      SetValue(linkOEPin, !linkOE);
      SetValue(linkDirPin, linkDir == TO_CE);
      SetValue(tempLEPin, tempLE);
      SetValue(clockPin, false);

      Flush();

      //Now write the alu bus
      if (!theBoardWillWriteToTheALUBus){
         for(int i = 0; i < 8; i ++) SetDir(aluBusPins[i], OUTPUT);
         for(int i = 0; i < 8; i ++) SetValue(aluBusPins[i], (setALUBus & (1 << i)) ? 1 : 0);
      }

      SetValue(clockPin, true);

      Flush();

      //Read the results
      if (theBoardWillWriteToTheALUBus){
         getALUBus = 0;
         for(int i = 0; i < 8; i ++) getALUBus |= GetValue(aluBusPins[i]) << i;
      }
      lBus = 0;
      for(int i = 0; i < 8; i ++) lBus |= GetValue(lBusPins[i]) << i;
      rBus = 0;
      for(int i = 0; i < 8; i ++) rBus |= GetValue(rBusPins[i]) << i;
      pcLowOE = !GetValue(pcLowOEPin);
      pcHighOE = !GetValue(pcHighOEPin);
      pcLowLE = GetValue(pcLowLEPin);
      pcHighLE = GetValue(pcHighLEPin);
      aluFlagsOE = !GetValue(aluFlagsOEPin);
      aluFlagsInputSelector = GENERATED;
      //aluFlagsInputSelector = GetValue(aluFlagsInputSelectorPin) ? GENERATED : BUS;

      SetValue(clockPin, false);

      Flush();
   }
}