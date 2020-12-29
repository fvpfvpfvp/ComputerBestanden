#include <fstream>
#include <iostream>

#include <vector>

using namespace std;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

enum Instruction {

	START = 0,
	HALT = 1,
	OUTPUT = 2,
	INPUT = 3,
	LOAD_LITERAL = 4,
	ADD_LITERAL = 5,
	SUB_LITERAL = 6,
	INTERRUPT = 7,

	LOAD_R0 = 8,
	LOAD_R1,
	LOAD_R2,
	LOAD_R3,
	LOAD_R4,
	LOAD_R5,
	LOAD_R6,
	LOAD_R7,

	SAVE_R0 = 16,
	SAVE_R1,
	SAVE_R2,
	SAVE_R3,
	SAVE_R4,
	SAVE_R5,
	SAVE_R6,
	SAVE_R7,

	ADD_R0 = 24,
	ADD_R1,
	ADD_R2,
	ADD_R3,
	ADD_R4,
	ADD_R5,
	ADD_R6,
	ADD_R7,

	SUB_R0 = 32,
	SUB_R1,
	SUB_R2,
	SUB_R3,
	SUB_R4,
	SUB_R5,
	SUB_R6,
	SUB_R7,

	JUMP = 40,
	SHL = 41,
	SHR = 42,
	INV = 43,
	ROL = 44,
	ROR = 45,
	ADC_LITERAL = 46,
	SBB_LITERAL = 47,

	OR_R0 = 48,
	OR_R1,
	OR_R2,
	OR_R3,
	OR_R4,
	OR_R5,
	OR_R6,
	OR_R7,

	AND_R0 = 48,
	AND_R1,
	AND_R2,
	AND_R3,
	AND_R4,
	AND_R5,
	AND_R6,
	AND_R7,

	DEC_R0 = 56,
	DEC_R1,
	DEC_R2,
	DEC_R3,
	DEC_R4,
	DEC_R5,
	DEC_R6,
	DEC_R7,

	INC_R0 = 64,
	INC_R1,
	INC_R2,
	INC_R3,
	INC_R4,
	INC_R5,
	INC_R6,
	INC_R7,

	JUMP_IF_CARRY = 72,
	JUMP_IF_NOT_CARRY = 73,
	JUMP_IF_ZERO = 74,
	JUMP_IF_NOT_ZERO = 75,
	JUMP_IF_SIGN = 76,
	JUMP_IF_NOT_SIGN = 77,
	AND_LITERAL = 78,
	OR_LITERAL = 79,

	ADC_R0 = 80,
	ADC_R1,
	ADC_R2,
	ADC_R3,
	ADC_R4,
	ADC_R5,
	ADC_R6,
	ADC_R7,

	SBB_R0 = 88,
	SBB_R1,
	SBB_R2,
	SBB_R3,
	SBB_R4,
	SBB_R5,
	SBB_R6,
	SBB_R7,

	EVALUATE = 96,
	LOAD_MEM = 97,
	SAVE_MEM = 98,
	MUL = 99,
	DIV = 100,
	MEMCPY = 101,
	MEMSET = 102,

	ZERO_R0 = 104,
	ZERO_R1,
	ZERO_R2,
	ZERO_R3,
	ZERO_R4,
	ZERO_R5,
	ZERO_R6,
	ZERO_R7,

	LOAD_MEM_RA = 112,
	LOAD_MEM_RB,
	LOAD_MEM_RC,
	LOAD_MEM_RD,
	SAVE_MEM_RA,
	SAVE_MEM_RB,
	SAVE_MEM_RC,
	SAVE_MEM_RD,

	LOAD_MEM_TO_RA = 120,
	LOAD_MEM_TO_RB,
	LOAD_MEM_TO_RC,
	LOAD_MEM_TO_RD,
	LOAD_MEM_TO_RA_USING_RB,
	LOAD_MEM_TO_RA_USING_RC,
	LOAD_MEM_TO_RA_USING_RD,


	INC_RA = 128,
	INC_RB,
	INC_RC,
	INC_RD,
	DEC_RA,
	DEC_RB,
	DEC_RC,
	DEC_RD,

