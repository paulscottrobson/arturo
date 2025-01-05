// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      cpu6502.h
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      5th January 2025
//      Reviewed :  No
//      Purpose :   6502 Emulation header.
//
// ***************************************************************************************
// ***************************************************************************************

#pragma once

//
//		Function pointer types for reading/writing on 6502
//
typedef uint8_t (*CPU6502READFUNC)(uint16_t);
typedef void (*CPU6502WRITEFUNC)(uint16_t,uint8_t);
//
//		Configuration structure.
//
typedef struct _CPU6502Setup {
	CPU6502READFUNC read;
	CPU6502WRITEFUNC write;
	int frameRate;
	int clockSpeed;
} CPU6502SETUP;
//
//		Status structure
//
typedef struct _CPU6502Status {
	int a,x,y,stackPointer,status,pc;
} CPU6502STATUS;

//
//  	Other prototypes
//
void CPU6502Setup(CPU6502SETUP *setup);
void CPU6502GetStatus(CPU6502STATUS *stat);
bool CPUReset(void);
int CPU6502ExecuteOne(void);
bool CPUTriggerNMI(void);
bool CPUTriggerIRQ(void);

