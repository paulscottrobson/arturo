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

	char buffer[32];
	sprintf(buffer,"Chars (%c%c)",224,225);;
	GFXSetMode(DVI_MODE_640_240_8);
	GFXDrawString(NULL,10,20,buffer,FONT_SYSTEM_8,6,1);

    while (1) {
  
        if (KBDEscapePressed(true)) {                                               // Escaped ?
            CONWriteString("Escape !\r");
        }

	
        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat
        }
    }
}