	COPY_RB_TO_RA = 136,
	COPY_RC_TO_RA,
	COPY_RD_TO_RA,
	COPY_RA_TO_RB,
	COPY_RA_TO_RC,
	COPY_RA_TO_RD,


	ADD_RB_TO_RA = 144,
	ADD_RC_TO_RA,
	ADD_RD_TO_RA,
	SUB_RB_FROM_RA,
	SUB_RC_FROM_RA,
	SUB_RD_FROM_RA,


	PUSH_R0 = 152,
	PUSH_R1,
	PUSH_R2,
	PUSH_R3,
	PUSH_R4,
	PUSH_R5,
	PUSH_R6,
	PUSH_R7,

	POP_R0 = 160,
	POP_R1,
	POP_R2,
	POP_R3,
	POP_R4,
	POP_R5,
	POP_R6,
	POP_R7,


	LOAD_RA_LITERAL = 168,
	LOAD_RB_LITERAL,
	LOAD_RC_LITERAL,
	LOAD_RD_LITERAL,


	LOAD_R0_LITERAL = 176,
	LOAD_R1_LITERAL,
	LOAD_R2_LITERAL,
	LOAD_R3_LITERAL,
	LOAD_R4_LITERAL,
	LOAD_R5_LITERAL,
	LOAD_R6_LITERAL,
	LOAD_R7_LITERAL,


	//CALL,
	//RETURN,


	SET_CARRY = 0b11110100,
	CLEAR_CARRY = 0b11111100,
	SET_INTERRUPT = 0b11110111,
	CLEAR_INTERRUPT = 0b11111111,
};

