/**
 * @file       gamepad_drivers.c
 *
 * @brief      Gamepad individual handlers.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"

/**
 * @brief      Dispatch commands to the appropriate driver.
 *
 *             This works like a little object system, though there is really
 *             only onr message type
 *
 * @param[in]  command  The command to be sent
 * @param      cs       The Controller state
 * @param      msg      The <essage data
 *
 * @return     Whatever is returned from the driver.
 */
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

/**
 * @brief      Display information about a device
 *
 * @param      cs    State of controller
 * @param[in]  name  Name of controller
 */
void CTLAnnounceDevice(CTLState *cs,const char *name) {
    CONWriteString("USB Device: [%04x:%04x] %s\r\n",cs->_hardwareTypeID >> 16,cs->_hardwareTypeID & 0xFFFF,name);
}
