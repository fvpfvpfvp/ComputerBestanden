#pragma once

#include "Interface.h"

namespace ALU {

   enum Operator {

      OR,
      AND,
      PASS,
      SHIFT_LEFT,
      SHIFT_RIGHT,
      SUBTRACT,
      ADD,
      INVERT,
   };

   enum LeftRightSelector {
      LEFT, RIGHT
   };
   enum RegisterSelector {
      ONE, TWO
   };
   enum InputSelector {
      GENERATED, BUS
   };

   //Set values
   void SetLeftValue(u8 value);
   void SetRightValue(u8 value);
   void SetOperator(Operator op);
   void SetLeftRightSelector(LeftRightSelector selector);
   void SetBus(u8 value);
   void SetSpecifiedCarry(bool carry);
   void SetUseSpecifiedCarry(bool use);
   void SetLatchOne(bool latch);
   void SetLatchTwo(bool latch);
   void SetRegisterSelector(RegisterSelector selector);
   void SetInputSelector(InputSelector selector);
   void SetOutputToBus(bool output);

   u8 GetResult();
   u8 GetBus();
   bool GetCarry();
   bool GetZero();
   bool GetSign();
   bool GetInterrupt();

   void Sample();
}