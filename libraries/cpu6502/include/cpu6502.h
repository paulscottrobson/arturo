/**
 * @file cpu6502.h
 *
 * @brief      Header file, library 6502 emulator
 *             
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */



#pragma once

//
//      Function pointer types for reading/writing on 6502
//
typedef uint8_t (*CPU6502READFUNC)(uint16_t);
typedef void (*CPU6502WRITEFUNC)(uint16_t,uint8_t);
//
//      Configuration structure.
//
typedef struct _CPU6502Setup {
    CPU6502READFUNC read;
    CPU6502WRITEFUNC write;
    int frameRate;
    int clockSpeed;
} CPU6502SETUP;
//
//      Status structure
//
typedef struct _CPU6502Status {
    int a,x,y,stackPointer,status,pc;
} CPU6502STATUS;

//
//      Other prototypes
//
void CPU6502Setup(CPU6502SETUP *setup);
void CPU6502GetStatus(CPU6502STATUS *stat);
bool CPU6502Reset(void);
int CPU6502ExecuteOne(void);
bool CPU6502TriggerNMI(void);
bool CPU6502TriggerIRQ(void);