int main(){

	std::vector<char> code(0x8000, HALT);
	int index = 0;
/*
	code[index++] = LOAD_R0_LITERAL;
	code[index++] = 60;
	code[index++] = LOAD_R1_LITERAL;
	code[index++] = 10;
	code[index++] = LOAD_R2_LITERAL;
	code[index++] = 250;
	code[index++] = LOAD_R3_LITERAL;
	code[index++] = 20;
	code[index++] = ADD_RB_TO_RA;
	code[index++] = LOAD_R0;
	code[index++] = OUTPUT;
	code[index++] = LOAD_R1;
	code[index++] = OUTPUT;
/*
	code[index++] = LOAD_LITERAL;
	code[index++] = 10;
	code[index++] = ADD_LITERAL;
	code[index++] = 10;
	code[index++] = OUTPUT;
	code[index++] = CLEAR_CARRY;
	code[index++] = LOAD_LITERAL;
	code[index++] = 10;
	code[index++] = ADC_LITERAL;
	code[index++] = 10;
	code[index++] = OUTPUT;
	code[index++] = SET_CARRY;
	code[index++] = LOAD_LITERAL;
	code[index++] = 10;
	code[index++] = ADC_LITERAL;
	code[index++] = 10;
	code[index++] = OUTPUT;
	code[index++] = SET_INTERRUPT;
	code[index++] = INPUT;
	code[index++] = CLEAR_INTERRUPT;

*/



/*
	code[index++] = LOAD_R4_LITERAL;
	code[index++] = 42;
	code[index++] = LOAD_R4;
	code[index++] = OUTPUT;
	code[index++] = PUSH_R4;
	code[index++] = LOAD_R4_LITERAL;
	code[index++] = 69;
	code[index++] = LOAD_R4;
	code[index++] = OUTPUT;
	code[index++] = POP_R4;
	code[index++] = LOAD_R4;
	code[index++] = OUTPUT;

*/
/*
	code[index++] = LOAD_R2_LITERAL;
	code[index++] = 250;
	code[index++] = LOAD_R3_LITERAL;
	code[index++] = 128;
	code[index++] = LOAD_R4_LITERAL;
	code[index++] = 20;
	code[index++] = ZERO_R5;
	code[index++] = LOAD_R0_LITERAL;
	code[index++] = 42;
	code[index++] = MEMSET;
	code[index++] = INPUT;

	code[index++] = LOAD_R2_LITERAL;
	code[index++] = 250;
	code[index++] = LOAD_R3_LITERAL;
	code[index++] = 128;
	code[index++] = LOAD_R4_LITERAL;
	code[index++] = 20;

	auto loop = index;
	code[index++] = LOAD_MEM_RB;
	code[index++] = OUTPUT;
	code[index++] = SUB_LITERAL;
	code[index++] = 42;
	code[index++] = JUMP_IF_ZERO;
	auto continueFillIndex = index;
	code[index++] = 0;
	code[index++] = 0;
	code[index++] = INPUT;
	code[index++] = LOAD_R4;
	code[index++] = OUTPUT;
	code[index++] = INPUT;
	code[continueFillIndex] = index;
	code[index++] = INC_RB;
	code[index++] = DEC_R4;
	code[index++] = JUMP_IF_NOT_ZERO;
	code[index++] = loop;
	code[index++] = 0;
*/

	code[index++] = INPUT;
	code[index++] = SAVE_R0;
	code[index++] = INPUT;
	code[index++] = SAVE_R1;
	code[index++] = LOAD_R4_LITERAL;
	code[index++] = 8;
	code[index++] = ZERO_R6;
	code[index++] = DIV;
	code[index++] = LOAD_R0;
	code[index++] = OUTPUT;
	code[index++] = INPUT;
	code[index++] = LOAD_R6;
	code[index++] = OUTPUT;

/*
	code[index++] = LOAD_LITERAL;
	code[index++] = 1;
	code[index++] = SAVE_MEM;
	code[index++] = 0;
	code[index++] = 255;
	code[index++] = SAVE_MEM;
	code[index++] = 1;
	code[index++] = 255;

	auto loop = index;
	code[index++] = LOAD_MEM;
	code[index++] = 1;
	code[index++] = 255;
	code[index++] = SAVE_R0;
	code[index++] = LOAD_MEM;
	code[index++] = 0;
	code[index++] = 255;
	code[index++] = ADD_R0;
	code[index++] = OUTPUT;
	code[index++] = SAVE_MEM;
	code[index++] = 2;
	code[index++] = 255;
	code[index++] = LOAD_R0;
	code[index++] = SAVE_MEM;
	code[index++] = 0;
	code[index++] = 255;
	code[index++] = LOAD_MEM;
	code[index++] = 2;
	code[index++] = 255;
	code[index++] = SAVE_MEM;
	code[index++] = 1;
	code[index++] = 255;
	code[index++] = JUMP;
	code[index++] = loop;
	code[index++] = 0;
*/


	//Initialize
	/*code[index++] = LOAD_LITERAL;
	code[index++] = 225;
	code[index++] = SAVE_R0;
	code[index++] = LOAD_LITERAL;
	code[index++] = 13;
	code[index++] = SAVE_R1;

	//Numerator in R0, denumerator in R1, remainder in R3, iterator in R2
	code[index++] = ZERO_R3;
	code[index++] = LOAD_LITERAL;
	code[index++] = 8;
	code[index++] = SAVE_R2;
	auto loop = index;
	code[index++] = LOAD_R0;
	code[index++] = SHL;
	code[index++] = SAVE_R0;
	code[index++] = LOAD_R3;
	code[index++] = ROL;
	code[index++] = SAVE_R3;
	code[index++] = LOAD_R1;
	code[index++] = SUB_R3;
	code[index++] = JUMP_IF_NOT_CARRY;
	auto skipFillIndex = index;
	code[index++] = 0;
	code[index++] = 0;
	code[index++] = SAVE_R3;
	code[index++] = INC_R0;
	code[skipFillIndex] = index;
	code[index++] = DEC_R2;
	code[index++] = JUMP_IF_NOT_ZERO;
	code[index++] = loop;
	code[index++] = 0;
	code[index++] = LOAD_R0;
	code[index++] = OUTPUT;
	code[index++] = LOAD_R3;
	code[index++] = OUTPUT;
	code[index++] = HALT;
*/
	/*auto copy = index;
	for(int i = 0; i < 30000; i ++){

		code[index++] = EVALUATE;
	}

	code[index++] = JUMP;
	code[index++] = copy;
	code[index++] = 0;*/

	fstream file("code.bin", ios::out | ios::trunc | ios::binary);
	file.write(code.data(), code.size());
	file.close();
}
