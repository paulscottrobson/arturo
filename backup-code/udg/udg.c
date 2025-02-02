/**
 * @file       udg.c
 *
 * @brief      Test UDG on Graphic fonts, also checks extents and backgrounds
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include <libraries.h>


/**
 * @brief      Run the main application
 */
void ApplicationRun(void) {
    uint8_t gr1[8] = {0xFF,0x81,0x81,0x81,0x8F,0x88,0x88,0xF8 };                    // Two 8x8 UDGs
    uint8_t gr2[8] = {0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55 };
    char buffer[32];                                                                // Create a string
    sprintf(buffer,"Chars (%c%c)",224,225);;

//  GFXSetMode(DVI_MODE_640_480_8);                                                 // Pick a mode
    GFXSetMode(DVI_MODE_640_240_8);
//  GFXSetMode(DVI_MODE_640_480_2);
//  GFXSetMode(DVI_MODE_320_240_8);
//  GFXSetMode(DVI_MODE_320_240_64);

    CONEnableConsole(true);                                                         // Enable the console, clear screen, define udgs
    CONWrite(12);
    CONDefineUDG(224,gr1);
    CONDefineUDG(225,gr2);
    CONWriteString(buffer);                                                         // Write buffer out, and debug console out.
    CONEnableConsole(false);

    int w,y1,y2;
    GFXDrawString(NULL,210,100,buffer,FONT_SYSTEM_8,6,1);                           // Print using FONT_SYSTEM_8 (the console debugging font)
    GFXGetStringExtent(buffer,FONT_SYSTEM_8,1,&w,&y1,&y2);;                         // Get the spread of it how wide it is, how far it goes above/below the centre line
    GFXFrameRect(NULL,210,100+y1,210+w-1,100+y2-1,7);                               // Draw a oc around it


    GFXDrawString(NULL,210,130,buffer,FONT_SCRIPT_12,0x100403,1);                   // Draw font in XOR draw mode

    while (1) {
  
        if (KBDEscapePressed(true)) {                                               // Escaped ?
            CONWriteString("Escape !\r");
            GFXFillRect(NULL,100,40,250,120,0x10003);                               // Draw a rect in XOR mode, which is clearly visible on pressing ESC/Shift-Esc
        }
    
        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat
        }
    }
}

