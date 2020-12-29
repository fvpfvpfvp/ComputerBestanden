#pragma once

#include "Interface.h"

namespace CE {

   enum class OESelector {

      R0, R1, R2, R3, R4, R5, R6, R7,
      PCL, PCH, SPL, SPH, INOUT, TEMP1, ALU, NONE
   };

   enum class LESelector {

      R0, R1, R2, R3, R4, R5, R6, R7,
      PCL, PCH, SPL, SPH, ALU, TEMP1, INOUT, NONE
   };

   enum RBusSelector {

      TEMP2, IMM_FLAG, IMM, MEM
   };

   enum LinkDir {

      TO_MEMORY, TO_CE
   };

   enum InputSelector {

      GENERATED, BUS
   };

   void SetALUResult(u8 value);
   void SetOESelector(OESelector selector);
   void SetLESelector(LESelector selector);
   void SetRBusSelector(RBusSelector selector);
   void SetFlagImmediate(u8 flagIndex);
   void SetImmediate(u8 value);
   void SetLinkOE(bool output);
   void SetLinkDir(LinkDir dir);
   void SetTempLE(bool latch);
   void SetALUBus(u8 value);

   u8 GetLBus();
   u8 GetRBus();
   bool GetPCLowOE();
   bool GetPCHighOE();
   bool GetPCLowLE();
   bool GetPCHighLE();
   u8 GetALUBus();
   bool GetALUFlagsOE();
   InputSelector GetALUFlagInputSelector();

   void Sample();
}