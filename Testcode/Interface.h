#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
//typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

enum Messages {

   M_SET_DIR_ALL,
   M_SET_DIR_SINGLE,
   M_SET_VALUE_ALL,
   M_SET_VALUE_SINGLE,
   M_GET_VALUE_ALL,
   M_GET_VALUE_SINGLE,
};

void SetDir(u8 pin, bool dir);
void SetValue(u8 pin, bool value);
bool GetValue(u8 pin);

void Flush();