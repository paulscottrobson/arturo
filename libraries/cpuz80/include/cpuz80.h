/**
 * @file       cpuz80.h
 *
 * @brief      Header file, library Z80 emulator
 *             
 * @author     Paul Robson
 *
 * @date       16/01/2025
 *
 */



#pragma once

//
//      Function pointer types for reading/writing on Z80, ports & memory
//
typedef uint8_t (*CPUZ80READFUNC)(uint16_t);
typedef void (*CPUZ80WRITEFUNC)(uint16_t,uint8_t);
//
//      Configuration structure.
//
typedef struct _CPUZ80Setup {
    CPUZ80READFUNC read;
    CPUZ80WRITEFUNC write;
    CPUZ80READFUNC readPort;
    CPUZ80WRITEFUNC writePort;
    int frameRate;
    int clockSpeed;
} CPUZ80SETUP;
//
//      Status structure
//
typedef struct _CPUZ80Status {
    int a,b,c,d,e,h,l,f,ix,iy,pc,sp,cycles;
} CPUZ80STATUS;

//
//      Other prototypes
//
void CPUZ80Setup(CPUZ80SETUP *setup);
void CPUZ80GetStatus(CPUZ80STATUS *stat);
bool CPUZ80Reset(void);
int CPUZ80ExecuteOne(void);
bool CPUZ80TriggerNMI(void);
bool CPUZ80TriggerIRQ(void);

