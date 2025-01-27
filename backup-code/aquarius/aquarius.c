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

uint8_t AquaRead(uint16_t a);
void AquaWrite(uint16_t a,uint8_t d);
uint8_t AquaPortRead(uint16_t a);
void AquaPortWrite(uint16_t a,uint8_t d);
uint8_t ReadKeyboard(int addr);
void ScreenPaint(uint16_t addr);
static uint8_t pickSelect(int colour,int bitMask);

/**
 * @brief      Dump the current 6502 status to stdout
 */

void Dump(void) {
    static CPUZ80STATUS st;
    CPUZ80GetStatus(&st);
    printf("%04x:%02x A:%02x B:%02x C:%02x D:%02x E:%02x H:%02x L:%02x:\n",st.pc,AquaRead(st.pc),st.a,st.b,st.c,st.d,st.e,st.h,st.l);;
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
uint8_t AquaRead(uint16_t a) { 
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
void AquaWrite(uint16_t a,uint8_t d) {
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
uint8_t AquaPortRead(uint16_t a) {
    if ((a & 0xFF) == 0xFF) {
        return ReadKeyboard(a);
    }
    return 0;
}

/**
 * @brief      Write value to Z80 I/O port
 *
 * @param[in]  a     Port to write to
 * @param[in]  d     Data to write
 */
void AquaPortWrite(uint16_t a,uint8_t d) {
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

static int keys[][8] = {
    { KEY_EQUAL,KEY_BACKSPACE,KEY_APOSTROPHE,KEY_ENTER,KEY_SEMICOLON,KEY_DOT,0,0 },
    { KEY_MINUS,KEY_SLASH,KEY_0,KEY_P,KEY_L,KEY_COMMA,0,0 },
    { KEY_9,KEY_O,KEY_K,KEY_M,KEY_N,KEY_J,0,0 },
    { KEY_8,KEY_I,KEY_7,KEY_U,KEY_H,KEY_B,0,0 },
    { KEY_6,KEY_Y,KEY_G,KEY_V,KEY_C,KEY_F,0,0 },
    { KEY_5,KEY_T,KEY_4,KEY_R,KEY_D,KEY_X,0,0 },
    { KEY_3,KEY_E,KEY_S,KEY_Z,KEY_SPACE,KEY_A,0,0 },
    { KEY_2,KEY_W,KEY_1,KEY_Q,-KEY_MOD_LSHIFT,-KEY_MOD_LCTRL,0,0 }
};

/**
 * @brief      Gets the bit set pattern for a specific row
 *
 * @param[in]  row   The row to check
 *
 * @return     Bit set if key is pressed in each slot.
 */
static int GetKeyboardRow(int row) {
    int word = 0;
    int p = 0;
    while (keys[row][p] != 0) {
        if (IsKeyPressed(keys[row][p])) word |= (1 << p);
        p++;
    }
    return word;
}

/**
 * @brief      Read the keyboard depending on what value has been written to the
 *             keyboard latch, all these are active low.
 *
 * @return     Byte constructed from whichever keys are pressed, active low.
 */
uint8_t ReadKeyboard(int addr) {
    uint8_t v = 0;
    for (int i = 0;i < 8;i++) {
        if ((addr & (0x0100 << i)) == 0) {
            v |= GetKeyboardRow(i);
        }
    }
    v ^= 0xFF;          
    return v;
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
    if (addr < 0x028) return;                                                       // Off the top
    int c = ram[addr];                                                              // Character
    int colour = ram[addr+0x400];
    int x = addr % 40,y = addr / 40-1;                                              // Cell position.
    if (y >= 24) return;                                                            // Off the display
    int offset = x + y * 320;                                                       // Offset into the bitmap

    const uint8_t *fontData = character_ROM + c * 8;                                // Character data to copy
    struct DVIModeInformation *dmi = DVIGetModeInformation();                       // Access bitmap planes

    static uint8_t patterns[] = { 0,0,0,0xFF };
    static uint8_t select[3];

    select[0] = pickSelect(colour,0x01);
    select[1] = pickSelect(colour,0x02);
    select[2] = pickSelect(colour,0x04);

    for (int i = 0;i < 8;i++) {                                                     // Write to bitmap
        patterns[1] = *fontData++;                                                  // Get the character line.
        patterns[2] = patterns[1] ^ 0xFF;
        *(dmi->bitPlane[0]+offset) = patterns[select[0]];                           // Write to RGB bitmaps
        *(dmi->bitPlane[1]+offset) = patterns[select[1]];
        *(dmi->bitPlane[2]+offset) = patterns[select[2]];
        offset += 320/8;                                                            // One line down
    }
}

static uint8_t FourToThree[16] = {                                                  // Map four bit colour to 3 bit colour.
    0,1,2,3,4,5,6,7,
    7,6,5,4,3,2,1,0
};

/**
 * @brief      For colour bit bitMask, workout the data to be put in this plane
 *
 * @param[in]  colour   The colour in ffff/bbbb format.
 * @param[in]  bitMask  The bit mask in the colour.
 *
 * @return     0 (clear) 1 (byte) 2 (~byte) 3 (all set)
 */

static uint8_t pickSelect(int colour,int bitMask) {
    int fgr = FourToThree[colour >> 4];
    int bgr = FourToThree[colour & 0x0F];
    if ((fgr & bitMask) == (bgr & bitMask)) {
        return (fgr & bitMask) ? 3 : 0;
    } else {
        return (fgr & bitMask) ? 1 : 2;
    }
}
/**
 * @brief      Run the main application
 */
void ApplicationRun(void) {
    CONEnableConsole(false);                                                        // Disable console output.
    GFXSetMode(DVI_MODE_320_240_8);                                                 // Display to 320x240x8 mode.
    CPUZ80SETUP s;                                                                  // Set up the processor r/w

    s.read = AquaRead;s.write = AquaWrite;
    s.readPort = AquaPortRead;s.writePort = AquaPortWrite;
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
