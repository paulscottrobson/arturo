/**
 * @file       main.c
 *
 * @brief      Simulator main program
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#include "artsim.h"

bool tick50HzHasFired = true;

#define FRAME_RATE  (50)

static int nextUpdateTime = 0;

//
//      Co-opt the USB updating routine, which doesn't happen in the simulator, to 
//      update the simulator, removing events from the queue and repainting the
//      display. This is because there is no tick interrupt.
//

int USBUpdate(void) {
    tick50HzHasFired = true;                                                        // Rigged so we fire every time round the main loop
    if (TMRReadTimeMS() >= nextUpdateTime) {                                        // So do this to limit the repaint rate to 50Hz.
        nextUpdateTime = TMRReadTimeMS()+100/FRAME_RATE;
        return SYSPollUpdate();                                                     // So we don't update *all* the time
    }
    return -1;
}

/**
 * @brief      Main program.
 *
 * @param[in]  argc  The count of arguments
 * @param      argv  The arguments array
 *
 * @return     { description_of_the_return_value }
 */
int main(int argc,char *argv[]) {
    DVISetMode(DVI_MODE_640_240_8);                                                 // Initialise display
    CONWrite(12);                                                                   // Clear the screen
    CONWriteString("Simulator booting\r\n\r\n");
    KBDReceiveEvent(0,0xFF,0);                                                      // Initialise keyboard manager
    FIOInitialise();                                                                // Initialise file system
    SYSOpen(false);                                                                 // Start SDL and Mouse/Controller/Sound that use it
    ApplicationRun();                                                               // Run the program
    SYSClose();                                                                     // Close down
    return(0);
}
