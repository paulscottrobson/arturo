/**
 * @file test_app.c
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


static int mode;
static int fg=3;                                                                    // 2 colour mode, you can change what they are.
static int bg=0;

/**
 * @brief      Run the main program
 */
void ApplicationRun(void) {
    int n = 0;
    CONWriteString("Kernel Demo Application\r");                                          
    //
    //      A typical 'main'
    //
    while (1) {
  
        n = KBDGetKey();                                                            // Echo any keystroke
        if (n != 0) CONWriteString("%d %c\r",n,n);
        if (n == ' ') DemoApp_CheckFileIO();                                        // Dump the USB key on space

    if (n == 'm') {                                                                 // Change working mode
        mode=(mode+1) % 5;
        DVISetMode(mode);
        CONWrite(12);
        fg = 3;
        bg = 0;
        if (mode==2) {                                                              // Set colour on Mode 2,640x480 mono
            DVISetMonoColour(fg, bg);
            CONSetColour(1, 0);
        } else {
            CONSetColour(fg, bg);
        }       
        CONWriteString("Set mode to %d\n",mode);
    }

    if (n == 'f') {                                                                 // Set foreground
        fg++;
        if (mode==3 && fg==64) {
            fg = 0; 
        }
        else if (mode!=3 && (fg & 7)==0) {
            fg = 0;
        }
        if (mode==2) {
            DVISetMonoColour(fg, bg);
        } else {
            CONSetColour(fg, bg);
        }       
            CONWriteString("Set foreground to %d\n",fg);
        }

    if (n == 'b') {                                                                 // Set background
        bg++;
        if (mode==3 && bg==64) {
            bg = 0; 
        }
        else if (mode!=3 && (bg & 7)==0) {
            bg = 0;
        }
        if (mode==2) {
            DVISetMonoColour(fg, bg);
        } else {
            CONSetColour(fg, bg);
        }       
            CONWriteString("Set background to %d\n",bg);
        }
     
    
        if (KBDEscapePressed(true)) {                                               // Escaped ?
            CONWriteString("Escape !\r");
        }

    
        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat
            
            int x,y,b,w;
            MSEGetState(&x,&y,&b,&w);                                               // Get the mouse state
            if (b != 0) CONWriteString("Mouse:%d %d %d %d\r",x,y,b,w);              // Show mouse if any button pressed
             
                                        
            CTLState *c = CTLReadController(-1);                                    // Show controller state if anything pressed.
            if (c != NULL) {                                                        // Shouldn't fail as controller -1 uses the keyboard if no gamepad present
                if (c->dx != 0 || c->dy != 0 || c->a || c->b || c->x || c->y) {
                    CONWriteString("DX:%3d DY:%3d A:%d B:%d X:%d Y:%d\r",c->dx,c->dy,
                                                c->a ? 1:0,c->b ? 1:0,c->x ? 1:0,c->y ? 1:0);
                }
            }
        }
    }
}

