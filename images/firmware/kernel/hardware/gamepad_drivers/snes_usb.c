/**
 * @file       snes_usb.c
 *
 * @brief      Driver for SNES type USB gamepads (like the Olimex standard one)
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#include "common.h"

/**
 * @brief      Driver for a SNES compatible USB connected controller
 *
 * @param[in]  command  Command for driver
 * @param      cs       Controller State
 * @param      msg      Message pcket
 *
 * @return     -1 always
 */
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