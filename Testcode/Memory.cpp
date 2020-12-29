#include "Memory.h"

#include <Arduino.h>

namespace Memory {

   u8 setBus;
   bool incPC;
   bool pcLowOE, pcHighOE, pcLowLE, pcHighLE, arLowLE, arHighLE;
   ReadWrite rw;
   bool outputMem;
   AddressSelector addrSel;
   bool reset;
   bool prefetchLE;

   u8 getBus;
   u8 prefetchReg;
   bool linkOE;
   LinkDir linkDir;

   void SetBus(u8 value){
      setBus = value;
   }

   void SetIncrementPC(bool increment){
      incPC = increment;
   }

   void SetOutputPCLow(bool output){
      pcLowOE = output;
   }

   void SetOutputPCHigh(bool output){
      pcHighOE = output;
   }

   void SetLatchPCLow(bool latch){
      pcLowLE = latch;
   }

   void SetLatchPCHigh(bool latch){
      pcHighLE = latch;
   }

   void SetLatchARLow(bool latch){
      arLowLE = latch;
   }

   void SetLatchARHigh(bool latch){
      arHighLE = latch;
   }

   void SetReadWrite(ReadWrite readWrite){
      rw = readWrite;
   }

   void SetOutputMemory(bool output){
      outputMem = output;
   }

   void SetAddressSelector(AddressSelector selector){
      addrSel = selector;
   }

   void SetLatchPrefetchRegister(bool latch){
      prefetchLE = latch;
   }

   void SetReset(bool aReset){
      reset = aReset;
   }

   u8 GetPrefetchRegister(){
      return prefetchReg;
   }

   u8 GetBus(){
      return getBus;
   }

   bool GetMemoryLinkOE(){
      return linkOE;
   }

   LinkDir GetMemoryLinkDir(){
      return linkDir;
   }

   void Sample(){

      constexpr u8 busPins[8] = { 47, 46, 45, 44, 43, 42, 41, 40 };
      constexpr u8 incPCPin = 14;
      constexpr u8 pcLowOEPin = 15;
      constexpr u8 pcHighOEPin = 18;
      constexpr u8 pcLowLEPin = 11;
      constexpr u8 pcHighLEPin = 12;
      constexpr u8 arLowLEPin = 16;
      constexpr u8 arHighLEPin = 19;
      constexpr u8 rwPin = 39;
      constexpr u8 outputMemPin = 38;
      constexpr u8 addrSelPin = 17;
      constexpr u8 resetPin = 13;
      constexpr u8 prefetchLEPin = 8;
      constexpr u8 prefetchRegPins[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };
      constexpr u8 linkOEPin = 36;
      constexpr u8 linkDirPin = 37;

      constexpr u8 clockPin = 9;
      constexpr u8 twiceClockPin = 10;

      bool theBoardWillWriteToTheBus = ((rw == READ && outputMem) || pcLowOE || pcHighOE) && !prefetchLE;

      //Write dirs
      if (theBoardWillWriteToTheBus){
         for(int i = 0; i < 8; i ++) SetDir(busPins[i], INPUT);
      }
      SetDir(incPCPin, OUTPUT);
      SetDir(pcLowOEPin, OUTPUT);
      SetDir(pcHighOEPin, OUTPUT);
      SetDir(pcLowLEPin, OUTPUT);
      SetDir(pcHighLEPin, OUTPUT);
      SetDir(arLowLEPin, OUTPUT);
      SetDir(arHighLEPin, OUTPUT);
      SetDir(rwPin, OUTPUT);
      SetDir(outputMemPin, OUTPUT);
      SetDir(addrSelPin, OUTPUT);
      SetDir(resetPin, OUTPUT);
      SetDir(prefetchLEPin, OUTPUT);
      for(int i = 0; i < 8; i ++) SetDir(prefetchRegPins[i], INPUT);
      SetDir(linkOEPin, INPUT);
      SetDir(linkDirPin, INPUT);

      SetDir(clockPin, OUTPUT);
      SetDir(twiceClockPin, OUTPUT);

      //Write values
      SetValue(incPCPin, incPC);
      SetValue(pcLowOEPin, !pcLowOE);
      SetValue(pcHighOEPin, !pcHighOE);
      SetValue(pcLowLEPin, pcLowLE);
      SetValue(pcHighLEPin, pcHighLE);
      SetValue(arLowLEPin, arLowLE);
      SetValue(arHighLEPin, arHighLE);
      SetValue(rwPin, rw == READ);
      SetValue(outputMemPin, !outputMem);
      SetValue(addrSelPin, addrSel == AR);
      SetValue(resetPin, !reset);
      SetValue(prefetchLEPin, prefetchLE);

      SetValue(clockPin, false);
      SetValue(twiceClockPin, false);
      Flush();

      //Write bus
      if (!theBoardWillWriteToTheBus){
         for(int i = 0; i < 8; i ++) SetDir(busPins[i], OUTPUT);
         for(int i = 0; i < 8; i ++) SetValue(busPins[i], (setBus & (1 << i)) ? 1 : 0);
      }

      SetValue(clockPin, true);
      SetValue(twiceClockPin, false);
      Flush();

      SetValue(clockPin, true);
      SetValue(twiceClockPin, true);
      Flush();

      SetValue(clockPin, true);
      SetValue(twiceClockPin, false);
      Flush();

      //Get results
      prefetchReg = 0;
      for(int i = 0; i < 8; i ++) prefetchReg |= GetValue(prefetchRegPins[i]) << i;
      if (theBoardWillWriteToTheBus){
         getBus = 0;
         for(int i = 0; i < 8; i ++) getBus |= GetValue(busPins[i]) << i;
      }
      linkOE = !GetValue(linkOEPin);
      linkDir = GetValue(linkDirPin) ? TO_MEMORY : TO_CE;

      SetValue(clockPin, false);
      SetValue(twiceClockPin, false);
      Flush();

      SetValue(clockPin, false);
      SetValue(twiceClockPin, true);
      Flush();

      SetValue(clockPin, false);
      SetValue(twiceClockPin, false);
      Flush();
   }
}