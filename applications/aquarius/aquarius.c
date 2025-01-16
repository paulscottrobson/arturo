/**
 * @file       aquarius.c
 *
 * @brief      Main code for a Mattel Aquarius emulator, demoing the Z80 library
 *
 * @author     Paul Robson
 *
 * @date       16/01/2025
 *
 */

#include "common.h"
#include <libraries.h>

uint8_t SuperRead(uint16_t a);
void SuperWrite(uint16_t a,uint8_t d);
uint8_t SuperPortRead(uint16_t a);
void SuperPortWrite(uint16_t a,uint8_t d);
uint8_t ReadKeyboard(void);
void ScreenPaint(uint16_t addr);

/**
 * @brief      Dump the current 6502 status to stdout
 */

void Dump(void) {
    static CPUZ80STATUS st;
    CPUZ80GetStatus(&st);
    printf("%04x:%02x A:%02x B:%02x C:%02x D:%02x E:%02x H:%02x L:%02x:\n",st.pc,SuperRead(st.pc),st.a,st.b,st.c,st.d,st.e,st.h,st.l);;
}

//
//              Binaries 
//
#include "roms/character_rom.h"                                                     // Load the ROMs into flash memory
#include "roms/monitor_rom.h"

//
//              Handle memory read/write (character ROM is not memory mapped)
//
 
#define RAMSIZE     (16384)                                                         // Extended RAM size

uint8_t ram[RAMSIZE+4096];                                                          // 8k of Program RAM

/**
 * @brief      Handles reads from 6502 memory which may involve side effects
 *
 * @param[in]  a     address to read in 64k space
 *
 * @return     byte read from that location
 */
uint8_t SuperRead(uint16_t a) { 
    if (a < 0x2000) return monitor_ROM[a];                                          // 0000-1FFF is ROM
    if (a > 0x3000 && a < RAMSIZE+0x4000) return ram[a-0x3000];                     // RAM area ?
    return 0xFF; 
}

/**
 * @brief      Handles writes to 6502 memory which may update memory or hardwar
 *
 * @param[in]  a     address to write to in 64k space
 * @param[in]  d     byte to write to that location
 */
void SuperWrite(uint16_t a,uint8_t d) {
    if (a >= 0x3000 && a < RAMSIZE+0x4000) {                                        // RAM area ?
        bool changed = (ram[a-0x3000] != d);
        ram[a - 0x3000] = d;
        if (a < 0x3800 && changed) {                                                // Written to screen RAM (char/colour RAM)
            ScreenPaint(a);
        }
    }
}

/**
 * @brief      Handle reads from Z80 I/O ports
 *
 * @param[in]  a     Port to read
 *
 * @return     Value read
 */
uint8_t SuperPortRead(uint16_t a) {
    return 0;
}

/**
 * @brief      Write value to Z80 I/O port
 *
 * @param[in]  a     Port to write to
 * @param[in]  d     Data to write
 */
void SuperPortWrite(uint16_t a,uint8_t d) {

}


/**
 * @brief      Determines whether the specified key pressed.
 *
 * @param[in]  c     key to check. +ve is a KEY_x value -ve is a KEY_MOD_ value,
 *                   0 is no check
 *
 * @return     True if the specified key pressed, False otherwise.
 */
bool IsKeyPressed(int c) {
    if (c == 0) return 0;                                                           // No key
    if (c < 0) return (KBDGetModifiers() & (-c)) != 0;                              // -ve is a modifier key (shift, control, alt etc.)
    return KBDGetStateArray()[c] != 0;                                              // +ve is a keyboard key
}

/**
 * @brief      Read the keyboard depending on what value has been written to the
 *             keyboard latch, all these are active low.
 *
 * @return     Byte constructed from whichever keys are pressed, active low.
 */
uint8_t ReadKeyboard(void) {
    return 0;
}


/**
 * @brief      Update the display screen, which has had a character written to
 *             it
 *
 * @param[in]  addr  The address of the screen location written to
 * @param[in]  c     Character written to it
 */
void ScreenPaint(uint16_t addr) {
    if (addr >= 0x3800) return;
    addr = addr & 0x3FF;        
    int c = ram[addr];                                                              // Character
    int x = addr % 40,y = addr / 40;                                                // Cell position.
    if (y >= 24) return;                                                            // Off the display
    int offset = x + y * 320;                                                       // Offset into the bitmap

    const uint8_t *fontData = character_ROM + c * 8;                                // Character data to copy
    struct DVIModeInformation *dmi = DVIGetModeInformation();                       // Access bitmap planes

    for (int i = 0;i < 8;i++) {                                                     // Write to bitmap
        uint8_t bd = *fontData++;                                                   // Get the character line.
        *(dmi->bitPlane[0]+offset) = bd;                                            // Write to RGB bitmaps
        *(dmi->bitPlane[1]+offset) = bd;
        *(dmi->bitPlane[2]+offset) = bd;
        offset += 320/8;                                                            // One line down
    }
}

/**
 * @brief      Run the main application
 */
void ApplicationRun(void) {
    CONEnableConsole(true);                                                         // Disable console output.
    GFXSetMode(DVI_MODE_320_240_8);                                                 // Display to 320x240x8 mode.
    CPUZ80SETUP s;                                                                  // Set up the processor r/w
    s.read = SuperRead;s.write = SuperWrite;
    s.readPort = SuperPortRead;s.writePort = SuperPortWrite;
    s.clockSpeed = 3580000;                                                         // 3.58Mhz Clock
    s.frameRate = 50;                                                               // 50 Frames/Second 
    CPUZ80Setup(&s);                                                                // Set up the processor
    CPUZ80Reset();                                                                  // Reset the CPU
    while (1) {
        while (CPUZ80ExecuteOne() == 0) {                                           // Do one frame
        }
        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat        
        }
    }
}
