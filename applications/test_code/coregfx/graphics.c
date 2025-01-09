/**
 * @file graphics.c
 *
 * @brief      Graphics test program
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */



#include "common.h"
#include <libraries.h>

void TestDrawStuff(void);
void TestScrollAndRect(void);
void TestTriangles(void);
void TestFonts(void);
void Test64Colours(void);


//
//      640x480 x 3 planes will ONLY work if DVI_SUPPORT_640_480_8 is set to 1
//
static GFXPort vp;
static int action = -1;
static int width,height;

/**
 * @brief      Update the mode being used
 *
 * @param[in]  mode  The mode to be used
 */
void UpdateMode(int mode) {
    GFXSetMode(mode);
    DVIGetScreenExtent(&width,&height);
    CONWrite(12);
    GFXFillRect(NULL,0,0,width,height,1);
    CONWriteString("Mode %d : %d x %d",mode,width,height);
    GFXPortInitialise(&vp,10,10,width-10,height-10);
}


/**
 * @brief      Run the main program
 */
void ApplicationRun(void) {
    int n = 0;
    int nextSkip = 0;
    int mode = 0;
    UpdateMode(mode);
    //
    //      Test sound is working
    //
    // SNDCHANNEL s;
    // s.frequency = 440;s.type = SNDTYPE_SQUARE;s.volume = 127;
    // SNDUpdate(0,&s);
    // s.frequency = 220;
    // SNDUpdate(1,&s);
    // Test64Colours();            
    //
    //      A typical 'main'
    //
    while (1) {
        if (TMRReadTimeMS() > nextSkip) {                                           // Once every 2 seconds
            nextSkip = TMRReadTimeMS()+2000;
            GFXFillRect(&vp,0,0,width,height,0);                                    // Clear screen and reset the scroll port (TestScrollAndRect() plays with it)
            GFXScrollPort(&vp,0,0);
            action++;                                                               // Do the next 'thing'
        }
        for (int i = 0;i < 100;i++) {
            TestDrawStuff();
        }

        if (KBDEscapePressed(true)) {                                               // Escaped ?
            CONWriteString("Escape !\r");
        }

        int k = KBDGetKey();                                                        // If a key is pressed go to the next screen mode
        if (k != 0) {
            mode = (mode + 1) % (DVI_MODE_MAX+1);
            UpdateMode(mode);
        }

        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat        
        }
    }
}


//
//      Simple support functions
//
static int randx() { return rand() % width; }
static int randy() { return rand() % height; }
static int randColour() { return rand() % 64; }

static int ctr = 0;

/**
 * @brief      Draw some elements , type defpendent on 'action'
 */
void TestDrawStuff(void) {
    int x = randx(),y = randy(),x1 = randx(),y1 = randy(),x2 = randx(),y2 = randy();
    switch(action) {
        case 0: 
            GFXDrawString(&vp,x,y,"Hello",rand()%FONT_COUNT,randColour(),1);break;      
        case 1:
            GFXLine(&vp,x,y,x1,y1,randColour());break;
        case 2:
            GFXLine(&vp,0,y,width,y,randColour());break;
        case 3:
            GFXLine(&vp,x,0,x,height,randColour());break;
        case 4:
            TestScrollAndRect();break;
        case 5:
            GFXFillEllipse(&vp,x,y,x1,y1,randColour());
            GFXFrameEllipse(&vp,x,y,x1,y1,randColour());
            GFXFrameRect(&vp,x,y,x1,y1,randColour());
            break;
        case 6:
            GFXFillTriangle(&vp,x,y,x1,y1,x2,y2,randColour());
            GFXFrameTriangle(&vp,x,y,x1,y1,x2,y2,randColour());break;
        default:
            action = 0;break;
    }
}


static int nextDraw = 0;

/**
 * @brief      This is specific to viewports scrolling, it adjusts the scrolling
 *             and keeps redrawing the rectangle so it looks like the scrolling
 *             is going up/left
 */
void TestScrollAndRect(void) {
    if (TMRReadTimeMS() < nextDraw) return;
    nextDraw = TMRReadTimeMS()+3;
    GFXScrollPort(&vp,0,0);                                                         // Reset scroll
    GFXPlot(&vp,0,0,7);GFXPlot(&vp,vp.width-1,0,7);GFXPlot(&vp,0,vp.height-1,7);    // For visual alignment check
    int s = -(ctr % 100);                                                           // Scroll the vp up to the right
    GFXScrollPort(&vp,s,s);
    GFXFillRect(&vp,9,9,301,101,ctr & 7);                                           // and repaint the square
    ctr++;
}


/**
 * @brief      A specific test for 320x240x64 colour mode
 */
void Test64Colours(void)
{
  DVISetMode(DVI_MODE_320_240_64);                                                    
  CONWrite(12);
  for (int i=0; i<64; i++) {
    CONSetColour(i,0);
    CONWriteString("12345");
    CONSetColour(0,i);
    CONWriteString("12345");
  }
  for (int i=0; i<8; i++) {
    CONSetColour(i*8,0);
    CONWriteString("Bright level");
    CONSetColour(i,0);
    CONWriteString("Bright level");
    CONSetColour(i*9,0);
    CONWriteString("Bright level");
    CONWrite(13);
  }
  CONSetColour(3,0);
}
