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

enum ALUOperation {

	OR,
	AND,
	PASS,
	SHIFT_LEFT,
	SHIFT_RIGHT,
	SUB,
	ADD,
	INVERT,
};

enum ALUSelector {

	LEFT, RIGHT
};

enum Register {

	R0, R1, R2, R3, R4, R5, R6, R7, PCL, PCH, SPL, SPH, FLAGS, ALU, INOUT, NONE
};

enum CarrySel {

	GENERATED, SPECIFIED
};

enum FlagSel {

	FLAG1, FLAG2
};

enum BusSel {

	TEMP, FLAG_IMMEDIATE, IMMEDIATE, MEMORY
};

enum MemSel {

	PC, ADDRESS
};

#pragma pack(push, 1)

struct Value {

	u8 newInstructionLE : 1;
	u8 instructionCounterReset : 1;
	u8 pcCountEnable : 1;
	u8 ceTempLE : 1;
	u8 aluOperation : 3;
	u8 aluSel : 1;
	u8 oeSel : 4;
	u8 leSel : 4;
	u8 carrySpec : 1;
	u8 carrySel : 1;
	u8 flag1LE : 1;
	u8 flag2LE : 1;
	u8 flagSel : 1;
	u8 busSel : 2;
	u8 prefetchLE : 1;
	u8 arLLE : 1;
	u8 arHLE : 1;
	u8 memWrite : 1;
	u8 memOE : 1;
	u8 memSel : 1;
	u8 immediate : 2;
	u8 pause : 1;

	Value(){

		newInstructionLE = true;
		instructionCounterReset = false;
		pcCountEnable = false;
		ceTempLE = false;
		aluOperation = PASS;
		aluSel = LEFT;
		oeSel = NONE;
		leSel = NONE;
		carrySpec = false;
		carrySel = SPECIFIED;
		flag1LE = true;
		flag2LE = true;
		flagSel = FLAG1;
		busSel = IMMEDIATE;
		prefetchLE = false;
		arLLE = false;
		arHLE = false;
		memWrite = false;
		memOE = false;
		memSel = PC;
		immediate = 0;
		pause = true;
	}
};

#pragma pack(pop)

std::vector<Value> code;

void WriteValue(u8 instruction, u8 step, Value value, int carry = -1, int zero = -1, int sign = -1){

	for(int i = 0; i < 8; i++){

		if ((carry == -1 || ((i & 1) == (carry << 0))) && 
			(zero == -1 || ((i & 2) == (zero << 1))) && 
			(sign == -1 || ((i & 4) == (sign << 2)))){
				code[(i << 12) + (step << 8) + instruction] = value;
		}
	}
}

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

//Nog te doen: adressing modes, branches, call

#define WRITE_CONDITIONAL_VALUE(instruction, step, initializers, _carry, _zero, _sign) WriteValue(instruction, step, []()->Value{ Value v; initializers; return v; }(), _carry, _zero, _sign);
#define WRITE_VALUE(instruction, step, initializers) WriteValue(instruction, step, []()->Value{ Value v; initializers; return v; }());
#define WRITE_SINGLE_VALUE(instruction, initializers) WriteValue(instruction, 0, []()->Value{ Value v; initializers; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; return v; }());

