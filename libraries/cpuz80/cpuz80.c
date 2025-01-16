/**
 * @file       cpuz80.c
 *
 * @brief      Z80 Emulation Main Code
 *
 * @author     Paul Robson
 *
 * @date       26/01/2025
 *
 */

#include "common.h"
#include "libraries.h"

//
//                                      CPU Status
//
static uint8_t A,B,C,D,E,H,L;                                                         // Standard register
static uint16_t AFalt,BCalt,DEalt,HLalt;                                              // Alternate data set.
static uint16_t PC,SP;                                                                // 16 bit registers
static uint16_t IX,IY;                                                                // IX IY accessed indirectly.

static char logBuffer[64];

static uint8_t s_Flag,z_Flag,c_Flag,h_Flag,n_Flag,p_Flag;                             // Flag Registers
static uint8_t I,R,intEnabled;                                                        // We don't really bother with these much.

static uint32_t temp32;
static uint16_t temp16,temp16a,*pIXY;
static uint8_t temp8,oldCarry;

static int cycles = 512;                                                            // Cycle Count.

static int frameRate;                                                               // Frame rate per second
static int cyclesPerFrame;                                                          // CPU Cycles per frame.
static int nextFrameSync = 0;                                                       // Timer frame sync next

//
//                                  Read / Write system
//
uint8_t _dummyRead(uint16_t a) { return 0; }                                        // These are dummies
void    _dummyWrite(uint16_t a,uint8_t d) {}

static CPUZ80READFUNC readFunc = _dummyRead;                                       // Set the read and write functions to the dummies
static CPUZ80WRITEFUNC writeFunc = _dummyWrite;
static CPUZ80READFUNC readPortFunc = _dummyRead;
static CPUZ80WRITEFUNC writePortFunc = _dummyWrite;

#define READ8(a)    ((*readFunc)(a))                                                // Basic Read
#define WRITE8(a,d) ((*writeFunc)(a,d))                                             // Basic Write

#define READ16(a)   (READ8(a) | ((READ8((a)+1) << 8)))                              // Read 16 bits.
#define WRITE16(a,d) { WRITE8(a,(d) & 0xFF);WRITE8((a)+1,(d) >> 8); }               // Write 16 bits

#define FETCH8()    READ8(PC++)                                                     // Fetch byte without fiddling.
#define FETCH16()   _Fetch16()                                                      // Fetch word

#define INPORT(p)   ((*readPortFunc)(a))
#define OUTPORT(p,d) ((*writePortFunc)(a,d))

#define CYCLES(n) cycles -= (n)

static uint16_t _Fetch16(void) {
    uint16_t w = READ16(PC);
    PC += 2;
    return w;
}

//
//                                          CPU Support routines.
//
#include "cpu_support.h"


/**
 * @brief      Set up the Z80
 *
 * @param      setup  Setup informaion
 */
void CPUZ80Setup(CPUZ80SETUP *setup) {
    readFunc = setup->read;writeFunc = setup->write;                                // Save the function pointers
    readPortFunc = setup->readPort;writePortFunc = setup->writePort;
    frameRate = setup->frameRate;                                                   // Save Frame rate (in Hz)
    cyclesPerFrame = setup->clockSpeed/setup->frameRate;                            // Cycles in each frame.
}

/**
 * @brief      Get the Z80 CPU Status
 *
 * @param      stat Pointer to the structure holding the status.
 */
void CPUZ80GetStatus(CPUZ80STATUS *stat) {
}

/**
 * @brief      Execute one Z80 instruction
 *
 * @return     Non-zero if the frame sync/redraw fired.
 */
int CPU6502ExecuteOne(void) {

    BYTE8 opcode = FETCH8();                                                        // Fetch opcode.
    switch(opcode) {                                                                // Execute it.
        #include "_code_group_0.h"
    }

    if (cycles < cyclesPerFrame) return 0;                                          // No frame, yet.
    cycles -= cyclesPerFrame;                                                       // Adjust the cycle counter back.
    while (TMRReadTimeMS() < nextFrameSync) {}                                      // Wait till frame time elapsed
    nextFrameSync = TMRReadTimeMS() + 1000 / frameRate;
    return 1;
}

// /**
//  * @brief      Trigger the NMI
//  *
//  * @return     true
//  */
// bool CPU6502TriggerNMI(void) {
//     nmiCode();
//     return true;
// }


// /**
//  * @brief      Trigger the IRQ
//  *
//  * @return     True if IRQ triggered.
//  */
// bool CPU6502TriggerIRQ(void) {
//     if (interruptDisableFlag == 0) {                                                // Fire if I flag is clear
//         executeInterrupt(0xFFFE,1);
//     }
//     return (interruptDisableFlag == 0);
// }

/**
 * @brief      Reset the Z80 emulation
 *
 * @return     True if reset has happened.
 */
bool CPUZ80Reset(void) {
    BuildParityTable();                                                             // Build the parity flag table.
    PC = 0;                                                                         // Zero PC.
    return true;
}