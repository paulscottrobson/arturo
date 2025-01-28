/**
 * @file       test_app.c
 *
 * @brief      Test various kernel functions
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include "testapp.h"


static int mode = 3;
static int fg=3;                                                                    // 2 colour mode, you can change what they are.
static int bg=0;

/**
 * @brief      Run the main program
 */
void ApplicationRun(void) {
    int n = 0;
    DVISetMode(mode);VDUWrite(12);
    VDUWriteString("Kernel Demo Application\r");                                          
    //
    //      A typical 'main'
    //
    while (1) {
  
        n = KBDGetKey();                                                            // Echo any keystroke
        if (n != 0) VDUWriteString("%d %c\r",n,n);
        if (n == ' ') DemoApp_CheckFileIO();                                        // Dump the USB key on space

        if (n == 'm') {                                                             // Change working mode
            mode=(mode+1) % DVI_MODE_COUNT;
            DVISetMode(mode);
            VDUWrite(12);
            struct DVIModeInformation *dmi = DVIGetModeInformation();            
            VDUWriteString("Set mode to %d : %dx%dx%d\n",mode,dmi->width,dmi->height,1 << (dmi->bitPlaneCount * dmi->bitPlaneDepth));

        }


        if (KBDEscapePressed(true)) {                                               // Escaped ?
            VDUWriteString("Escape !\r");
        }

        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat
            
            int x,y,b,w;
            MSEGetState(&x,&y,&b,&w);                                               // Get the mouse state
            if (b != 0) VDUWriteString("Mouse:%d %d %d %d\r",x,y,b,w);              // Show mouse if any button pressed
             
                                        
            CTLState *c = CTLReadController(-1);                                    // Show controller state if anything pressed.
            if (c != NULL) {                                                        // Shouldn't fail as controller -1 uses the keyboard if no gamepad present
                if (c->dx != 0 || c->dy != 0 || c->a || c->b || c->x || c->y) {
                    VDUWriteString("DX:%3d DY:%3d A:%d B:%d X:%d Y:%d\r",c->dx,c->dy,
                                                c->a ? 1:0,c->b ? 1:0,c->x ? 1:0,c->y ? 1:0);
                }
            }
        }
    }
}

