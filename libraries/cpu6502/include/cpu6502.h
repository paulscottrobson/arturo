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
//  	Other prototypes
//
void CPU6502Setup(CPU6502READFUNC read,CPU6502WRITEFUNC write,int fRate,int clock);
bool CPUReset(void);
int CPU6502ExecuteOne(void);
bool CPUTriggerNMI(void);
bool CPUTriggerIRQ(void);

