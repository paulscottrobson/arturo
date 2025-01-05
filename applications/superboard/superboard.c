// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      superboard.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      5th January 2025
//      Reviewed :  No
//      Purpose :   Superboard emulator
//
// ***************************************************************************************
// ***************************************************************************************

#include "common.h"
#include <libraries.h>

uint8_t SuperRead(uint16_t a);

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

#include "roms/basic_rom.h"
#include "roms/monitor_rom.h"

#define RAMSIZE 	(8192)  														// Installed RAM size

uint8_t ram[RAMSIZE];  																// 8k of Program RAM
uint8_t videoRAM[0x400];  															// 1k of Video RAM

//
//		Handles reads
//
uint8_t SuperRead(uint16_t a) { 
	if (a < RAMSIZE) return ram[a];  												// 0000-1FFF is RAM
	if (a >= 0xD000 && a < 0xD400) return videoRAM[a-0xD000];						// D000-D3FF is the video RAM
	if (a >= 0xDF00 && a < 0xE000) return 0xFF;  									// DF00-DFFF is the keyboard
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
		videoRAM[a-0xD000] = d;
		printf("Screen %d %d %c\n",a-0xD000,d,d);
	}
	if (st.pc >= 0xF800) printf("Write %x %x\n",a,d);
}

// ***************************************************************************************
//
//                      Start and run the CPU. Does not have to return.
//
// ***************************************************************************************

static GFXPort vp;

void ApplicationRun(void) {
    GFXSetMode(DVI_MODE_320_240_8);   												// Display to 320x240x8 mode.

    CPU6502SETUP s;    																// Set up the processor r/w
    s.read = SuperRead;s.write = SuperWrite;
    s.clockSpeed = 1000000;  														// 1Mhz Clock
    s.frameRate = 60;  																// 60 Frames/Second	
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

