// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      superboard.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      5th January 2025
//      Reviewed :  Yes
//      Purpose :   Superboard emulator, typical library use.
// 					It would be helpful to have the Superboard manual for the full details
//
// ***************************************************************************************
// ***************************************************************************************

#include "common.h"
#include <libraries.h>

uint8_t SuperRead(uint16_t a);
void SuperWrite(uint16_t a,uint8_t d);
uint8_t ReadKeyboard(void);
void ScreenPaint(uint16_t addr,uint8_t c);

// ***************************************************************************************
//	
//								Dump the Processor Status
//
// ***************************************************************************************

static CPU6502STATUS st;

void Dump(void) {
	CPU6502GetStatus(&st);
	printf("%04x : %02x A:%02x X:%02x Y:%02x S:%02x\n",st.pc,SuperRead(st.pc),st.a,st.x,st.y,st.stackPointer);
}

// ***************************************************************************************
//
//				Handle memory read/write (character ROM is not memory mapped)
//
// ***************************************************************************************

#include "roms/basic_rom.h"  														// Load the ROMs into flash memory
#include "roms/monitor_rom.h"

#define RAMSIZE 	(8192)  														// Installed RAM size

uint8_t ram[RAMSIZE];  																// 8k of Program RAM
uint8_t videoRAM[0x400];  															// 1k of Video RAM
uint8_t keyboardPort;  																// Keyboard port latch.

//
//		Handles reads
//
uint8_t SuperRead(uint16_t a) { 
	if (a < RAMSIZE) return ram[a];  												// 0000-1FFF is RAM
	if (a >= 0xD000 && a < 0xD400) return videoRAM[a-0xD000];						// D000-D3FF is the video RAM
	if (a >= 0xDF00 && a < 0xE000) return ReadKeyboard(); 							// DF00-DFFF is the keyboard
	if (a >= 0xA000 && a < 0xC000) return basic_rom[a-0xA000];  					// A000-BFFF is the BASIC ROM
	if (a >= 0xF800) return monitor_rom[a-0xF800];   								// F800-FFFF is the Monitor ROM
	return 0; 
}

//
//		Handles writes
// 
void SuperWrite(uint16_t a,uint8_t d) {
	if (a < RAMSIZE) ram[a] = d;  													// 0000-1FFF is RAM
	if (a >= 0xD000 && a < 0xD400) {  												// D000-D3FF is video RAM
		videoRAM[a-0xD000] = d;  													// Update and repaint.
		ScreenPaint(a,d); 
	}
	if (a >= 0xDF00 && a < 0xDFFF) {  												// DF00-DFFF is the keyboard port.
		keyboardPort = d;
	}
}

// ***************************************************************************************
//
//                     			Read a keyboard row
//
// ***************************************************************************************

//
//		This is the array of keyboard keys to the row/columns on the Superboard II
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

//
//		Is a key pressed ? 0 is used, -ve is a modifier, +ve is a standard key
//
bool IsKeyPressed(int c) {
	if (c == 0) return 0;  															// No key
	if (c < 0) return (KBDGetModifiers() & (-c)) != 0;  							// -ve is a modifier key (shift, control, alt etc.)
	return KBDGetStateArray()[c] != 0;  											// +ve is a keyboard key
}

//
//		Read the keyboard depending on what value has been written to the keyboard latch, all these are active low.
//
uint8_t ReadKeyboard(void) {
	uint8_t output = 0xFF;   														// We calculate it active low

	for (int row = 0;row < 8;row ++) {  											// Do each row
		if ((keyboardPort & (0x80 >> row)) == 0) {  								// If the port bit is clear, check the row.
			for (int col = 0;col < 8;col++) {
				if (IsKeyPressed(keyboardMap[row * 8 + col])) {  					// Key pressed ?
					output ^= (0x80 >> col); 										// Toggle the column bit
				}
			}
		}
	}
	return output;
}

// ***************************************************************************************
//
//                     			Update the display
//
// ***************************************************************************************

#include "roms/character_rom.h"   													// Binary character graphics, these are *not* on the memory map.

void ScreenPaint(uint16_t addr,uint8_t c) {
	if (addr < 0xD085 || addr > 0xD37C) return;   									// Outside the 24x24 screen..
	addr = addr - 0xD085;  															// Offset into VRAM, $D085 is the top left.
	int x = addr % 32,y = addr / 32;  												// Cell position.
	if (x >= 24) return;  															// Off the display
	int offset = x + y * 320; 														// Offset into the bitmap
	offset += 12*40 + 8;  															// Centre on the display 

	const uint8_t *fontData = character_rom + c * 8;   								// Character data to copy
	struct DVIModeInformation *dmi = DVIGetModeInformation();  						// Access bitmap planes

	for (int i = 0;i < 8;i++) {  													// Write to bitmap
		uint8_t bd = *fontData++,bdr = 0;  											// Get the character line and reverse it.
		for (int i = 0;i < 8;i++) {
			if (bd & (1 << i)) bdr |= 0x80 >> i;
		}
		*(dmi->bitPlane[0]+offset) = bdr;  											// Write to RGB bitmaps
		*(dmi->bitPlane[1]+offset) = bdr;
		*(dmi->bitPlane[2]+offset) = bdr;
		offset += 320/8;  															// One line down
	}
}

// ***************************************************************************************
//
//                      Start and run the CPU. Does not have to return.
//
// ***************************************************************************************

void ApplicationRun(void) {
	CONEnableConsole(true);  														// Disable console output.
    GFXSetMode(DVI_MODE_320_240_8);   												// Display to 320x240x8 mode.
    CPU6502SETUP s;    																// Set up the processor r/w
    s.read = SuperRead;s.write = SuperWrite;
    s.clockSpeed = 1000000;  														// 1Mhz Clock
    s.frameRate = 50;  																// 50 Frames/Second	
    CPU6502Setup(&s);   															// Set up the processor
    CPU6502Reset();  																// Reset the CPU
    while (1) {
	    while (CPU6502ExecuteOne() == 0) {}  										// Do one frame
        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat        
        }
    }
}

