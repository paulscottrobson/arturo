/**
 * @file   mouse.c
 *
 * @brief      Mouse testcode
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include <libraries.h>

/**
 * @brief      Run main program
 */
void ApplicationRun(void) {

    for (int i = 0;i < 140;i++) {
        GFXFrameRect(NULL,rand()%640,rand()%240,rand()%640,rand()%240,rand()&7);
    }

    MMGRDrawPointer(320,120,10);

    while (1) {


        if (KBDEscapePressed(true)) {                                               // Escaped ?
        }

        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat
        }
    }
}
