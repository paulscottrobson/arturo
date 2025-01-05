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

// ***************************************************************************************
//	
//										CPU Status
//
// ***************************************************************************************

static uint8_t a,x,y,sp;															// 6502 A,X,Y and Stack registers
static uint8_t carryFlag,interruptDisableFlag,breakFlag,							// Values representing status reg
			 decimalFlag,overflowFlag,sValue,zValue;
static uint16_t pc;																	// Program Counter.
static int cycles;																	// Cycle Count.

static int frameRate;   															// Frame rate per second
static int cyclesPerFrame;  														// CPU Cycles per frame.

#define READ8(a)		(0)
#define WRITE8(a,d)  	{}

//
//			The extended functions, which have default implementation. Only R/W are required.
//
#ifndef READ16
#define READ16(a) 		(READ8(a) + ((READ8(a+1)) << 8))
#endif 

#ifndef FETCH8
#define FETCH8() 		READ8(pc++)
#endif

#ifndef FETCH16
#define FETCH16()   	{ temp16 = FETCH8();temp16 |= (FETCH8() << 8); }
#endif


#define CYCLES(x) 		cycles += (x)

#include "generator/__6502support.h"

// ***************************************************************************************
//
//				Execute a single instruction. Returns true on frame completed
//
// ***************************************************************************************

int CPU6502ExecuteOne(void) {
	switch(FETCH8()) {  															// Execute one 6502 opcode
		#include "generator/__6502opcodes.h"
	}
	if (cycles < cyclesPerFrame) return 0;  										// No frame, yet.
	cycles -= cyclesPerFrame;   													// Adjust the cycle counter back.
	return 1;
}

// ***************************************************************************************
//
//										Fire NMI
//
// ***************************************************************************************

bool CPUTriggerNMI(void) {
	nmiCode();
	return true;
}

// ***************************************************************************************
//
//										Fire IRQ
//
// ***************************************************************************************

bool CPUTriggerIRQ(void) {
	if (interruptDisableFlag == 0) {  												// Fire if I flag is clear
		executeInterrupt(0xFFFE,1);
	}
	return (interruptDisableFlag == 0);
}

// ***************************************************************************************
//
//									  Reset the 6502
//
// ***************************************************************************************

bool CPUReset(void) {
	resetProcessor();
	return true;
}
