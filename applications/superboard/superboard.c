/**
 * @file       superboard.c
 *
 * @brief      Main code for a Superboard 2 emulator, demoing the 6502 library
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include <libraries.h>

uint8_t SuperRead(uint16_t a);
void SuperWrite(uint16_t a,uint8_t d);
uint8_t ReadKeyboard(void);

static CPU6502STATUS st;

/**
 * @brief      Dump the current 6502 status to stdout
 */

void Dump(void) {
    CPU6502GetStatus(&st);
    printf("%04x : %02x A:%02x X:%02x Y:%02x S:%02x\n",st.pc,SuperRead(st.pc),st.a,st.x,st.y,st.stackPointer);
}

//
//              Handle memory read/write (character ROM is not memory mapped)
//

#include "roms/basic_rom.h"                                                         // Load the ROMs into flash memory
#include "roms/monitor_rom.h"

#define RAMSIZE     (8192)                                                          // Installed RAM size

uint8_t ram[RAMSIZE];                                                               // 8k of Program RAM
uint8_t videoRAM[0x400];                                                            // 1k of Video RAM
uint8_t keyboardPort;                                                               // Keyboard port latch.

/**
 * @brief      Handles reads from 6502 memory which may involve side effects
 *
 * @param[in]  a     address to read in 64k space
 *
 * @return     byte read from that location
 */
uint8_t SuperRead(uint16_t a) { 
    if (a < RAMSIZE) return ram[a];                                                 // 0000-1FFF is RAM
    if (a >= 0xD000 && a < 0xD400) return videoRAM[a-0xD000];                       // D000-D3FF is the video RAM
    if (a >= 0xDF00 && a < 0xE000) return ReadKeyboard();                           // DF00-DFFF is the keyboard
    if (a >= 0xA000 && a < 0xC000) return basic_rom[a-0xA000];                      // A000-BFFF is the BASIC ROM
    if (a >= 0xF800) return monitor_rom[a-0xF800];                                  // F800-FFFF is the Monitor ROM
    return 0; 
}

/**
 * @brief      Handles writes to 6502 memory which may update memory or hardwar
 *
 * @param[in]  a     address to write to in 64k space
 * @param[in]  d     byte to write to that location
 */
void SuperWrite(uint16_t a,uint8_t d) {
    if (a < RAMSIZE) ram[a] = d;                                                    // 0000-1FFF is RAM
    if (a >= 0xD000 && a < 0xD400) {                                                // D000-D3FF is video RAM
        videoRAM[a-0xD000] = d;                                                     // Update and repaint.
        ScreenPaint(a,d); 
    }
    if (a >= 0xDF00 && a < 0xDFFF) {                                                // DF00-DFFF is the keyboard port.
        keyboardPort = d;
    }
}

//
//      This is the array of keyboard keys to the row/columns on the Superboard II
//
const int keyboardMap[] = {
    KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,0,
    KEY_8,KEY_9,KEY_0,KEY_APOSTROPHE,KEY_MINUS, KEY_BACKSPACE, 0,  0,
    KEY_DOT,KEY_L,KEY_O, 0, KEY_ENTER, 0,  0,  0,
    KEY_W,KEY_E,KEY_R,KEY_T,KEY_Y,KEY_U,KEY_I,0,
    KEY_S,KEY_D,KEY_F,KEY_G,KEY_H,KEY_J,KEY_K,0,
    KEY_X,KEY_C,KEY_V,KEY_B,KEY_N,KEY_M,KEY_COMMA,0,
    KEY_Q,KEY_A,KEY_Z,KEY_SPACE,KEY_SLASH,KEY_SEMICOLON,KEY_P,0,
    KEY_TAB,-KEY_MOD_LCTRL,KEY_ESC,0,0,-KEY_MOD_LSHIFT,-KEY_MOD_RSHIFT
};


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
    uint8_t output = 0xFF;                                                          // We calculate it active low

    for (int row = 0;row < 8;row ++) {                                              // Do each row
        if ((keyboardPort & (0x80 >> row)) == 0) {                                  // If the port bit is clear, check the row.
            for (int col = 0;col < 8;col++) {
                if (IsKeyPressed(keyboardMap[row * 8 + col])) {                     // Key pressed ?
                    output ^= (0x80 >> col);                                        // Toggle the column bit
                }
            }
        }
    }
    return output;
}


#include "roms/character_rom.h"                                                     // Binary character graphics, these are *not* on the memory map.

/**
 * @brief      Update the display screen, which has had a character written to
 *             it
 *
 * @param[in]  addr  The address of the screen location written to
 * @param[in]  c     Character written to it
 */
void ScreenPaint(uint16_t addr,uint8_t c) {
    if (addr < 0xD085 || addr > 0xD37C) return;                                     // Outside the 24x24 screen..
    addr = addr - 0xD085;                                                           // Offset into VRAM, $D085 is the top left.
    int x = addr % 32,y = addr / 32;                                                // Cell position.
    if (x >= 24) return;                                                            // Off the display
    int offset = x + y * 320;                                                       // Offset into the bitmap
    offset += 12*40 + 8;                                                            // Centre on the display 

    const uint8_t *fontData = character_rom + c * 8;                                // Character data to copy
    struct DVIModeInformation *dmi = DVIGetModeInformation();                       // Access bitmap planes

    for (int i = 0;i < 8;i++) {                                                     // Write to bitmap
        uint8_t bd = *fontData++,bdr = 0;                                           // Get the character line and reverse it.
        for (int i = 0;i < 8;i++) {
            if (bd & (1 << i)) bdr |= 0x80 >> i;
        }
        *(dmi->bitPlane[0]+offset) = bdr;                                           // Write to RGB bitmaps
        *(dmi->bitPlane[1]+offset) = bdr;
        *(dmi->bitPlane[2]+offset) = bdr;
        offset += 320/8;                                                            // One line down
    }
}


/**
 * @brief      Run the main application
 */
void ApplicationRun(void) {
    CONEnableConsole(true);                                                         // Disable console output.
    GFXSetMode(DVI_MODE_320_240_8);                                                 // Display to 320x240x8 mode.
    CPU6502SETUP s;                                                                 // Set up the processor r/w
    s.read = SuperRead;s.write = SuperWrite;
    s.clockSpeed = 1000000;                                                         // 1Mhz Clock
    s.frameRate = 50;                                                               // 50 Frames/Second 
    CPU6502Setup(&s);                                                               // Set up the processor
    CPU6502Reset();                                                                 // Reset the CPU
    while (1) {
        while (CPU6502ExecuteOne() == 0) {}                                         // Do one frame
        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat        
        }
    }
}
