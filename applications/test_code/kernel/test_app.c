// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      test_app.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      18th December 2024
//      Reviewed :  No
//      Purpose :   Test application
//
// ***************************************************************************************
// ***************************************************************************************

#include "common.h"
#include "testapp.h"

// ***************************************************************************************
//
//                      Start and run the CPU. Does not have to return.
//
// ***************************************************************************************

static int mode;
static int fg=3;
static int bg=0;

void ApplicationRun(void) {
    int n = 0;
    CONWriteString("Kernel Demo Application\r");                                          
    PrintHello();                                                                   // A library function
    //
    //      A typical 'main'
    //
    while (1) {
  
        n = KBDGetKey();                                                            // Echo any keystroke
        if (n != 0) CONWriteString("%d %c\r",n,n);
        if (n == ' ') DemoApp_CheckFileIO();                                        // Dump the USB key on space

	if (n == 'm') {
	  mode=(mode+1) % 5;
	  DVISetMode(mode);
	  CONWrite(12);
	  fg = 3;
	  bg = 0;
	  if (mode==2) {
	    DVISetMonoColour(fg, bg);
	    CONSetColour(1, 0);
	  } else {
	      CONSetColour(fg, bg);
	  }	    
	  CONWriteString("Set mode to %d\n",mode);
	}
	if (n == 'f') {
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
	if (n == 'b') {
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
             MSEGetState(&x,&y,&b,&w);
             if (b != 0) CONWriteString("Mouse:%d %d %d %d\r",x,y,b,w);              // Show mouse if button pressed
             
             if (CTLControllerCount() != 0) {                                        // Show controller state if anything pressed.
                CTLState *c = CTLReadController(0);
                if (c->dx != 0 || c->dy != 0 || c->a || c->b || c->x || c->y) {
                    CONWriteString("DX:%3d DY:%3d A:%d B:%d X:%d Y:%d\r",c->dx,c->dy,
                                             c->a ? 1:0,c->b ? 1:0,c->x ? 1:0,c->y ? 1:0);
                    }
                }
        }
    }
}

// ***************************************************************************************
//
//     Sound driver. The left channel (0) is a beep, the right channel (1) white noise
//
// ***************************************************************************************

static int count = 0;

int8_t ApplicationGetChannelSample(int channel) {
    count++;
    if (channel == 0) {                                                             // Square wave on left channel.
        int toggleRate = SNDGetSampleFrequency() / (440*2);                         // 440Hz is A
        return ((count/toggleRate) & 1) ? -127:127;
    }
    if (channel == 1) {                                                             // Noise on right channel if supported
         return rand() & 0xFF;
    } 
    return 0; 
}
