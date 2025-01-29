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


/**
 * @brief      Set up for text window test
 */
static void _TextWindowTest(void) {
    VDUWrite(17);VDUWrite(129);VDUWrite(12);                                        // Clear background to red
    VDUWrite(31);VDUWrite(10);VDUWrite(1);

    VDUWrite(23);VDUWrite(0xE0);VDUWrite(0xFF);                                     // Define $E0
    for (int i = 0;i < 6;i++) VDUWrite(0x81);
    VDUWrite(0xFF);

    VDUWriteString("Kernel Demo Application\r\nNext line %c%c\r\n",0xE0,0x2A);                                          
    VDUWrite(17);VDUWrite(132);                                                     // Background blue
    VDUWrite(17);VDUWrite(2);
    VDUWrite(28);VDUWrite(4);VDUWrite(4);VDUWrite(35);VDUWrite(22);                 // Create a working window
    VDUWrite(12);                                                                   // Clear it
}

/**
 * @brief      Do the graphics test.
 */
static void _GraphicsTest(void) {

    GFXAPlot(0,0);GFXAPlot(160,120);
    GFXAPlot(160,238);GFXAPlot(162,239);
    GFXAPlot(319,120);

    GFXAHorizLine(162,317,120);
    GFXAVertLine(160,122,236);
    GFXALine(2,2,158,118);
}

/**
 * @brief      Run the main program
 */
void ApplicationRun(void) {
    int n = 0;
    int mode = 3;
    VDUWrite(22);VDUWrite(mode);                                                    // Switch mode
    //_TextWindowTest();
    _GraphicsTest();

    while (1) {
  
        n = KBDGetKey();                                                            // Echo any keystroke
        if (n != 0) {                                                               // Handle the CR/LF issue.
            VDUWrite(n);
            if (n == 13) VDUWrite(10);
        }
        if (n == ' ') DemoApp_CheckFileIO();                                        // Dump the USB key on space

        if (n == 'm') {                                                             // Change working mode
            mode=(mode+1) % DVI_MODE_COUNT;
            VDUWrite(22);VDUWrite(mode); 
            VDUWrite(17);VDUWrite(3);VDUWrite(17);VDUWrite(0x81);
            struct DVIModeInformation *dmi = DVIGetModeInformation();            
            VDUWriteString("Set mode to %d : %dx%dx%d\r\n",mode,dmi->width,dmi->height,1 << (dmi->bitPlaneCount * dmi->bitPlaneDepth));
            _GraphicsTest();
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
