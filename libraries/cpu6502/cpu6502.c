// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      cpu6502.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      5th January 2025
//      Reviewed :  No
//      Purpose :   6502 Emulation Main Code
//
// ***************************************************************************************
// ***************************************************************************************

#include "common.h"

static uint8_t a,x,y,s;																// 6502 A,X,Y and Stack registers
static uint8_t carryFlag,interruptDisableFlag,breakFlag,							// Values representing status reg
			 decimalFlag,overflowFlag,sValue,zValue;
static uint8_t pc;																	// Program Counter.
static uint32_t cycles;																// Cycle Count.

#define READ8(a)		(0)
#define READ16(a) 		(READ8(a) + ((READ8(a+1)) << 8))
#define WRITE8(a,d)  	{}
#define FETCH8() 		READ8(pc++)
#define FETCH16()   	{ temp16 = 0; }

#define CYCLES(x) 		cycles += (x)

#include "generator/__6502support.h"

int CPU6502ExecuteOne(void) {
	switch(FETCH8()) {
		#include "generator/__6502opcodes.h"
	}
	return 0;
}