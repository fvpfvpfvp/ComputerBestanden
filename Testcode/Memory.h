#pragma once

#include "Interface.h"

namespace Memory {

   enum LinkDir {

      TO_MEMORY, TO_CE
   };
   
   enum ReadWrite {

      READ, WRITE
   };

   enum AddressSelector {

      AR, PC
   };

   void SetBus(u8 value);
   void SetIncrementPC(bool increment);
   void SetOutputPCLow(bool output);
   void SetOutputPCHigh(bool output);
   void SetLatchPCLow(bool latch);
   void SetLatchPCHigh(bool latch);
   void SetLatchARLow(bool latch);
   void SetLatchARHigh(bool latch);
   void SetReadWrite(ReadWrite rw);
   void SetOutputMemory(bool output);
   void SetAddressSelector(AddressSelector selector);
   void SetReset(bool reset);
   void SetLatchPrefetchRegister(bool latch);

   u8 GetPrefetchRegister();
   u8 GetBus();
   bool GetMemoryLinkOE();
   LinkDir GetMemoryLinkDir();

   void Sample();
}