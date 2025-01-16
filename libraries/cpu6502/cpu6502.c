/**
 * @file       cpu6502.c
 *
 * @brief      6502 Emulation Main Code
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include "libraries.h"


//
//                                      CPU Status
//

static uint8_t a,x,y,sp;                                                            // 6502 A,X,Y and Stack registers
static uint8_t carryFlag,interruptDisableFlag,breakFlag,                            // Values representing status reg
             decimalFlag,overflowFlag,sValue,zValue;
static uint16_t pc;                                                                 // Program Counter.
static int cycles;                                                                  // Cycle Count.

static int frameRate;                                                               // Frame rate per second
static int cyclesPerFrame;                                                          // CPU Cycles per frame.
static int nextFrameSync = 0;                                                       // Timer frame sync next

//
//                                  Read / Write system
//
uint8_t _dummyRead(uint16_t a) { return 0; }                                        // These are dummies
void    _dummyWrite(uint16_t a,uint8_t d) {}

static CPU6502READFUNC readFunc = _dummyRead;                                       // Set the read and write functions to the dummies
static CPU6502WRITEFUNC writeFunc = _dummyWrite;

#define READ8(a)        ((*readFunc)(a))
#define WRITE8(a,d)     ((*writeFunc)(a,d))
#define READ16(a)       (READ8(a) + ((READ8(a+1)) << 8))
#define FETCH8()        READ8(pc++)
#define FETCH16()       { temp16 = FETCH8();temp16 |= (FETCH8() << 8); }

#define CYCLES(n) cycles += (n)

#include "generator/__6502support.h"


/**
 * @brief      Set up the 65C02
 *
 * @param      setup  Setup informaion
 */
void CPU6502Setup(CPU6502SETUP *setup) {
    readFunc = setup->read;writeFunc = setup->write;                                // Save the function pointer
    frameRate = setup->frameRate;                                                   // Save Frame rate (in Hz)
    cyclesPerFrame = setup->clockSpeed/setup->frameRate;                            // Cycles in each frame.
}

/**
 * @brief      Get the 65C02 CPU Status
 *
 * @param      stat Pointer to the structure holding the status.
 */
void CPU6502GetStatus(CPU6502STATUS *stat) {
    stat->a = a;stat->x = x;stat->y = y;
    stat->pc = pc;stat->stackPointer = sp;
    stat->status = constructFlagRegister();
}

/**
 * @brief      Execute one 6502 instruction
 *
 * @return     Non-zero if the frame sync/redraw fired.
 */
int CPU6502ExecuteOne(void) {

    switch(FETCH8()) {                                                              // Execute one 6502 opcode
        #include "generator/__6502opcodes.h"
    }
    if (cycles < cyclesPerFrame) return 0;                                          // No frame, yet.
    cycles -= cyclesPerFrame;                                                       // Adjust the cycle counter back.
    while (TMRReadTimeMS() < nextFrameSync) {}                                      // Wait till frame time elapsed
    nextFrameSync = TMRReadTimeMS() + 1000 / frameRate;
    return 1;
}

/**
 * @brief      Trigger the NMI
 *
 * @return     true
 */
bool CPU6502TriggerNMI(void) {
    nmiCode();
    return true;
}


/**
 * @brief      Trigger the IRQ
 *
 * @return     True if IRQ triggered.
 */
bool CPU6502TriggerIRQ(void) {
    if (interruptDisableFlag == 0) {                                                // Fire if I flag is clear
        executeInterrupt(0xFFFE,1);
    }
    return (interruptDisableFlag == 0);
}

/**
 * @brief      Reset the 6502 emulation
 *
 * @return     True if reset has happened.
 */
bool CPU6502Reset(void) {
    resetProcessor();
    return true;
}