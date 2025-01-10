/**
 * @file       mouse.c
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

    struct _SmallBuffer scr;
    GFXSetMode(DVI_MODE_640_240_8);

    for (int i = 0;i < 140;i++) {
        GFXFrameRect(NULL,rand()%640,rand()%240,rand()%640,rand()%240,rand()&7);
    }

    GFXCopyScreenToSmallBuffer(32,32,&scr);
    GFXFillRect(NULL,32,32,47,47,7);

    while (1) {


        if (KBDEscapePressed(true)) {                                               // Escaped ?
            GFXCopySmallBufferToScreen(&scr);
        }

        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat
        }
    }
}
