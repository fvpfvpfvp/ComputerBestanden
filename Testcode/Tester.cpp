#include "Tester.h"

#include "ALU.h"
#include "Memory.h"
#include "CE.h"

#include <Arduino.h>

#define PSTR(text) __extension__({ const char* PROGMEM v = text; v; })

int printf(const char* text, ...){

   char buf[256];
   va_list args;
   va_start(args, text);
   int result = vsnprintf(buf, sizeof(buf), text, args);
   va_end(args);
   Serial.print(buf);

   return result;
}

void TestALU(){

   //Initialize
   ALU::SetLeftRightSelector(ALU::RIGHT);
   ALU::SetLatchOne(false);
   ALU::SetLatchTwo(false);
   ALU::SetSpecifiedCarry(false);
   ALU::SetUseSpecifiedCarry(true);
   ALU::SetRegisterSelector(ALU::ONE);
   ALU::SetInputSelector(ALU::GENERATED);
   ALU::SetOutputToBus(false);

   //First test the ALU with all the operators.
   /*
   {
      Serial.println("Testing AND");
      ALU::SetOperator(ALU::AND);
      for(int left = 0; left < 256; ++left){
         for(int right = 0; right < 256; ++right){

            ALU::SetLeftValue(left);
            ALU::SetRightValue(right);

            ALU::Sample();

            auto result = ALU::GetResult();

            if (result != (left & right)){
               printf(PSTR("AND failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, left & right);
            }
         }
      }
   }

   {
      Serial.println("Testing OR");
      ALU::SetOperator(ALU::OR);
      for(int left = 0; left < 256; ++left){
         for(int right = 0; right < 256; ++right){

            ALU::SetLeftValue(left);
            ALU::SetRightValue(right);

            ALU::Sample();

            auto result = ALU::GetResult();

            if (result != (left | right)){
               printf(PSTR("OR failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, left | right);
            }
         }
      }
   }
   
   {
      Serial.println("Testing ADD");
      ALU::SetOperator(ALU::ADD);
      ALU::SetUseSpecifiedCarry(true);

      {
         ALU::SetSpecifiedCarry(false);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != ((left + right) & 0xFF)){
                  printf(PSTR("ADD failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, ((left + right) & 0xFF));
               }
            }
         }
      }

      {
         ALU::SetSpecifiedCarry(true);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != ((left + right + 1) & 0xFF)){
                  printf(PSTR("ADD failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, ((left + right + 1) & 0xFF));
               }
            }
         }
      }
   }

   {
      Serial.println("Testing subtract");
      ALU::SetOperator(ALU::SUBTRACT);
      ALU::SetUseSpecifiedCarry(true);

      {
         ALU::SetSpecifiedCarry(true);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != ((left - right) & 0xFF)){
                  printf(PSTR("SUB failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, ((left - right) & 0xFF));
               }
            }
         }
      }

      {
         ALU::SetSpecifiedCarry(false);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != ((left - right - 1) & 0xFF)){
                  printf(PSTR("SUB failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, ((left - right - 1) & 0xFF));
               }
            }
         }
      }
   }

   {
      Serial.println("Testing INVERT");
      ALU::SetOperator(ALU::INVERT);
      ALU::SetUseSpecifiedCarry(true);
      ALU::SetSpecifiedCarry(false);

      ALU::SetLeftRightSelector(ALU::RIGHT);
      for(int left = 0; left < 256; ++left){
         for(int right = 0; right < 256; ++right){

            ALU::SetLeftValue(left);
            ALU::SetRightValue(right);

            ALU::Sample();

            auto result = ALU::GetResult();

            if (result != (u8)~right){
               printf(PSTR("INVERT failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, ~right);
            }
         }
      }
      ALU::SetLeftRightSelector(ALU::LEFT);
      for(int left = 0; left < 256; ++left){
         for(int right = 0; right < 256; ++right){

            ALU::SetLeftValue(left);
            ALU::SetRightValue(right);

            ALU::Sample();

            auto result = ALU::GetResult();

            if (result != (u8)~left){
               printf(PSTR("INVERT failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, ~left);
            }
         }
      }
   }

   {
      Serial.println("Testing SHIFT_LEFT");
      ALU::SetOperator(ALU::SHIFT_LEFT);
      ALU::SetUseSpecifiedCarry(true);
      
      {
         ALU::SetSpecifiedCarry(false);
         ALU::SetLeftRightSelector(ALU::LEFT);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != (u8)(left << 1)){
                  printf(PSTR("SHL failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, left << 1);
               }
            }
         }
      }

      {
         ALU::SetSpecifiedCarry(true);
         ALU::SetLeftRightSelector(ALU::LEFT);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != (u8)((left << 1) + 1)){
                  printf(PSTR("SHL failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, (left << 1) + 1);
               }
            }
         }
      }

      {
         ALU::SetLeftRightSelector(ALU::RIGHT);
         ALU::SetSpecifiedCarry(false);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != (u8)(right << 1)){
                  printf(PSTR("SHL failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, right << 1);
               }
            }
         }
      }

      {
         ALU::SetLeftRightSelector(ALU::RIGHT);
         ALU::SetSpecifiedCarry(true);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != (u8)((right << 1) + 1)){
                  printf(PSTR("SHL failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, (right << 1) + 1);
               }
            }
         }
      }
   }

   {
      Serial.println("Testing SHIFT_RIGHT");
      ALU::SetOperator(ALU::SHIFT_RIGHT);
      ALU::SetUseSpecifiedCarry(true);

      {
         ALU::SetSpecifiedCarry(false);
         ALU::SetLeftRightSelector(ALU::LEFT);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != (u8)(left >> 1)){
                  printf(PSTR("SHR failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, left >> 1);
               }
            }
         }
      }

      {
         ALU::SetSpecifiedCarry(true);
         ALU::SetLeftRightSelector(ALU::LEFT);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != (u8)((left >> 1) + 0b10000000)){
                  printf(PSTR("SHR failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, (left >> 1) + 0b10000000);
               }
            }
         }
      }

      {
         ALU::SetSpecifiedCarry(false);
         ALU::SetLeftRightSelector(ALU::RIGHT);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != (u8)(right >> 1)){
                  printf(PSTR("SHR failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, right >> 1);
               }
            }
         }
      }

      {
         ALU::SetSpecifiedCarry(true);
         ALU::SetLeftRightSelector(ALU::RIGHT);
         for(int left = 0; left < 256; ++left){
            for(int right = 0; right < 256; ++right){

               ALU::SetLeftValue(left);
               ALU::SetRightValue(right);

               ALU::Sample();

               auto result = ALU::GetResult();

               if (result != (u8)((right >> 1) + 0b10000000)){
                  printf(PSTR("SHR failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, (right >> 1) + 0b10000000);
               }
            }
         }
      }
   }

   {
      Serial.println("Testing PASS");
      ALU::SetOperator(ALU::PASS);
      ALU::SetUseSpecifiedCarry(true);
      ALU::SetSpecifiedCarry(false);

      ALU::SetLeftRightSelector(ALU::LEFT);
      for(int left = 0; left < 256; ++left){
         for(int right = 0; right < 256; ++right){

            ALU::SetLeftValue(left);
            ALU::SetRightValue(right);

            ALU::Sample();

            auto result = ALU::GetResult();

            if (result != left){
               printf(PSTR("PASS failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, left);
            }
         }
      }

      ALU::SetLeftRightSelector(ALU::RIGHT);
      for(int left = 0; left < 256; ++left){
         for(int right = 0; right < 256; ++right){

            ALU::SetLeftValue(left);
            ALU::SetRightValue(right);

            ALU::Sample();

            auto result = ALU::GetResult();

            if (result != right){
               printf(PSTR("SHR failed. Left: %d, right: %d, result: %d, result should be: %d.\n"), left, right, result, right);
            }
         }
      }
   }
*/
   Serial.println("Testing trivia");

   //Now test ALU flags.
   { //Clear the flag registers
      ALU::SetInputSelector(ALU::BUS);
      ALU::SetBus(0);
      ALU::SetLatchOne(true);
      ALU::SetLatchTwo(true);
      ALU::SetOutputToBus(false);
      
      ALU::Sample();
   }

   {
      ALU::SetLatchOne(false);
      ALU::SetLatchTwo(false);
      ALU::SetOutputToBus(true);

      ALU::Sample();

      if (ALU::GetBus() != 0){
         printf(PSTR("BUS LE or OE failed. Set to zero, received %d.\n"), ALU::GetBus());
      }

      if (ALU::GetCarry() || ALU::GetZero() || ALU::GetSign() || ALU::GetInterrupt()){
         printf(PSTR("BUS LE or gen failed. C: %d, Z: %d, S: %d, I: %d.\n"), ALU::GetCarry(), ALU::GetZero(), ALU::GetSign(), ALU::GetInterrupt());
      }
   }

   { //Test reg 1
      //Test carry
      {
         ALU::SetInputSelector(ALU::BUS);
         ALU::SetBus(1); //Carry of reg 1
         ALU::SetLatchOne(true);
         ALU::SetOutputToBus(false);
         ALU::SetRegisterSelector(ALU::ONE);

         ALU::Sample();

         if (!ALU::GetCarry()){
            printf(PSTR("BUS LE or carry failed. Set to 1, received 0.\n"));
         }
      }

      //Test zero
      {
         ALU::SetInputSelector(ALU::BUS);
         ALU::SetBus(2); //Zero of reg 1
         ALU::SetLatchOne(true);
         ALU::SetOutputToBus(false);
         ALU::SetRegisterSelector(ALU::ONE);

         ALU::Sample();

         if (!ALU::GetZero()){
            printf(PSTR("BUS LE or zero failed. Set to 1, received 0.\n"));
         }
      }

      //Test sign
      {
         ALU::SetInputSelector(ALU::BUS);
         ALU::SetBus(4); //Sign of reg 1
         ALU::SetLatchOne(true);
         ALU::SetOutputToBus(false);
         ALU::SetRegisterSelector(ALU::ONE);

         ALU::Sample();

         if (!ALU::GetSign()){
            printf(PSTR("BUS LE or sign failed. Set to 1, received 0.\n"));
         }
      }

      //Test interrupt
      {
         ALU::SetInputSelector(ALU::BUS);
         ALU::SetBus(8); //Interrupt of reg 1
         ALU::SetLatchOne(true);
         ALU::SetOutputToBus(false);
         ALU::SetRegisterSelector(ALU::ONE);

         ALU::Sample();

         if (!ALU::GetInterrupt()){
            printf(PSTR("BUS LE or interrupt failed. Set to 1, received 0.\n"));
         }
      }
   }

   ALU::SetLatchOne(false);
   ALU::SetLatchTwo(false);

   { //Test reg 2
      //Test carry
      {
         ALU::SetInputSelector(ALU::BUS);
         ALU::SetBus(16); //Carry of reg 2
         ALU::SetLatchTwo(true);
         ALU::SetOutputToBus(false);
         ALU::SetRegisterSelector(ALU::TWO);

         ALU::Sample();

         if (!ALU::GetCarry()){
            printf(PSTR("BUS LE or carry failed. Set to 1, received 0.\n"));
         }
      }

      //Test zero
      {
         ALU::SetInputSelector(ALU::BUS);
         ALU::SetBus(32); //Zero of reg 2
         ALU::SetLatchTwo(true);
         ALU::SetOutputToBus(false);
         ALU::SetRegisterSelector(ALU::TWO);

         ALU::Sample();

         if (!ALU::GetZero()){
            printf(PSTR("BUS LE or zero failed. Set to 1, received 0.\n"));
         }
      }

      //Test sign
      {
         ALU::SetInputSelector(ALU::BUS);
         ALU::SetBus(64); //Sign of reg 2
         ALU::SetLatchTwo(true);
         ALU::SetOutputToBus(false);
         ALU::SetRegisterSelector(ALU::TWO);

         ALU::Sample();

         if (!ALU::GetSign()){
            printf(PSTR("BUS LE or sign failed. Set to 1, received 0.\n"));
         }
      }

      //Test interrupt
      {
         ALU::SetInputSelector(ALU::BUS);
         ALU::SetBus(128); //Interrupt of reg 2
         ALU::SetLatchTwo(true);
         ALU::SetOutputToBus(false);
         ALU::SetRegisterSelector(ALU::TWO);

         ALU::Sample();

         if (!ALU::GetInterrupt()){
            printf(PSTR("BUS LE or interrupt failed. Set to 1, received 0.\n"));
         }
      }
   }

   { //Test generated carry

      ALU::SetOperator(ALU::SHIFT_LEFT);
      ALU::SetLeftRightSelector(ALU::LEFT);
      ALU::SetLeftValue(0b10000000);
      ALU::SetInputSelector(ALU::GENERATED);
      ALU::SetRegisterSelector(ALU::ONE);
      ALU::SetLatchOne(true);

      ALU::Sample();

      if (!ALU::GetCarry()){
         printf(PSTR("Generated carry failed.\n"));
      }
   }

   { //Test generated zero

      ALU::SetOperator(ALU::PASS);
      ALU::SetLeftRightSelector(ALU::LEFT);
      ALU::SetLeftValue(0);
      ALU::SetInputSelector(ALU::GENERATED);
      ALU::SetRegisterSelector(ALU::ONE);
      ALU::SetLatchOne(true);

      ALU::Sample();

      if (!ALU::GetZero()){
         printf(PSTR("Generated zero failed.\n"));
      }
   }

   { //Test generated sign

      ALU::SetOperator(ALU::PASS);
      ALU::SetLeftRightSelector(ALU::LEFT);
      ALU::SetLeftValue(0b10000000);
      ALU::SetInputSelector(ALU::GENERATED);
      ALU::SetRegisterSelector(ALU::ONE);
      ALU::SetLatchOne(true);

      ALU::Sample();

      if (!ALU::GetSign()){
         printf(PSTR("Generated zero failed.\n"));
      }
   }
}

