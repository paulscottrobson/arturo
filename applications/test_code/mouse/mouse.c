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

#define WIN_COUNT   (16)

struct _Window {
    GFXPort viewPort;  
    bool    inUse;
    int     count;
} Window[WIN_COUNT];

/**
 * @brief      Update the window
 *
 * @param      w     Window to update
 */
static void WindowUpdate(struct _Window *w) {
    char temp[8];
    GFXPort *vp = &(w->viewPort);
    int c = rand() & 7;
    GFXFillRect(vp,0,0,vp->width,vp->height,c);
    GFXFrameRect(vp,0,0,vp->width-1,vp->height-1,7);
    sprintf(temp,"%d",w->count);
    GFXDrawString(vp,8,24,temp,FONT_SANS_12,c^7,1);
}

/**
 * @brief      Run main program
 */
void ApplicationRun(void) {
    int nextUpdate = 0;
    int current = 0;

    CONEnableConsole(false);
    GFXSetMode(DVI_MODE_640_240_8);
    GFXClearBackground();

    for (int i = 0;i < WIN_COUNT;i++) {
        Window[i].inUse = true;
        Window[i].count = 0;
        Window[i].viewPort.x = rand()%500;
        Window[i].viewPort.y = rand()%160 ;
        Window[i].viewPort.width = rand()%60+30;
        Window[i].viewPort.height = rand()%40+30;
        WindowUpdate(&Window[i]);
    }

    while (1) {

        GFXUpdateMouse();

        if (TMRReadTimeMS() > nextUpdate) {
            current = (current + 1) % WIN_COUNT;
            if (Window[current].inUse) {
                    Window[current].count += (current + 1);
                    WindowUpdate(&Window[current]);
            }
            nextUpdate = TMRReadTimeMS()+30;
        }

        if (KBDEscapePressed(true)) {                                               // Escaped ?
        }

        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat
        }
    }
}
