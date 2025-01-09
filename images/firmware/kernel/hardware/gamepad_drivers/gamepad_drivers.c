/**
 * @file   gamepad_drivers.c
 *
 * @brief      Gamepad individual handlers.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


//
//      Name :      gamepad_drivers.c
//      Authors :   Paul Robson (paul@robsons.org.uk), based on work by Sascha Schneider
//      Date :      20th December 2024
//      Reviewed :  No
//      Purpose :   Gamepad individual handlers.
//



#include "common.h"



//
//                  Dispatch command and data to the correct handler.
//


int  CTLDispatchMessage(int command,CTLState *cs,struct _CTL_MessageData *msg) {
    int retVal = 0;
    //
    //      Dispatch on hardware type.
    //
    switch(cs->_hardwareTypeID) {
        //
        //      SNES Compatible keypads. 0814:E401 is the Olimex one. 0810:E501 is a "Personal Comms System Inc" gamepad.
        //
        case CTL_DEVICE_TYPE_ID(0x081F,0xE401):
        case CTL_DEVICE_TYPE_ID(0x0810,0xE501):
            retVal = CTLDriverSNESType(command,cs,msg);
            break;
        //
        //      Unknown Device
        //
        default:
            CTLAnnounceDevice(cs,"Unknown hardware device");
            break;
    }
    return retVal;
}


//
//                      Utility function to display the status.
//


void CTLAnnounceDevice(CTLState *cs,const char *name) {
    CONWriteString("USB Device: [%04x:%04x] %s\r",cs->_hardwareTypeID >> 16,cs->_hardwareTypeID & 0xFFFF,name);
}
