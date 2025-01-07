// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      udg.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      6th January 2025
//      Reviewed :  No
//      Purpose :   Test UDG on Graphic fonts
//
// ***************************************************************************************
// ***************************************************************************************

#include "common.h"
#include <libraries.h>

// ***************************************************************************************
//
//                      Start and run the CPU. Does not have to return.
//
// ***************************************************************************************

void ApplicationRun(void) {
	uint8_t gr1[8] = {0xFF,0x81,0x81,0x81,0x8F,0x88,0x88,0xF8 };
	uint8_t gr2[8] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55 };
	char buffer[32];
	sprintf(buffer,"Chars (%c%c)",224,225);;
//	GFXSetMode(DVI_MODE_640_240_8);
//	GFXSetMode(DVI_MODE_640_480_2);
//	GFXSetMode(DVI_MODE_320_240_8);
	GFXSetMode(DVI_MODE_320_240_64);
	CONEnableConsole(true);
	CONWrite(12);
	CONDefineUDG(224, gr1);
	CONDefineUDG(225,gr2);
	CONWriteString(buffer);
	CONEnableConsole(false);
	GFXDrawString(NULL,210,100,buffer,FONT_SYSTEM_8,6,1);
	GFXDrawString(NULL,210,130,buffer,FONT_SCRIPT_12,5,1);


	GFXFillRect(NULL,10,10,200,90,1);

    while (1) {
  
        if (KBDEscapePressed(true)) {                                               // Escaped ?
            CONWriteString("Escape !\r");
			GFXFillRect(NULL,100,40,250,120,0x103);
        }

	
        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat
        }
    }
}

