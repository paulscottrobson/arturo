/**
 * @file       gamepad.c
 *
 * @brief      Gamepad controller class
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#include "common.h"

//
//      Controller state data
//
#define MAX_CONTROLLERS     (4)                                                     // In practice, only one can plug in !

static int controllerCount;                                                         // Number of controllers
static CTLState controllers[MAX_CONTROLLERS];                                       // Controller states.


/**
 * @brief      Initialise the controller system
 */
void CTLInitialise(void) {
    controllerCount = 0;
}


/**
 * @brief      Get the number of controllers
 *
 * @return     Number of controllers (does not include default keypad)
 */
int  CTLControllerCount(void) {
    return controllerCount;
}


/**
 * @brief      Read a controller
 *
 * @param[in]  n     Controller ID. If -1 uses the first or keyboard if none
 *                   plugged in.
 *
 * @return     The controller state
 */
CTLState *CTLReadController(int n) {
    if (n < 0) {                                                                    // Autoselect
        if (controllerCount > 0) return &controllers[0];                            // If there is a plugged in controller, use that.
        #if ARTURO_PROCESS_KEYS == 1
        return KBDReadController();
        #else
        return NULL;
        #endif
    }
    return (n < controllerCount) ? &controllers[n] : NULL;
}


/**
 * @brief      Add a specific controller device to the device lists
 *
 * @param[in]  dev_addr  The device address
 * @param[in]  instance  The instance
 * @param[in]  vid       The USB vid
 * @param[in]  pid       The USB pid
 */
void CTLAddController(uint8_t dev_addr,uint8_t instance,uint16_t vid,uint16_t pid) {
    if (controllerCount < MAX_CONTROLLERS) {                                        // If not reached the end.
        //CONWriteString("%04x:%04x %02x %02x\r\n",vid,pid,dev_addr,instance);
        CTLState *cs = &controllers[controllerCount++];                             // Pointer to new controller
        cs->_hardwareID = CTL_HARDWARE_ID(dev_addr,instance);                       // Store the hardware ID, and the type ID.
        cs->_hardwareTypeID = CTL_DEVICE_TYPE_ID(vid,pid);
        cs->dx = cs->dy = 0;cs->a = cs->b = cs->x = cs->y = false;                  // Reset the rest
        if (CTLSendMessage(CTLM_REGISTER,cs->_hardwareID,NULL) == 0) {              // Identify it, if failed remove from list.
            controllerCount--;
        }
    }
}


/**
 * @brief      { function_description }
 *
 * @param[in]  dev_addr  The device address
 * @param[in]  instance  The instance
 * @param      report    The USB report
 * @param[in]  len       The length of the report
 */
void CTLUpdateController(uint8_t dev_addr,uint8_t instance,uint8_t const *report,uint8_t len) {
    if (false) {                                                                    // "true" here allows you to know exactly what data is being sent.
        CONWriteString("%d : ",len);
        for (int i = 0;i < len;i++) CONWriteString("%d:%02x ",i,report[i]);
        CONWriteString("\r\n");
    }
    struct _CTL_MessageData msgBlock;
    msgBlock.len = len;msgBlock.report = report;                                    // Construct the message block.
    CTLSendMessage(CTLM_UPDATE,CTL_HARDWARE_ID(dev_addr,instance),&msgBlock);       // Send it with the update message
}


/**
 * @brief      Dispatch message to drivers
 *
 * @param[in]  command  The command
 * @param[in]  hwid     The hwid
 * @param      msg      The message structure
 *
 * @return     { description_of_the_return_value }
 */
int CTLSendMessage(int command,uint16_t hwid,struct _CTL_MessageData *msg) {
    for (int i = 0;i < controllerCount;i++) {                                       // Identify which controller is in receipt of the message.
        if (controllers[i]._hardwareID == hwid) {
            return CTLDispatchMessage(command,&controllers[i],msg);                 // Dispatch and return.
        }
    }
    return 0;
}