void TestMemory(){

   Memory::SetBus(0);
   Memory::SetIncrementPC(0);
   Memory::SetOutputPCLow(false);
   Memory::SetOutputPCHigh(false);
   Memory::SetLatchPCLow(false);
   Memory::SetLatchPCHigh(false);
   Memory::SetLatchARLow(false);
   Memory::SetLatchARHigh(false);
   Memory::SetReadWrite(Memory::READ);
   Memory::SetOutputMemory(false);
   Memory::SetAddressSelector(Memory::PC);
   Memory::SetLatchPrefetchRegister(false);

   Serial.println("Testing reset");
   { //Reset
      Memory::SetReset(true);
      Memory::Sample();
      Memory::SetReset(false);
   }
/*
   Serial.println("Testing PC increment");
   { //Test the PC increment

      for(u32 i = 0; i < 65536; ++i){

         Memory::SetOutputPCLow(true);
         Memory::Sample();
         Memory::SetOutputPCLow(false);
         auto low = Memory::GetBus();

         Memory::SetOutputPCHigh(true);
         Memory::Sample();
         Memory::SetOutputPCHigh(false);
         auto high = Memory::GetBus();

         if (low != (i & 0xFF)){
            printf(PSTR("PC low failed. Low: %d, should be: %d.\n"), low, i & 0xFF);
         }

         if (high != (i >> 8)){
            printf(PSTR("PC high failed. High: %d, should be: %d.\n"), high, i >> 8);
         }

         Memory::SetIncrementPC(true);
         Memory::Sample();
         Memory::SetIncrementPC(false);
      }
   }

   Serial.println("Testing PC latch enable");
   { //Test the PC latch enable
      
      for(u32 i = 0; i < 65536; ++i){

         Memory::SetBus(i & 0xFF);
         Memory::SetLatchPCLow(true);
         Memory::Sample();
         Memory::SetLatchPCLow(false);

         Memory::SetBus(i >> 8);
         Memory::SetLatchPCHigh(true);
         Memory::Sample();
         Memory::SetLatchPCHigh(false);

         Memory::SetOutputPCLow(true);
         Memory::Sample();
         Memory::SetOutputPCLow(false);
         auto low = Memory::GetBus();

         Memory::SetOutputPCHigh(true);
         Memory::Sample();
         Memory::SetOutputPCHigh(false);
         auto high = Memory::GetBus();

         if (low != (i & 0xFF)){
            printf(PSTR("PC low failed. Low: %d, should be: %d.\n"), low, i & 0xFF);
         }

         if (high != (i >> 8)){
            printf(PSTR("PC high failed. High: %d, should be: %d.\n"), high, i >> 8);
         }
      }
   }

   Serial.println("Testing memory RW");
   { //Test the memory write and read
   
      Memory::SetReset(true);
      Memory::SetReadWrite(Memory::READ);
      Memory::SetAddressSelector(Memory::PC);
      Memory::SetOutputMemory(true);
      Memory::Sample();
      Memory::SetReset(false);

      //Write to the memory
      for(u16 i = 0; i < 100; ++i){

         Memory::SetBus((i >> 8) + (i & 0xFF));
         Memory::SetReadWrite(Memory::WRITE);
         Memory::Sample();
         Memory::SetReadWrite(Memory::READ);

         Memory::SetIncrementPC(true);
         Memory::Sample();
         Memory::SetIncrementPC(false);
      }

      Memory::SetReset(true);
      Memory::Sample();
      Memory::SetReset(false);
      
      //Read from the memory
      for(u16 i = 0; i < 100; ++i){

         Memory::SetReadWrite(Memory::READ);
         Memory::Sample();
         auto value = Memory::GetBus();
         
         Memory::SetIncrementPC(true);
         Memory::Sample();
         Memory::SetIncrementPC(false);

         if (value != (u8)((i >> 8) + (i & 0xFF))){
            printf(PSTR("Memory PC read and write failed at address %u. Read: %d, should be %d.\n"), value, (u8)((i >> 8) + (i & 0xFF)));
         }
      }
   }
*/
   Serial.println("Testing AR RW");
   { //Test the AR write and read
   
      Memory::SetReset(true);
      Memory::SetReadWrite(Memory::READ);
      Memory::SetAddressSelector(Memory::AR);
      Memory::SetOutputMemory(false);
      Memory::Sample();
      Memory::SetReset(false);

      //Write to the memory sample 1: ROM
      for(u16 i = 0; i < 100; ++i){

         Memory::SetBus(i & 0xFF);
         Memory::SetLatchARLow(true);
         Memory::Sample();
         Memory::SetLatchARLow(false);

         Memory::SetBus(i >> 8);
         Memory::SetLatchARHigh(true);
         Memory::Sample();
         Memory::SetLatchARHigh(false);

         Memory::SetBus((i >> 8) + (i & 0xFF));
         Memory::SetOutputMemory(true);
         Memory::SetReadWrite(Memory::WRITE);
         Memory::Sample();
         Memory::SetReadWrite(Memory::READ);
         Memory::Sample();
         Memory::SetOutputMemory(false);
      }

      //Write to the memory sample 2: RAM
      for(u16 i = 32768; i < 32868; ++i){

         Memory::SetBus(i & 0xFF);
         Memory::SetLatchARLow(true);
         Memory::Sample();
         Memory::SetLatchARLow(false);

         Memory::SetBus(i >> 8);
         Memory::SetLatchARHigh(true);
         Memory::Sample();
         Memory::SetLatchARHigh(false);

         Memory::SetBus((i >> 8) + (i & 0xFF));
         Memory::SetOutputMemory(true);
         Memory::SetReadWrite(Memory::WRITE);
         Memory::Sample();
         Memory::SetReadWrite(Memory::READ);
         Memory::Sample();
         Memory::SetOutputMemory(false);
      }

      Memory::SetReadWrite(Memory::READ);

      //Read from the memory sample 1: ROM
      for(u16 i = 0; i < 100; ++i){

         Memory::SetBus(i & 0xFF);
         Memory::SetLatchARLow(true);
         Memory::Sample();
         Memory::SetLatchARLow(false);

         Memory::SetBus(i >> 8);
         Memory::SetLatchARHigh(true);
         Memory::Sample();
         Memory::SetLatchARHigh(false);

         Memory::SetOutputMemory(true);
         Memory::Sample();
         Memory::SetOutputMemory(false);
         auto value = Memory::GetBus();

         if (value != (u8)((i >> 8) + (i & 0xFF))){
            printf(PSTR("Memory AR read and write failed at address %u. Read: %d, should be %d.\n"), value, (u8)((i >> 8) + (i & 0xFF)));
         }
      }

      //Read from the memory sample 2: RAM
      for(u16 i = 32768; i < 32868; ++i){

         Memory::SetBus(i & 0xFF);
         Memory::SetLatchARLow(true);
         Memory::Sample();
         Memory::SetLatchARLow(false);

         Memory::SetBus(i >> 8);
         Memory::SetLatchARHigh(true);
         Memory::Sample();
         Memory::SetLatchARHigh(false);

         Memory::SetOutputMemory(true);
         Memory::Sample();
         Memory::SetOutputMemory(false);
         auto value = Memory::GetBus();

         if (value != (u8)((i >> 8) + (i & 0xFF))){
            printf(PSTR("Memory AR read and write failed at address %u. Read: %d, should be %d.\n"), value, (u8)((i >> 8) + (i & 0xFF)));
         }
      }
   }

   Serial.println("Testing link gen");
   { //Test the generated link signals.

      Memory::SetReadWrite(Memory::READ);
      Memory::SetOutputMemory(false);
      Memory::SetLatchARLow(false);
      Memory::SetLatchARHigh(false);
      Memory::SetLatchPCLow(false);  
      Memory::SetLatchPCHigh(false);
      Memory::SetOutputPCLow(false);
      Memory::SetOutputPCHigh(false);
      Memory::SetAddressSelector(Memory::PC);
      Memory::SetLatchPrefetchRegister(false);

      Memory::Sample();

      auto AssertLink = [](bool dir, bool oe){

         if (Memory::GetMemoryLinkDir() != dir || Memory::GetMemoryLinkOE() != oe){
            printf(PSTR("Link error. To memory: %d, should be: %d, link OE: %d, should be: %d.\n"), Memory::GetMemoryLinkDir() == Memory::TO_MEMORY, dir == Memory::TO_MEMORY, Memory::GetMemoryLinkOE(), oe);
         }
      };

      AssertLink(Memory::TO_MEMORY, true);
      
      Memory::SetOutputPCLow(true);
      Memory::Sample();
      Memory::SetOutputPCLow(false);
      AssertLink(Memory::TO_CE, true);

      Memory::SetOutputPCHigh(true);
      Memory::Sample();
      Memory::SetOutputPCHigh(false);
      AssertLink(Memory::TO_CE, true);

      Memory::SetOutputMemory(true);
      Memory::SetReadWrite(Memory::READ);
      Memory::Sample();
      AssertLink(Memory::TO_CE, true);
      Memory::SetReadWrite(Memory::WRITE);
      Memory::Sample();
      AssertLink(Memory::TO_MEMORY, true);
      Memory::SetOutputMemory(false);

      Memory::SetLatchPrefetchRegister(true);
      {
         Memory::SetOutputPCLow(true);
         Memory::Sample();
         Memory::SetOutputPCLow(false);
         AssertLink(Memory::TO_MEMORY, false);

         Memory::SetOutputPCHigh(true);
         Memory::Sample();
         Memory::SetOutputPCHigh(false);
         AssertLink(Memory::TO_MEMORY, false);

         Memory::SetOutputMemory(true);
         Memory::SetReadWrite(Memory::READ);
         Memory::Sample();
         AssertLink(Memory::TO_MEMORY, false);
         Memory::SetReadWrite(Memory::WRITE);
         AssertLink(Memory::TO_MEMORY, false);
         Memory::SetOutputMemory(false);
         Memory::SetReadWrite(Memory::READ);
      }

      Memory::SetLatchPrefetchRegister(false);
   }

   Serial.println("Testing prefetch");
   {
      Memory::SetLatchPrefetchRegister(true);
      Memory::SetReadWrite(Memory::READ);
      Memory::SetOutputMemory(false);

      for(int i = 0; i < 256; i ++){

         Memory::SetBus(i);
         Memory::Sample();

         auto value = Memory::GetPrefetchRegister();
         if (value != i){
            printf(PSTR("Prefetch register failed. Register: %d, should be: %d.\n"), value, i);
         }
      }
   }
}