int main(){

	code.resize(0x8000);
	for(auto& entry : code){

		//Make sure that by default every value at every step will make sure a new instruction gets loaded.
		entry.newInstructionLE = false;
		entry.instructionCounterReset = true;
		entry.pcCountEnable = true;
		entry.prefetchLE = true;
		entry.memOE = true;
		entry.memSel = PC;
	}

	//Clear ALU flags
	WRITE_VALUE(START, 0, v.leSel = ALU; v.busSel = IMMEDIATE; v.immediate = 3; v.aluOperation = PASS; v.aluSel = RIGHT);
	WRITE_VALUE(START, 1, v.leSel = SPL; v.oeSel = ALU);
	WRITE_VALUE(START, 2, v.leSel = SPH; v.oeSel = ALU);
	WRITE_VALUE(START, 3, v.busSel = IMMEDIATE; v.immediate = 0; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = RIGHT);
	WRITE_VALUE(START, 4, v.oeSel = ALU; v.leSel = INOUT);
	WRITE_VALUE(START, 5, v.oeSel = ALU; v.leSel = FLAGS; v.flag1LE = false; v.flag2LE = false; v.prefetchLE = true; v.memOE = true; v.memSel = PC; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_SINGLE_VALUE(OUTPUT, v.leSel = INOUT; v.oeSel = ALU);

	WRITE_SINGLE_VALUE(INPUT, v.oeSel = INOUT; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT; v.pause = false);

	WRITE_VALUE(LOAD_LITERAL, 0, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT);
	WRITE_VALUE(LOAD_LITERAL, 1, v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(ADD_LITERAL, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADD_LITERAL, 1, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = TEMP; v.flag2LE = false);
	WRITE_VALUE(ADD_LITERAL, 2, v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(SUB_LITERAL, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SUB_LITERAL, 1, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.leSel = ALU; v.aluOperation = SUB; v.aluSel = RIGHT; v.busSel = TEMP; v.carrySpec = true; v.flag2LE = false);
	WRITE_VALUE(SUB_LITERAL, 2, v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(HALT, 0, v.instructionCounterReset = true);

	WRITE_SINGLE_VALUE(LOAD_R0, v.oeSel = R0; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT);
	WRITE_SINGLE_VALUE(LOAD_R1, v.oeSel = R1; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT);
	WRITE_SINGLE_VALUE(LOAD_R2, v.oeSel = R2; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT);
	WRITE_SINGLE_VALUE(LOAD_R3, v.oeSel = R3; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT);
	WRITE_SINGLE_VALUE(LOAD_R4, v.oeSel = R4; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT);
	WRITE_SINGLE_VALUE(LOAD_R5, v.oeSel = R5; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT);
	WRITE_SINGLE_VALUE(LOAD_R6, v.oeSel = R6; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT);
	WRITE_SINGLE_VALUE(LOAD_R7, v.oeSel = R7; v.leSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT);

	WRITE_SINGLE_VALUE(SAVE_R0, v.oeSel = ALU; v.leSel = R0);
	WRITE_SINGLE_VALUE(SAVE_R1, v.oeSel = ALU; v.leSel = R1);
	WRITE_SINGLE_VALUE(SAVE_R2, v.oeSel = ALU; v.leSel = R2);
	WRITE_SINGLE_VALUE(SAVE_R3, v.oeSel = ALU; v.leSel = R3);
	WRITE_SINGLE_VALUE(SAVE_R4, v.oeSel = ALU; v.leSel = R4);
	WRITE_SINGLE_VALUE(SAVE_R5, v.oeSel = ALU; v.leSel = R5);
	WRITE_SINGLE_VALUE(SAVE_R6, v.oeSel = ALU; v.leSel = R6);
	WRITE_SINGLE_VALUE(SAVE_R7, v.oeSel = ALU; v.leSel = R7);

	WRITE_VALUE(ADD_R0, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADD_R0, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADD_R1, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADD_R1, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R1; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADD_R2, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADD_R2, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R2; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADD_R3, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADD_R3, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R3; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADD_R4, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADD_R4, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R4; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADD_R5, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADD_R5, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R5; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADD_R6, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADD_R6, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R6; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADD_R7, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADD_R7, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R7; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(SUB_R0, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SUB_R0, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.carrySpec = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SUB_R1, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SUB_R1, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R1; v.leSel = ALU; v.carrySpec = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SUB_R2, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SUB_R2, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R2; v.leSel = ALU; v.carrySpec = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SUB_R3, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SUB_R3, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R3; v.leSel = ALU; v.carrySpec = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SUB_R4, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SUB_R4, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R4; v.leSel = ALU; v.carrySpec = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SUB_R5, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SUB_R5, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R5; v.leSel = ALU; v.carrySpec = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SUB_R6, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SUB_R6, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R6; v.leSel = ALU; v.carrySpec = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SUB_R7, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SUB_R7, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R7; v.leSel = ALU; v.carrySpec = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(JUMP, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.aluOperation = PASS; v.aluSel = LEFT; v.leSel = ALU);
	WRITE_VALUE(JUMP, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.ceTempLE = true);
	WRITE_VALUE(JUMP, 2, v.oeSel = ALU; v.leSel = PCL);
	WRITE_VALUE(JUMP, 3, v.leSel = ALU; v.busSel = TEMP; v.aluOperation = PASS; v.aluSel = RIGHT);
	WRITE_VALUE(JUMP, 4, v.oeSel = ALU; v.leSel = PCH);
	WRITE_VALUE(JUMP, 5, v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(SHL, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SHL, 1, v.aluOperation = SHIFT_LEFT; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SHR, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SHR, 1, v.aluOperation = SHIFT_RIGHT; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(INV, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(INV, 1, v.aluOperation = INVERT; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.leSel = ALU; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	
	WRITE_VALUE(ROL, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ROL, 1, v.aluOperation = SHIFT_LEFT; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ROR, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ROR, 1, v.aluOperation = SHIFT_RIGHT; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	
	WRITE_VALUE(ADC_LITERAL, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADC_LITERAL, 1, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = TEMP; v.flag2LE = false; v.carrySel = GENERATED; v.flagSel = FLAG2);
	WRITE_VALUE(ADC_LITERAL, 2, v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(SBB_LITERAL, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SBB_LITERAL, 1, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.leSel = ALU; v.aluOperation = SUB; v.aluSel = RIGHT; v.busSel = TEMP; v.flag2LE = false; v.carrySel = GENERATED; v.flagSel = FLAG2);
	WRITE_VALUE(SBB_LITERAL, 2, v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(INC_R0, 0, v.oeSel = R0; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 1; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(INC_R0, 1, v.oeSel = ALU; v.leSel = R0; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(INC_R1, 0, v.oeSel = R1; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 1; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(INC_R1, 1, v.oeSel = ALU; v.leSel = R1; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(INC_R2, 0, v.oeSel = R2; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 1; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(INC_R2, 1, v.oeSel = ALU; v.leSel = R2; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(INC_R3, 0, v.oeSel = R3; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 1; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(INC_R3, 1, v.oeSel = ALU; v.leSel = R3; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(INC_R4, 0, v.oeSel = R4; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 1; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(INC_R4, 1, v.oeSel = ALU; v.leSel = R4; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(INC_R5, 0, v.oeSel = R5; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 1; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(INC_R5, 1, v.oeSel = ALU; v.leSel = R5; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(INC_R6, 0, v.oeSel = R6; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 1; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(INC_R6, 1, v.oeSel = ALU; v.leSel = R6; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(INC_R7, 0, v.oeSel = R7; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 1; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(INC_R7, 1, v.oeSel = ALU; v.leSel = R7; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(DEC_R0, 0, v.oeSel = R0; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 3; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(DEC_R0, 1, v.oeSel = ALU; v.leSel = R0; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(DEC_R1, 0, v.oeSel = R1; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 3; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(DEC_R1, 1, v.oeSel = ALU; v.leSel = R1; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(DEC_R2, 0, v.oeSel = R2; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 3; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(DEC_R2, 1, v.oeSel = ALU; v.leSel = R2; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(DEC_R3, 0, v.oeSel = R3; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 3; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(DEC_R3, 1, v.oeSel = ALU; v.leSel = R3; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(DEC_R4, 0, v.oeSel = R4; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 3; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(DEC_R4, 1, v.oeSel = ALU; v.leSel = R4; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(DEC_R5, 0, v.oeSel = R5; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 3; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(DEC_R5, 1, v.oeSel = ALU; v.leSel = R5; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(DEC_R6, 0, v.oeSel = R6; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 3; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(DEC_R6, 1, v.oeSel = ALU; v.leSel = R6; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(DEC_R7, 0, v.oeSel = R7; v.leSel = ALU; v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = IMMEDIATE; v.immediate = 3; v.prefetchLE = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(DEC_R7, 1, v.oeSel = ALU; v.leSel = R7; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(AND_R0, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(AND_R0, 1, v.aluOperation = AND; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(AND_R1, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(AND_R1, 1, v.aluOperation = AND; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(AND_R2, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(AND_R2, 1, v.aluOperation = AND; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(AND_R3, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(AND_R3, 1, v.aluOperation = AND; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(AND_R4, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(AND_R4, 1, v.aluOperation = AND; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(AND_R5, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(AND_R5, 1, v.aluOperation = AND; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(AND_R6, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(AND_R6, 1, v.aluOperation = AND; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(AND_R7, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(AND_R7, 1, v.aluOperation = AND; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(OR_R0, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(OR_R0, 1, v.aluOperation = OR; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(OR_R1, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(OR_R1, 1, v.aluOperation = OR; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(OR_R2, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(OR_R2, 1, v.aluOperation = OR; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(OR_R3, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(OR_R3, 1, v.aluOperation = OR; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(OR_R4, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(OR_R4, 1, v.aluOperation = OR; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(OR_R5, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(OR_R5, 1, v.aluOperation = OR; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(OR_R6, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(OR_R6, 1, v.aluOperation = OR; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(OR_R7, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(OR_R7, 1, v.aluOperation = OR; v.aluSel = RIGHT; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_CONDITIONAL_VALUE(JUMP_IF_CARRY, 0, v.flagSel = FLAG2; v.pcCountEnable = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_CARRY, 0, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.aluOperation = PASS; v.aluSel = LEFT; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_CARRY, 1, v.flagSel = FLAG2; v.pcCountEnable = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_CARRY, 1, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.ceTempLE = true, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_CARRY, 2, v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_CARRY, 2, v.flagSel = FLAG2; v.oeSel = ALU; v.leSel = PCL, 1, -1, -1);
	WRITE_VALUE(JUMP_IF_CARRY, 3, v.leSel = ALU; v.busSel = TEMP; v.aluOperation = PASS; v.aluSel = RIGHT);
	WRITE_VALUE(JUMP_IF_CARRY, 4, v.oeSel = ALU; v.leSel = PCH);
	WRITE_VALUE(JUMP_IF_CARRY, 5, v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_CARRY, 0, v.flagSel = FLAG2; v.pcCountEnable = true, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_CARRY, 0, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.aluOperation = PASS; v.aluSel = LEFT; v.leSel = ALU, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_CARRY, 1, v.flagSel = FLAG2; v.pcCountEnable = true, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_CARRY, 1, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.ceTempLE = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_CARRY, 2, v.flagSel = FLAG2; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_CARRY, 2, v.flagSel = FLAG2; v.oeSel = ALU; v.leSel = PCL, 0, -1, -1);
	WRITE_VALUE(JUMP_IF_NOT_CARRY, 3, v.leSel = ALU; v.busSel = TEMP; v.aluOperation = PASS; v.aluSel = RIGHT);
	WRITE_VALUE(JUMP_IF_NOT_CARRY, 4, v.oeSel = ALU; v.leSel = PCH);
	WRITE_VALUE(JUMP_IF_NOT_CARRY, 5, v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_CONDITIONAL_VALUE(JUMP_IF_ZERO, 0, v.flagSel = FLAG2; v.pcCountEnable = true, -1, 0, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_ZERO, 0, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.aluOperation = PASS; v.aluSel = LEFT; v.leSel = ALU, -1, 1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_ZERO, 1, v.flagSel = FLAG2; v.pcCountEnable = true, -1, 0, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_ZERO, 1, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.ceTempLE = true, -1, 1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_ZERO, 2, v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true, -1, 0, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_ZERO, 2, v.flagSel = FLAG2; v.flagSel = FLAG2; v.oeSel = ALU; v.leSel = PCL, -1, 1, -1);
	WRITE_VALUE(JUMP_IF_ZERO, 3, v.leSel = ALU; v.busSel = TEMP; v.aluOperation = PASS; v.aluSel = RIGHT);
	WRITE_VALUE(JUMP_IF_ZERO, 4, v.oeSel = ALU; v.leSel = PCH);
	WRITE_VALUE(JUMP_IF_ZERO, 5, v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_ZERO, 0, v.flagSel = FLAG2; v.pcCountEnable = true, -1, 1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_ZERO, 0, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.aluOperation = PASS; v.aluSel = LEFT; v.leSel = ALU, -1, 0, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_ZERO, 1, v.flagSel = FLAG2; v.pcCountEnable = true, -1, 1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_ZERO, 1, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.ceTempLE = true, -1, 0, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_ZERO, 2, v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true, -1, 1, -1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_ZERO, 2, v.flagSel = FLAG2; v.oeSel = ALU; v.leSel = PCL, -1, 0, -1);
	WRITE_VALUE(JUMP_IF_NOT_ZERO, 3, v.leSel = ALU; v.busSel = TEMP; v.aluOperation = PASS; v.aluSel = RIGHT);
	WRITE_VALUE(JUMP_IF_NOT_ZERO, 4, v.oeSel = ALU; v.leSel = PCH);
	WRITE_VALUE(JUMP_IF_NOT_ZERO, 5, v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_CONDITIONAL_VALUE(JUMP_IF_SIGN, 0, v.flagSel = FLAG2; v.pcCountEnable = true, -1, -1, 0);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_SIGN, 0, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.aluOperation = PASS; v.aluSel = LEFT; v.leSel = ALU, -1, -1, 1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_SIGN, 1, v.flagSel = FLAG2; v.pcCountEnable = true, -1, -1, 0);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_SIGN, 1, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.ceTempLE = true, -1, -1, 1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_SIGN, 2, v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true, -1, -1, 0);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_SIGN, 2, v.flagSel = FLAG2; v.oeSel = ALU; v.leSel = PCL, -1, -1, 1);
	WRITE_VALUE(JUMP_IF_SIGN, 3, v.leSel = ALU; v.busSel = TEMP; v.aluOperation = PASS; v.aluSel = RIGHT);
	WRITE_VALUE(JUMP_IF_SIGN, 4, v.oeSel = ALU; v.leSel = PCH);
	WRITE_VALUE(JUMP_IF_SIGN, 5, v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_SIGN, 0, v.flagSel = FLAG2; v.pcCountEnable = true, -1, -1, 1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_SIGN, 0, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.aluOperation = PASS; v.aluSel = LEFT; v.leSel = ALU, -1, -1, 0);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_SIGN, 1, v.flagSel = FLAG2; v.pcCountEnable = true, -1, -1, 1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_SIGN, 1, v.flagSel = FLAG2; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.ceTempLE = true, -1, -1, 0);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_SIGN, 2, v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true, -1, -1, 1);
	WRITE_CONDITIONAL_VALUE(JUMP_IF_NOT_SIGN, 2, v.flagSel = FLAG2; v.oeSel = ALU; v.leSel = PCL, -1, -1, 0);
	WRITE_VALUE(JUMP_IF_NOT_SIGN, 3, v.leSel = ALU; v.busSel = TEMP; v.aluOperation = PASS; v.aluSel = RIGHT);
	WRITE_VALUE(JUMP_IF_NOT_SIGN, 4, v.oeSel = ALU; v.leSel = PCH);
	WRITE_VALUE(JUMP_IF_NOT_SIGN, 5, v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(AND_LITERAL, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(AND_LITERAL, 1, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.leSel = ALU; v.aluOperation = AND; v.aluSel = RIGHT; v.busSel = TEMP; v.flag2LE = false);
	WRITE_VALUE(AND_LITERAL, 2, v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(OR_LITERAL, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(OR_LITERAL, 1, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.leSel = ALU; v.aluOperation = OR; v.aluSel = RIGHT; v.busSel = TEMP; v.flag2LE = false);
	WRITE_VALUE(OR_LITERAL, 2, v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(ADC_R0, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADC_R0, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADC_R1, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADC_R1, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R1; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADC_R2, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADC_R2, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R2; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADC_R3, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADC_R3, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R3; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADC_R4, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADC_R4, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R4; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADC_R5, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADC_R5, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R5; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADC_R6, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADC_R6, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R6; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(ADC_R7, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(ADC_R7, 1, v.aluOperation = ADD; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R7; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(SBB_R0, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SBB_R0, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R0; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SBB_R1, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SBB_R1, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R1; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SBB_R2, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SBB_R2, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R2; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SBB_R3, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SBB_R3, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R3; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SBB_R4, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SBB_R4, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R4; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SBB_R5, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SBB_R5, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R5; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SBB_R6, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SBB_R6, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R6; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);
	WRITE_VALUE(SBB_R7, 0, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SBB_R7, 1, v.aluOperation = SUB; v.aluSel = RIGHT; v.flag2LE = false; v.busSel = TEMP; v.oeSel = R7; v.leSel = ALU; v.carrySel = GENERATED; v.flagSel = FLAG2; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(EVALUATE, 0, v.oeSel = ALU; v.aluOperation = PASS; v.aluSel = LEFT; v.flag2LE = false; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true; v.pcCountEnable = true; v.memSel = PC; v.memOE = true);

	WRITE_VALUE(LOAD_MEM, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.arLLE = true);
	WRITE_VALUE(LOAD_MEM, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM, 2, v.memOE = true; v.memSel = ADDRESS; v.aluOperation = PASS; v.aluSel = LEFT; v.leSel = ALU);
	WRITE_VALUE(LOAD_MEM, 3, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(SAVE_MEM, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.arLLE = true);
	WRITE_VALUE(SAVE_MEM, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.arHLE = true);
	WRITE_VALUE(SAVE_MEM, 2, v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = ALU);
	WRITE_VALUE(SAVE_MEM, 3, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);


	//Mul. 0 -> R7, R6, 8 -> R4; R0 * R1 -> R6, R7, 0 -> R0, R4
	WRITE_VALUE(MUL, 0, v.oeSel = R6; v.aluOperation = SHIFT_LEFT; v.aluSel = LEFT; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(MUL, 1, v.oeSel = ALU; v.leSel = R6);
	WRITE_VALUE(MUL, 2, v.oeSel = R7; v.aluOperation = SHIFT_LEFT; v.aluSel = LEFT; v.leSel = ALU; v.flagSel = FLAG1; v.carrySel = GENERATED);
	WRITE_VALUE(MUL, 3, v.oeSel = ALU; v.leSel = R7);
	WRITE_VALUE(MUL, 4, v.oeSel = R0; v.aluOperation = SHIFT_LEFT; v.aluSel = LEFT; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(MUL, 5, v.oeSel = ALU; v.leSel = R0);
	WRITE_CONDITIONAL_VALUE(MUL, 6, v.flagSel = FLAG1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(MUL, 6, v.flagSel = FLAG1; v.oeSel = R1; v.ceTempLE = true, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(MUL, 7, v.flagSel = FLAG1; v.oeSel = R6; v.aluOperation = PASS; v.aluSel = LEFT; v.leSel = ALU; v.flag1LE = false, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(MUL, 7, v.flagSel = FLAG1; v.oeSel = R6; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag1LE = false, 1, -1, -1);
	WRITE_VALUE(MUL, 8, v.oeSel = ALU; v.leSel = R6);
	WRITE_CONDITIONAL_VALUE(MUL, 9, v.flagSel = FLAG1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(MUL, 9, v.flagSel = FLAG1; v.oeSel = R7; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(MUL, 10, v.flagSel = FLAG1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(MUL, 10, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R7, 1, -1, -1);
	WRITE_VALUE(MUL, 11, v.oeSel = R4; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(MUL, 12, v.oeSel = ALU; v.leSel = R4);
	WRITE_CONDITIONAL_VALUE(MUL, 13, v.flagSel = FLAG1; v.instructionCounterReset = true, -1, 0, -1);
	WRITE_CONDITIONAL_VALUE(MUL, 13, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, -1, 1, -1);

	//Div. 0 -> R6, 8 -> R4; R0 / R1 -> R0, R6, 0 -> R4
	WRITE_VALUE(DIV, 0, v.oeSel = R0; v.aluOperation = SHIFT_LEFT; v.aluSel = LEFT; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(DIV, 1, v.oeSel = ALU; v.leSel = R0);
	WRITE_VALUE(DIV, 2, v.oeSel = R6; v.aluOperation = SHIFT_LEFT; v.aluSel = LEFT; v.leSel = ALU; v.flagSel = FLAG1; v.carrySel = GENERATED);
	WRITE_VALUE(DIV, 3, v.oeSel = ALU; v.leSel = R6);
	WRITE_VALUE(DIV, 4, v.oeSel = R1; v.ceTempLE = true);
	WRITE_VALUE(DIV, 5, v.oeSel = R6; v.aluOperation = SUB; v.aluSel = RIGHT; v.carrySpec = true; v.leSel = ALU; v.flag1LE = false; v.busSel = TEMP);
	WRITE_CONDITIONAL_VALUE(DIV, 6, v.flagSel = FLAG1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(DIV, 6, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R6, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(DIV, 7, v.flagSel = FLAG1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(DIV, 7, v.flagSel = FLAG1; v.oeSel = R0; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(DIV, 8, v.flagSel = FLAG1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(DIV, 8, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R0, 1, -1, -1);
	WRITE_VALUE(DIV, 9, v.oeSel = R4; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(DIV, 10, v.oeSel = ALU; v.leSel = R4);
	WRITE_CONDITIONAL_VALUE(DIV, 11, v.flagSel = FLAG1; v.instructionCounterReset = true, -1, 0, -1);
	WRITE_CONDITIONAL_VALUE(DIV, 11, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, -1, 1, -1);

	//Memset. Ptr -> R2, R3, Count -> R4, R5, Value -> R0; 0 -> R4, R5. Count should be at least one!
	WRITE_VALUE(MEMSET, 0, v.oeSel = R2; v.arLLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(MEMSET, 1, v.oeSel = R3; v.arHLE = true);
	WRITE_VALUE(MEMSET, 2, v.oeSel = R0; v.memOE = true; v.memSel = ADDRESS; v.memWrite = true);
	WRITE_VALUE(MEMSET, 3, v.oeSel = ALU; v.leSel = R2);
	WRITE_CONDITIONAL_VALUE(MEMSET, 4, v.flagSel = FLAG1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(MEMSET, 4, v.flagSel = FLAG1; v.oeSel = R3; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(MEMSET, 5, v.flagSel = FLAG1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(MEMSET, 5, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R3, 1, -1, -1);
	WRITE_VALUE(MEMSET, 6, v.oeSel = R4; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(MEMSET, 7, v.oeSel = ALU; v.leSel = R4);
	WRITE_CONDITIONAL_VALUE(MEMSET, 8, v.flagSel = FLAG1; v.oeSel = R5; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(MEMSET, 8, v.flagSel = FLAG1; v.instructionCounterReset = true, 1, 0, -1);
	WRITE_CONDITIONAL_VALUE(MEMSET, 8, v.flagSel = FLAG1; v.oeSel = R5; v.aluOperation = PASS; v.aluSel = LEFT; v.flag1LE = false, 1, 1, -1);
	WRITE_CONDITIONAL_VALUE(MEMSET, 9, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R5; v.instructionCounterReset = true, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(MEMSET, 9, v.flagSel = FLAG1; v.instructionCounterReset = true, 0, 0, -1);
	WRITE_CONDITIONAL_VALUE(MEMSET, 9, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, 1, -1);

	//Memcpy. Dest -> R2, R3, Src -> R0, R1, Count -> R4, R5
	WRITE_VALUE(MEMCPY, 0, v.oeSel = R0; v.arLLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(MEMCPY, 1, v.oeSel = ALU; v.leSel = R0);
	WRITE_VALUE(MEMCPY, 2, v.oeSel = R1; v.arHLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU);
	WRITE_CONDITIONAL_VALUE(MEMCPY, 3, v.flagSel = FLAG1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(MEMCPY, 3, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R1, 1, -1, -1);
	WRITE_VALUE(MEMCPY, 4, v.memOE = true; v.memSel = ADDRESS; v.ceTempLE = true);
	WRITE_VALUE(MEMCPY, 5, v.oeSel = R2; v.arLLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(MEMCPY, 6, v.oeSel = ALU; v.leSel = R2);
	WRITE_VALUE(MEMCPY, 7, v.oeSel = R1; v.arHLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU);
	WRITE_CONDITIONAL_VALUE(MEMCPY, 8, v.flagSel = FLAG1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(MEMCPY, 8, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R3, 1, -1, -1);
	WRITE_VALUE(MEMCPY, 9, v.leSel = ALU; v.aluOperation = PASS; v.aluSel = RIGHT; v.busSel = TEMP);
	WRITE_VALUE(MEMCPY, 10, v.oeSel = ALU; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS);
	WRITE_VALUE(MEMCPY, 11, v.oeSel = R4; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(MEMCPY, 12, v.oeSel = ALU; v.leSel = R4);
	WRITE_CONDITIONAL_VALUE(MEMCPY, 13, v.flagSel = FLAG1; v.oeSel = R5; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(MEMCPY, 13, v.flagSel = FLAG1; v.instructionCounterReset = true, 1, 0, -1);
	WRITE_CONDITIONAL_VALUE(MEMCPY, 13, v.flagSel = FLAG1; v.oeSel = R5; v.aluOperation = PASS; v.aluSel = LEFT; v.flag1LE = false, 1, 1, -1);
	WRITE_CONDITIONAL_VALUE(MEMCPY, 14, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R5; v.instructionCounterReset = true, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(MEMCPY, 14, v.flagSel = FLAG1; v.instructionCounterReset = true, 0, 0, -1);
	WRITE_CONDITIONAL_VALUE(MEMCPY, 14, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, 1, -1);




	WRITE_VALUE(ZERO_R0, 0, v.leSel = ALU; v.aluOperation = PASS; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 0);
	WRITE_VALUE(ZERO_R0, 1, v.oeSel = ALU; v.leSel = R0; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(ZERO_R1, 0, v.leSel = ALU; v.aluOperation = PASS; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 0);
	WRITE_VALUE(ZERO_R1, 1, v.oeSel = ALU; v.leSel = R1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(ZERO_R2, 0, v.leSel = ALU; v.aluOperation = PASS; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 0);
	WRITE_VALUE(ZERO_R2, 1, v.oeSel = ALU; v.leSel = R2; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(ZERO_R3, 0, v.leSel = ALU; v.aluOperation = PASS; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 0);
	WRITE_VALUE(ZERO_R3, 1, v.oeSel = ALU; v.leSel = R3; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(ZERO_R4, 0, v.leSel = ALU; v.aluOperation = PASS; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 0);
	WRITE_VALUE(ZERO_R4, 1, v.oeSel = ALU; v.leSel = R4; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(ZERO_R5, 0, v.leSel = ALU; v.aluOperation = PASS; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 0);
	WRITE_VALUE(ZERO_R5, 1, v.oeSel = ALU; v.leSel = R5; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(ZERO_R6, 0, v.leSel = ALU; v.aluOperation = PASS; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 0);
	WRITE_VALUE(ZERO_R6, 1, v.oeSel = ALU; v.leSel = R6; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(ZERO_R7, 0, v.leSel = ALU; v.aluOperation = PASS; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 0);
	WRITE_VALUE(ZERO_R7, 1, v.oeSel = ALU; v.leSel = R7; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);


	WRITE_VALUE(LOAD_MEM_RA, 0, v.oeSel = R0; v.arLLE = true);
	WRITE_VALUE(LOAD_MEM_RA, 1, v.oeSel = R1; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_RA, 2, v.memOE = true; v.memSel = ADDRESS; v.leSel = ALU);
	WRITE_VALUE(LOAD_MEM_RA, 3, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(LOAD_MEM_RB, 0, v.oeSel = R2; v.arLLE = true);
	WRITE_VALUE(LOAD_MEM_RB, 1, v.oeSel = R3; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_RB, 2, v.memOE = true; v.memSel = ADDRESS; v.leSel = ALU);
	WRITE_VALUE(LOAD_MEM_RB, 3, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(LOAD_MEM_RC, 0, v.oeSel = R4; v.arLLE = true);
	WRITE_VALUE(LOAD_MEM_RC, 1, v.oeSel = R5; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_RC, 2, v.memOE = true; v.memSel = ADDRESS; v.leSel = ALU);
	WRITE_VALUE(LOAD_MEM_RC, 3, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(LOAD_MEM_RD, 0, v.oeSel = R6; v.arLLE = true);
	WRITE_VALUE(LOAD_MEM_RD, 1, v.oeSel = R7; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_RD, 2, v.memOE = true; v.memSel = ADDRESS; v.leSel = ALU);
	WRITE_VALUE(LOAD_MEM_RD, 3, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(SAVE_MEM_RA, 0, v.oeSel = R0; v.arLLE = true);
	WRITE_VALUE(SAVE_MEM_RA, 1, v.oeSel = R1; v.arHLE = true);
	WRITE_VALUE(SAVE_MEM_RA, 2, v.memOE = true; v.memWrite = true; v.oeSel = ALU);
	WRITE_VALUE(SAVE_MEM_RA, 3, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(SAVE_MEM_RB, 0, v.oeSel = R2; v.arLLE = true);
	WRITE_VALUE(SAVE_MEM_RB, 1, v.oeSel = R3; v.arHLE = true);
	WRITE_VALUE(SAVE_MEM_RB, 2, v.memOE = true; v.memWrite = true; v.oeSel = ALU);
	WRITE_VALUE(SAVE_MEM_RB, 3, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(SAVE_MEM_RC, 0, v.oeSel = R4; v.arLLE = true);
	WRITE_VALUE(SAVE_MEM_RC, 1, v.oeSel = R5; v.arHLE = true);
	WRITE_VALUE(SAVE_MEM_RC, 2, v.memOE = true; v.memWrite = true; v.oeSel = ALU);
	WRITE_VALUE(SAVE_MEM_RC, 3, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(SAVE_MEM_RD, 0, v.oeSel = R6; v.arLLE = true);
	WRITE_VALUE(SAVE_MEM_RD, 1, v.oeSel = R7; v.arHLE = true);
	WRITE_VALUE(SAVE_MEM_RD, 2, v.memOE = true; v.memWrite = true; v.oeSel = ALU);
	WRITE_VALUE(SAVE_MEM_RD, 3, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);


	WRITE_VALUE(LOAD_MEM_TO_RA, 0, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.arLLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.leSel = ALU; v.busSel = IMMEDIATE; v.immediate = 1; v.flag1LE = false);
	WRITE_VALUE(LOAD_MEM_TO_RA, 1, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_TO_RA, 2, v.memSel = ADDRESS; v.memOE = true; v.leSel = R0);
	WRITE_VALUE(LOAD_MEM_TO_RA, 3, v.oeSel = ALU; v.arLLE = true);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA, 4, v.flagSel = FLAG1; v.memSel = ADDRESS; v.memOE = true; v.leSel = R1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA, 4, v.flagSel = FLAG1; v.memSel = PC; v.memOE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA, 5, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA, 5, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(LOAD_MEM_TO_RA, 6, v.memSel = ADDRESS; v.memOE = true; v.leSel = R1);
	WRITE_VALUE(LOAD_MEM_TO_RA, 7, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	
	WRITE_VALUE(LOAD_MEM_TO_RB, 0, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.arLLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.leSel = ALU; v.busSel = IMMEDIATE; v.immediate = 1; v.flag1LE = false);
	WRITE_VALUE(LOAD_MEM_TO_RB, 1, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_TO_RB, 2, v.memSel = ADDRESS; v.memOE = true; v.leSel = R2);
	WRITE_VALUE(LOAD_MEM_TO_RB, 3, v.oeSel = ALU; v.arLLE = true);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RB, 4, v.flagSel = FLAG1; v.memSel = ADDRESS; v.memOE = true; v.leSel = R3, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RB, 4, v.flagSel = FLAG1; v.memSel = PC; v.memOE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RB, 5, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RB, 5, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(LOAD_MEM_TO_RB, 6, v.memSel = ADDRESS; v.memOE = true; v.leSel = R3);
	WRITE_VALUE(LOAD_MEM_TO_RB, 7, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	
	WRITE_VALUE(LOAD_MEM_TO_RC, 0, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.arLLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.leSel = ALU; v.busSel = IMMEDIATE; v.immediate = 1; v.flag1LE = false);
	WRITE_VALUE(LOAD_MEM_TO_RC, 1, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_TO_RC, 2, v.memSel = ADDRESS; v.memOE = true; v.leSel = R4);
	WRITE_VALUE(LOAD_MEM_TO_RC, 3, v.oeSel = ALU; v.arLLE = true);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RC, 4, v.flagSel = FLAG1; v.memSel = ADDRESS; v.memOE = true; v.leSel = R5, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RC, 4, v.flagSel = FLAG1; v.memSel = PC; v.memOE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RC, 5, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RC, 5, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(LOAD_MEM_TO_RC, 6, v.memSel = ADDRESS; v.memOE = true; v.leSel = R5);
	WRITE_VALUE(LOAD_MEM_TO_RC, 7, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	
	WRITE_VALUE(LOAD_MEM_TO_RD, 0, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.arLLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.leSel = ALU; v.busSel = IMMEDIATE; v.immediate = 1; v.flag1LE = false);
	WRITE_VALUE(LOAD_MEM_TO_RD, 1, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_TO_RD, 2, v.memSel = ADDRESS; v.memOE = true; v.leSel = R6);
	WRITE_VALUE(LOAD_MEM_TO_RD, 3, v.oeSel = ALU; v.arLLE = true);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RD, 4, v.flagSel = FLAG1; v.memSel = ADDRESS; v.memOE = true; v.leSel = R7, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RD, 4, v.flagSel = FLAG1; v.memSel = PC; v.memOE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RD, 5, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RD, 5, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(LOAD_MEM_TO_RD, 6, v.memSel = ADDRESS; v.memOE = true; v.leSel = R7);
	WRITE_VALUE(LOAD_MEM_TO_RD, 7, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	

	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RB, 0, v.oeSel = R2; v.arLLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RB, 1, v.oeSel = R3; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RB, 2, v.memSel = ADDRESS; v.memOE = true; v.leSel = R0);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RB, 3, v.oeSel = ALU; v.arLLE = true);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RB, 4, v.flagSel = FLAG1; v.memSel = ADDRESS; v.memOE = true; v.leSel = R1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RB, 4, v.flagSel = FLAG1; v.oeSel = R3; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RB, 5, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RB, 5, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RB, 6, v.memSel = ADDRESS; v.memOE = true; v.leSel = R1);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RB, 7, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RC, 0, v.oeSel = R4; v.arLLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RC, 1, v.oeSel = R5; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RC, 2, v.memSel = ADDRESS; v.memOE = true; v.leSel = R0);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RC, 3, v.oeSel = ALU; v.arLLE = true);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RC, 4, v.flagSel = FLAG1; v.memSel = ADDRESS; v.memOE = true; v.leSel = R1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RC, 4, v.flagSel = FLAG1; v.oeSel = R5; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RC, 5, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RC, 5, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RC, 6, v.memSel = ADDRESS; v.memOE = true; v.leSel = R1);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RC, 7, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RD, 0, v.oeSel = R6; v.arLLE = true; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RD, 1, v.oeSel = R7; v.arHLE = true);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RD, 2, v.memSel = ADDRESS; v.memOE = true; v.leSel = R0);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RD, 3, v.oeSel = ALU; v.arLLE = true);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RD, 4, v.flagSel = FLAG1; v.memSel = ADDRESS; v.memOE = true; v.leSel = R1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RD, 4, v.flagSel = FLAG1; v.oeSel = R7; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RD, 5, v.flagSel = FLAG1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(LOAD_MEM_TO_RA_USING_RD, 5, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RD, 6, v.memSel = ADDRESS; v.memOE = true; v.leSel = R1);
	WRITE_VALUE(LOAD_MEM_TO_RA_USING_RD, 7, v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);


	WRITE_VALUE(INC_RA, 0, v.oeSel = R0; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false);
	WRITE_CONDITIONAL_VALUE(INC_RA, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R0; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1); 
	WRITE_CONDITIONAL_VALUE(INC_RA, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R0, 1, -1, -1);
	WRITE_VALUE(INC_RA, 2, v.oeSel = R1; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU);
	WRITE_VALUE(INC_RA, 3, v.oeSel = ALU; v.leSel = R1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(INC_RB, 0, v.oeSel = R2; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false);
	WRITE_CONDITIONAL_VALUE(INC_RB, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R2; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1); 
	WRITE_CONDITIONAL_VALUE(INC_RB, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R2, 1, -1, -1);
	WRITE_VALUE(INC_RB, 2, v.oeSel = R3; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU);
	WRITE_VALUE(INC_RB, 3, v.oeSel = ALU; v.leSel = R3; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(INC_RC, 0, v.oeSel = R4; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false);
	WRITE_CONDITIONAL_VALUE(INC_RC, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R4; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1); 
	WRITE_CONDITIONAL_VALUE(INC_RC, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R4, 1, -1, -1);
	WRITE_VALUE(INC_RC, 2, v.oeSel = R5; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU);
	WRITE_VALUE(INC_RC, 3, v.oeSel = ALU; v.leSel = R5; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(INC_RD, 0, v.oeSel = R6; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false);
	WRITE_CONDITIONAL_VALUE(INC_RD, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R6; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 0, -1, -1); 
	WRITE_CONDITIONAL_VALUE(INC_RD, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R6, 1, -1, -1);
	WRITE_VALUE(INC_RD, 2, v.oeSel = R7; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU);
	WRITE_VALUE(INC_RD, 3, v.oeSel = ALU; v.leSel = R7; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);


	WRITE_VALUE(DEC_RA, 0, v.oeSel = R0; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_CONDITIONAL_VALUE(DEC_RA, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R0; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 1, -1, -1); 
	WRITE_CONDITIONAL_VALUE(DEC_RA, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R0, 0, -1, -1);
	WRITE_VALUE(DEC_RA, 2, v.oeSel = R1; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_VALUE(DEC_RA, 3, v.oeSel = ALU; v.leSel = R1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(DEC_RB, 0, v.oeSel = R2; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_CONDITIONAL_VALUE(DEC_RB, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R2; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 1, -1, -1); 
	WRITE_CONDITIONAL_VALUE(DEC_RB, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R2, 0, -1, -1);
	WRITE_VALUE(DEC_RB, 2, v.oeSel = R3; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_VALUE(DEC_RB, 3, v.oeSel = ALU; v.leSel = R3; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(DEC_RC, 0, v.oeSel = R4; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_CONDITIONAL_VALUE(DEC_RC, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R4; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 1, -1, -1); 
	WRITE_CONDITIONAL_VALUE(DEC_RC, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R4, 0, -1, -1);
	WRITE_VALUE(DEC_RC, 2, v.oeSel = R5; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_VALUE(DEC_RC, 3, v.oeSel = ALU; v.leSel = R5; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(DEC_RD, 0, v.oeSel = R6; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_CONDITIONAL_VALUE(DEC_RD, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R6; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true, 1, -1, -1); 
	WRITE_CONDITIONAL_VALUE(DEC_RD, 1, v.flagSel = FLAG1; v.oeSel = ALU; v.leSel = R6, 0, -1, -1);
	WRITE_VALUE(DEC_RD, 2, v.oeSel = R7; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_VALUE(DEC_RD, 3, v.oeSel = ALU; v.leSel = R7; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);



	WRITE_VALUE(COPY_RB_TO_RA, 0, v.oeSel = R2; v.leSel = R0);
	WRITE_VALUE(COPY_RB_TO_RA, 1, v.oeSel = R3; v.leSel = R1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(COPY_RC_TO_RA, 0, v.oeSel = R4; v.leSel = R0);
	WRITE_VALUE(COPY_RC_TO_RA, 1, v.oeSel = R5; v.leSel = R1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(COPY_RD_TO_RA, 0, v.oeSel = R6; v.leSel = R0);
	WRITE_VALUE(COPY_RD_TO_RA, 1, v.oeSel = R7; v.leSel = R1; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	
	WRITE_VALUE(COPY_RA_TO_RB, 0, v.oeSel = R0; v.leSel = R2);
	WRITE_VALUE(COPY_RA_TO_RB, 1, v.oeSel = R0; v.leSel = R3; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(COPY_RA_TO_RC, 0, v.oeSel = R0; v.leSel = R4);
	WRITE_VALUE(COPY_RA_TO_RC, 1, v.oeSel = R0; v.leSel = R5; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(COPY_RA_TO_RD, 0, v.oeSel = R0; v.leSel = R6);
	WRITE_VALUE(COPY_RA_TO_RD, 1, v.oeSel = R0; v.leSel = R7; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);


	WRITE_VALUE(ADD_RB_TO_RA, 0, v.oeSel = R2; v.ceTempLE = true);
	WRITE_VALUE(ADD_RB_TO_RA, 1, v.oeSel = R0; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false);
	WRITE_VALUE(ADD_RB_TO_RA, 2, v.oeSel = ALU; v.leSel = R0);
	WRITE_VALUE(ADD_RB_TO_RA, 3, v.oeSel = R3; v.ceTempLE = true);
	WRITE_VALUE(ADD_RB_TO_RA, 4, v.oeSel = R1; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false; v.flagSel = FLAG2; v.carrySel = GENERATED);
	WRITE_VALUE(ADD_RB_TO_RA, 5, v.oeSel = ALU; v.leSel = R1);

	WRITE_VALUE(ADD_RC_TO_RA, 0, v.oeSel = R4; v.ceTempLE = true);
	WRITE_VALUE(ADD_RC_TO_RA, 1, v.oeSel = R0; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false);
	WRITE_VALUE(ADD_RC_TO_RA, 2, v.oeSel = ALU; v.leSel = R0);
	WRITE_VALUE(ADD_RC_TO_RA, 3, v.oeSel = R5; v.ceTempLE = true);
	WRITE_VALUE(ADD_RC_TO_RA, 4, v.oeSel = R1; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false; v.flagSel = FLAG2; v.carrySel = GENERATED);
	WRITE_VALUE(ADD_RC_TO_RA, 5, v.oeSel = ALU; v.leSel = R1);

	WRITE_VALUE(ADD_RD_TO_RA, 0, v.oeSel = R6; v.ceTempLE = true);
	WRITE_VALUE(ADD_RD_TO_RA, 1, v.oeSel = R0; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false);
	WRITE_VALUE(ADD_RD_TO_RA, 2, v.oeSel = ALU; v.leSel = R0);
	WRITE_VALUE(ADD_RD_TO_RA, 3, v.oeSel = R7; v.ceTempLE = true);
	WRITE_VALUE(ADD_RD_TO_RA, 4, v.oeSel = R1; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false; v.flagSel = FLAG2; v.carrySel = GENERATED);
	WRITE_VALUE(ADD_RD_TO_RA, 5, v.oeSel = ALU; v.leSel = R1);


	WRITE_VALUE(SUB_RB_FROM_RA, 0, v.oeSel = R2; v.ceTempLE = true);
	WRITE_VALUE(SUB_RB_FROM_RA, 1, v.oeSel = R0; v.aluOperation = SUB; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false; v.carrySpec = true);
	WRITE_VALUE(SUB_RB_FROM_RA, 2, v.oeSel = ALU; v.leSel = R0);
	WRITE_VALUE(SUB_RB_FROM_RA, 3, v.oeSel = R3; v.ceTempLE = true);
	WRITE_VALUE(SUB_RB_FROM_RA, 4, v.oeSel = R1; v.aluOperation = SUB; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false; v.flagSel = FLAG2; v.carrySel = GENERATED);
	WRITE_VALUE(SUB_RB_FROM_RA, 5, v.oeSel = ALU; v.leSel = R1);

	WRITE_VALUE(SUB_RC_FROM_RA, 0, v.oeSel = R4; v.ceTempLE = true);
	WRITE_VALUE(SUB_RC_FROM_RA, 1, v.oeSel = R0; v.aluOperation = SUB; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false; v.carrySpec = true);
	WRITE_VALUE(SUB_RC_FROM_RA, 2, v.oeSel = ALU; v.leSel = R0);
	WRITE_VALUE(SUB_RC_FROM_RA, 3, v.oeSel = R5; v.ceTempLE = true);
	WRITE_VALUE(SUB_RC_FROM_RA, 4, v.oeSel = R1; v.aluOperation = SUB; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false; v.flagSel = FLAG2; v.carrySel = GENERATED);
	WRITE_VALUE(SUB_RC_FROM_RA, 5, v.oeSel = ALU; v.leSel = R1);

	WRITE_VALUE(SUB_RD_FROM_RA, 0, v.oeSel = R6; v.ceTempLE = true);
	WRITE_VALUE(SUB_RD_FROM_RA, 1, v.oeSel = R0; v.aluOperation = SUB; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false; v.carrySpec = true);
	WRITE_VALUE(SUB_RD_FROM_RA, 2, v.oeSel = ALU; v.leSel = R0);
	WRITE_VALUE(SUB_RD_FROM_RA, 3, v.oeSel = R7; v.ceTempLE = true);
	WRITE_VALUE(SUB_RD_FROM_RA, 4, v.oeSel = R1; v.aluOperation = SUB; v.aluSel = RIGHT; v.busSel = TEMP; v.leSel = ALU; v.flag2LE = false; v.flagSel = FLAG2; v.carrySel = GENERATED);
	WRITE_VALUE(SUB_RD_FROM_RA, 5, v.oeSel = ALU; v.leSel = R1);


	WRITE_VALUE(PUSH_R0, 0, v.arLLE = true; v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(PUSH_R0, 1, v.oeSel = ALU; v.leSel = SPL; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(PUSH_R0, 2, v.arHLE = true; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_CONDITIONAL_VALUE(PUSH_R0, 3, v.oeSel = ALU; v.leSel = SPH, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(PUSH_R0, 3, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R0, 1, -1, -1);
	WRITE_VALUE(PUSH_R0, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R0);

	WRITE_VALUE(PUSH_R1, 0, v.arLLE = true; v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(PUSH_R1, 1, v.oeSel = ALU; v.leSel = SPL; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(PUSH_R1, 2, v.arHLE = true; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_CONDITIONAL_VALUE(PUSH_R1, 3, v.oeSel = ALU; v.leSel = SPH, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(PUSH_R1, 3, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R1, 1, -1, -1);
	WRITE_VALUE(PUSH_R1, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R1);

	WRITE_VALUE(PUSH_R2, 0, v.arLLE = true; v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(PUSH_R2, 1, v.oeSel = ALU; v.leSel = SPL; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(PUSH_R2, 2, v.arHLE = true; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_CONDITIONAL_VALUE(PUSH_R2, 3, v.oeSel = ALU; v.leSel = SPH, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(PUSH_R2, 3, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R2, 1, -1, -1);
	WRITE_VALUE(PUSH_R2, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R2);

	WRITE_VALUE(PUSH_R3, 0, v.arLLE = true; v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(PUSH_R3, 1, v.oeSel = ALU; v.leSel = SPL; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(PUSH_R3, 2, v.arHLE = true; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_CONDITIONAL_VALUE(PUSH_R3, 3, v.oeSel = ALU; v.leSel = SPH, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(PUSH_R3, 3, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R3, 1, -1, -1);
	WRITE_VALUE(PUSH_R3, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R3);

	WRITE_VALUE(PUSH_R4, 0, v.arLLE = true; v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(PUSH_R4, 1, v.oeSel = ALU; v.leSel = SPL; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(PUSH_R4, 2, v.arHLE = true; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_CONDITIONAL_VALUE(PUSH_R4, 3, v.oeSel = ALU; v.leSel = SPH, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(PUSH_R4, 3, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R4, 1, -1, -1);
	WRITE_VALUE(PUSH_R4, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R4);

	WRITE_VALUE(PUSH_R5, 0, v.arLLE = true; v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(PUSH_R5, 1, v.oeSel = ALU; v.leSel = SPL; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(PUSH_R5, 2, v.arHLE = true; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_CONDITIONAL_VALUE(PUSH_R5, 3, v.oeSel = ALU; v.leSel = SPH, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(PUSH_R5, 3, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R5, 1, -1, -1);
	WRITE_VALUE(PUSH_R5, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R5);

	WRITE_VALUE(PUSH_R6, 0, v.arLLE = true; v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(PUSH_R6, 1, v.oeSel = ALU; v.leSel = SPL; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(PUSH_R6, 2, v.arHLE = true; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_CONDITIONAL_VALUE(PUSH_R6, 3, v.oeSel = ALU; v.leSel = SPH, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(PUSH_R6, 3, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R6, 1, -1, -1);
	WRITE_VALUE(PUSH_R6, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R6);

	WRITE_VALUE(PUSH_R7, 0, v.arLLE = true; v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU; v.flag1LE = false);
	WRITE_VALUE(PUSH_R7, 1, v.oeSel = ALU; v.leSel = SPL; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(PUSH_R7, 2, v.arHLE = true; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 3; v.leSel = ALU);
	WRITE_CONDITIONAL_VALUE(PUSH_R7, 3, v.oeSel = ALU; v.leSel = SPH, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(PUSH_R7, 3, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R7, 1, -1, -1);
	WRITE_VALUE(PUSH_R7, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memWrite = true; v.memSel = ADDRESS; v.oeSel = R7);




	WRITE_VALUE(POP_R0, 0, v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(POP_R0, 1, v.arLLE = true; v.oeSel = ALU; v.leSel = SPL);
	WRITE_CONDITIONAL_VALUE(POP_R0, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.arHLE = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R0, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R0, 3, v.flagSel = FLAG1; v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R0, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R0, 3, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(POP_R0, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R0);

	WRITE_VALUE(POP_R1, 0, v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(POP_R1, 1, v.arLLE = true; v.oeSel = ALU; v.leSel = SPL);
	WRITE_CONDITIONAL_VALUE(POP_R1, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.arHLE = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R1, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R1, 3, v.flagSel = FLAG1; v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R1, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R1, 3, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(POP_R1, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R1);

	WRITE_VALUE(POP_R2, 0, v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(POP_R2, 1, v.arLLE = true; v.oeSel = ALU; v.leSel = SPL);
	WRITE_CONDITIONAL_VALUE(POP_R2, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.arHLE = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R2, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R2, 3, v.flagSel = FLAG1; v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R2, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R2, 3, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(POP_R2, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R2);

	WRITE_VALUE(POP_R3, 0, v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(POP_R3, 1, v.arLLE = true; v.oeSel = ALU; v.leSel = SPL);
	WRITE_CONDITIONAL_VALUE(POP_R3, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.arHLE = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R3, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R3, 3, v.flagSel = FLAG1; v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R3, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R3, 3, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(POP_R3, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R3);

	WRITE_VALUE(POP_R4, 0, v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(POP_R4, 1, v.arLLE = true; v.oeSel = ALU; v.leSel = SPL);
	WRITE_CONDITIONAL_VALUE(POP_R4, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.arHLE = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R4, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R4, 3, v.flagSel = FLAG1; v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R4, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R4, 3, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(POP_R4, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R4);

	WRITE_VALUE(POP_R5, 0, v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(POP_R5, 1, v.arLLE = true; v.oeSel = ALU; v.leSel = SPL);
	WRITE_CONDITIONAL_VALUE(POP_R5, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.arHLE = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R5, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R5, 3, v.flagSel = FLAG1; v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R5, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R5, 3, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(POP_R5, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R5);

	WRITE_VALUE(POP_R6, 0, v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(POP_R6, 1, v.arLLE = true; v.oeSel = ALU; v.leSel = SPL);
	WRITE_CONDITIONAL_VALUE(POP_R6, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.arHLE = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R6, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R6, 3, v.flagSel = FLAG1; v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R6, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R6, 3, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(POP_R6, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R6);

	WRITE_VALUE(POP_R7, 0, v.oeSel = SPL; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU; v.flag1LE = false; v.pcCountEnable = true; v.memSel = PC; v.memOE = true; v.prefetchLE = true);
	WRITE_VALUE(POP_R7, 1, v.arLLE = true; v.oeSel = ALU; v.leSel = SPL);
	WRITE_CONDITIONAL_VALUE(POP_R7, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.arHLE = true, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R7, 2, v.flagSel = FLAG1; v.oeSel = SPH; v.aluOperation = ADD; v.aluSel = RIGHT; v.busSel = IMMEDIATE; v.immediate = 1; v.leSel = ALU, 1, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R7, 3, v.flagSel = FLAG1; v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R7, 0, -1, -1);
	WRITE_CONDITIONAL_VALUE(POP_R7, 3, v.flagSel = FLAG1; v.oeSel = ALU; v.arHLE = true, 1, -1, -1);
	WRITE_VALUE(POP_R7, 4, v.newInstructionLE = false; v.instructionCounterReset = true; v.memOE = true; v.memSel = ADDRESS; v.leSel = R7);


	WRITE_VALUE(LOAD_RA_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R0);
	WRITE_VALUE(LOAD_RA_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R1);
	WRITE_VALUE(LOAD_RA_LITERAL, 2, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(LOAD_RB_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R2);
	WRITE_VALUE(LOAD_RB_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R3);
	WRITE_VALUE(LOAD_RB_LITERAL, 2, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(LOAD_RC_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R4);
	WRITE_VALUE(LOAD_RC_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R5);
	WRITE_VALUE(LOAD_RC_LITERAL, 2, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(LOAD_RD_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R6);
	WRITE_VALUE(LOAD_RD_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R7);
	WRITE_VALUE(LOAD_RD_LITERAL, 2, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);


	WRITE_VALUE(LOAD_R0_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R0);
	WRITE_VALUE(LOAD_R0_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(LOAD_R1_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R1);
	WRITE_VALUE(LOAD_R1_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(LOAD_R2_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R2);
	WRITE_VALUE(LOAD_R2_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(LOAD_R3_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R3);
	WRITE_VALUE(LOAD_R3_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(LOAD_R4_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R4);
	WRITE_VALUE(LOAD_R4_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(LOAD_R5_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R5);
	WRITE_VALUE(LOAD_R5_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(LOAD_R6_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R6);
	WRITE_VALUE(LOAD_R6_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);
	WRITE_VALUE(LOAD_R7_LITERAL, 0, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.leSel = R7);
	WRITE_VALUE(LOAD_R7_LITERAL, 1, v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);


	WRITE_VALUE(SET_CARRY, 0, v.leSel = ALU; v.aluOperation = INVERT; v.aluSel = RIGHT; v.busSel = FLAG_IMMEDIATE);
	WRITE_VALUE(SET_CARRY, 1, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SET_CARRY, 2, v.oeSel = FLAGS; v.leSel = ALU; v.aluOperation = OR; v.busSel = TEMP);
	WRITE_VALUE(SET_CARRY, 3, v.oeSel = ALU; v.leSel = FLAGS; v.flag2LE = false; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(CLEAR_CARRY, 0, v.oeSel = FLAGS; v.leSel = ALU; v.aluOperation = AND; v.busSel = FLAG_IMMEDIATE);
	WRITE_VALUE(CLEAR_CARRY, 1, v.oeSel = ALU; v.leSel = FLAGS; v.flag2LE = false; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(SET_INTERRUPT, 0, v.leSel = ALU; v.aluOperation = INVERT; v.aluSel = RIGHT; v.busSel = FLAG_IMMEDIATE);
	WRITE_VALUE(SET_INTERRUPT, 1, v.oeSel = ALU; v.ceTempLE = true);
	WRITE_VALUE(SET_INTERRUPT, 2, v.oeSel = FLAGS; v.leSel = ALU; v.aluOperation = OR; v.busSel = TEMP);
	WRITE_VALUE(SET_INTERRUPT, 3, v.oeSel = ALU; v.leSel = FLAGS; v.flag2LE = false);
	WRITE_VALUE(SET_INTERRUPT, 4, v.flagSel = FLAG2; v.flag1LE = false; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);

	WRITE_VALUE(CLEAR_INTERRUPT, 0, v.oeSel = FLAGS; v.leSel = ALU; v.aluOperation = AND; v.busSel = FLAG_IMMEDIATE);
	WRITE_VALUE(CLEAR_INTERRUPT, 1, v.oeSel = ALU; v.leSel = FLAGS; v.flag2LE = false);
	WRITE_VALUE(CLEAR_INTERRUPT, 2, v.flagSel = FLAG2; v.flag1LE = false; v.pcCountEnable = true; v.memOE = true; v.memSel = PC; v.prefetchLE = true; v.newInstructionLE = false; v.instructionCounterReset = true);


	for(int i = 0; i < 4; i ++){

		std::ofstream file("out" + std::to_string(i) + ".bin", ios::out | ios::binary | ios::trunc);

		for(int j = 0; j < 0x8000; j ++){

			file.write((char*)(&code[j]) + i, 1);
		}

		file.close();
	}

	std::cout << sizeof(Value) << "\n";
}
