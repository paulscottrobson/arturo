/**
 * @file 
 *
 * @brief      
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


//
//      Name :      logo.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      26th December 2024
//      Reviewed :  No
//      Purpose :   Draw Logo
//



#include "common.h"
#include "support/__logo_data.h"
#include "support/__config.h"


//
//                              Draw logos on boot screen
//


void LOGODrawTitle(void) {
    LOGODraw(DVIGetModeInformation()->width-LOGO_WIDTH_BYTES*8-4,4,LOGO_WIDTH_BYTES,LOGO_HEIGHT,logoData,1,0);
}


//
//                                  Draw a specific logo
//


void LOGODraw(int x,int y,int w,int h,const uint8_t *pixelData,int fgr,int bgr) {
    for (int yc = 0;yc < h;yc++) {
        for (int xb = 0;xb < w;xb++) {
            for (int bit = 0;bit < 8;bit++) {
                CONDrawPixel(x+xb*8+bit,y+yc,(*pixelData & (0x80 >> bit)) ? fgr:bgr);
            }
            pixelData++;
        }
    }
}


//
//                                  Display text header
//


void LOGOTextHeader(void) {
    CONSetColour(CON_COL_CYAN,CON_COL_BLACK);
    CONWriteString("*** Arturo Kernel ***\r\r");
    CONSetColour(CON_COL_YELLOW,CON_COL_BLACK);
    CONWriteString("Hardware by Olimex\r");
    CONWriteString("Software by Paul Robson,Lennart Benschop 2024/2025\r(MIT License)\r");
    CONWriteString("Version 0.1.1 (28-12-24)\r");
    CONWriteString("%s version\r",PICO_PLATFORM_NAME);
}