void TestCE(){

   CE::SetALUResult(0);
   CE::SetOESelector(CE::OESelector::NONE);
   CE::SetLESelector(CE::LESelector::NONE);
   CE::SetRBusSelector(CE::IMM);
   CE::SetFlagImmediate(0);
   CE::SetImmediate(0);
   CE::SetLinkOE(true);
   CE::SetLinkDir(CE::TO_MEMORY);
   CE::SetTempLE(false);
   CE::SetALUBus(0);

   CE::Sample();

   Serial.println("Testing registers");
   {

      for(int i = 0; i < 256; i ++){

         CE::SetALUResult(i);
         CE::SetLESelector(CE::LESelector::TEMP1);
         CE::Sample();

         for(int j = 0; j < 8; j ++){

            CE::SetLESelector((CE::LESelector)j);
            CE::SetOESelector(CE::OESelector::TEMP1);
            CE::Sample();
            CE::SetLESelector(CE::LESelector::NONE);
            CE::SetOESelector((CE::OESelector)j);
            CE::Sample();
            CE::SetOESelector(CE::OESelector::NONE);

            if (i != CE::GetLBus()){
               printf(PSTR("Register %d LE or OE failed. Value: %d, should be: %d.\n"), j, CE::GetLBus(), i);
            }
         }
      }
   }

   Serial.println("Testing immediates");
   {
      for(int i = -2; i < 2; i ++){

         CE::SetImmediate(i);
         CE::SetRBusSelector(CE::IMM);
         CE::Sample();

         if (CE::GetRBus() != (u8)i){
            printf(PSTR("Immediate failed. Value: %d, should be: %d.\n"), CE::GetRBus(), i);
         }
      }

      for(int i = 0; i < 8; i ++){

         CE::SetFlagImmediate(i);
         CE::SetRBusSelector(CE::IMM_FLAG);
         CE::Sample();

         if (CE::GetRBus() != (u8)~(1 << i)){
            printf(PSTR("Flag immediate failed. Value: %d, should be: %d.\n"), CE::GetRBus(), (u8)~(1 << i));
         }
      }
   }

   Serial.println("Testing generated flags");
   {
      CE::SetLESelector(CE::LESelector::NONE);
      CE::SetOESelector(CE::OESelector::NONE);

      auto AssertGeneratedFlags = [](bool pcLowOE, bool pcHighOE, bool pcLowLE, bool pcHighLE, bool flagsOE, CE::InputSelector flagsInputSel){

         if (CE::GetPCLowOE() != pcLowOE || CE::GetPCHighOE() != pcHighOE || CE::GetPCLowLE() != pcLowLE || CE::GetPCHighLE() != pcHighLE || CE::GetALUFlagsOE() != flagsOE || CE::GetALUFlagInputSelector() != flagsInputSel){
            printf(PSTR("One or more flags failed. PC Low OE: %d, should be: %d, PC High OE: %d, should be: %d, PC Low LE: %d, should be: %d, PC High LE: %d, should be: %d, flags OE: %d, should be: %d, flags input from bus: %d, should be: %d.\n"), CE::GetPCLowOE(), pcLowOE, CE::GetPCHighOE(), pcHighOE, CE::GetPCLowLE(), pcLowLE, CE::GetPCHighLE(), pcHighLE, CE::GetALUFlagsOE(), flagsOE, CE::GetALUFlagInputSelector() == CE::BUS, flagsInputSel == CE::BUS);
         }
      };

      CE::Sample();
      AssertGeneratedFlags(false, false, false, false, false, CE::GENERATED);

      CE::SetLESelector(CE::LESelector::PCL);
      CE::Sample();
      AssertGeneratedFlags(false, false, true, false, false, CE::GENERATED);
      CE::SetLESelector(CE::LESelector::NONE);

      CE::SetLESelector(CE::LESelector::PCH);
      CE::Sample();
      AssertGeneratedFlags(false, false, false, true, false, CE::GENERATED);
      CE::SetLESelector(CE::LESelector::NONE);

      CE::SetOESelector(CE::OESelector::PCL);
      CE::Sample();
      AssertGeneratedFlags(true, false, false, false, false, CE::GENERATED);
      CE::SetOESelector(CE::OESelector::NONE);

      CE::SetOESelector(CE::OESelector::PCH);
      CE::Sample();
      AssertGeneratedFlags(false, true, false, false, false, CE::GENERATED);
      CE::SetOESelector(CE::OESelector::NONE);

      CE::SetOESelector(CE::OESelector::ALU);
      CE::Sample();
      AssertGeneratedFlags(false, false, false, false, true, CE::GENERATED);
      CE::SetOESelector(CE::OESelector::NONE);

      //Don't test the ALU input selector because that line isn't hooked up. If we do decide to try it
      // make sure that CE.cpp is updated because we now always read CE::GENERATED.
      /*
      CE::SetLESelector(CE::ALU);
      CE::Sample();
      AssertGeneratedFlags(false, false, false, false, false, CE::BUS);
      */
   }

   Serial.println("Testing temp 2 register");
   {
      CE::SetLESelector(CE::LESelector::TEMP1);
      CE::SetOESelector(CE::OESelector::TEMP1);
      CE::SetRBusSelector(CE::TEMP2);
      CE::SetTempLE(true);

      for(int i = 0; i < 256; i ++){

         CE::SetALUResult(i);
         CE::Sample();
         
         if (CE::GetRBus() != i){
            printf(PSTR("Temp 2 register LE or OE failed. Value: %d, should be: %d.\n"), CE::GetRBus(), i);
         }
      }

      CE::SetTempLE(false);
   }
}