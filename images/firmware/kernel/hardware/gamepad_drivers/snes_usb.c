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
//      Name :      snes_usb.c
//      Authors :   Paul Robson (paul@robsons.org.uk), based on work by Sascha Schneider
//      Date :      20th December 2024
//      Reviewed :  No
//      Purpose :   Driver for SNES type USB gamepads (like the Olimex standard one)
//



#include "common.h"


//
//                      SNES USB PC compatible Game Controller
//


int  CTLDriverSNESType(int command,CTLState *cs,struct _CTL_MessageData *msg) {
    int retVal = 0;
    switch(command) {
        //
        //      This command announces the type and VID/PID
        //
        case CTLM_REGISTER:
            CTLAnnounceDevice(cs,"Olimex SNES style USB Controller");
            retVal = -1;
            break;
        //
        //      This takes the message data and updates the status values.
        //
        case CTLM_UPDATE:
            const uint8_t *r = msg->report;                                         // Access the USB report data
            retVal = -1;
            //
            //      D-Pad code. This is report[0] (x axis) and report[1] (y-axis). Both return $7F when centred.
            //      $00 for left/up and $ff for right/down.
            //
            cs->dx = cs->dy = 0;
            if (r[0] != 0x7F) cs->dx = (r[0] == 0) ? -1 : 1;
            if (r[1] != 0x7F) cs->dy = (r[1] == 0) ? -1 : 1;
            //
            //      Button code. This is from the upper 4 bits of report 5, which is ybax1111, active high.
            //
            cs->a = (r[5] & 0x20) != 0;
            cs->b = (r[5] & 0x40) != 0;
            cs->x = (r[5] & 0x10) != 0;
            cs->y = (r[5] & 0x80) != 0;

            break;

    }

    return retVal;
}
