// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      graphics.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      28th December 2024
//      Reviewed :  No
//      Purpose :   Graphics core test
//
// ***************************************************************************************
// ***************************************************************************************

#include "common.h"
#include <libraries.h>
#include "gfxtest.h"

void TestDrawStuff(void);
void TestScrollAndRect(void);
void TestTriangles(void);
void TestFonts(void);
void Test64Colours(void);

// ***************************************************************************************
//
//                      Start and run the CPU. Does not have to return.
//
// ***************************************************************************************

static GFXPort vp;
static int action = -1;
static int width,height;

void UpdateMode(int mode) {
    GFXSetMode(mode);
    DVIGetScreenExtent(&width,&height);
    CONWrite(12);
    GFXFillRect(NULL,0,0,width,height,1);
    CONWriteString("Mode %d : %d x %d",mode,width,height);
    GFXPortInitialise(&vp,10,10,width-10,height-10);
}

void ApplicationRun(void) {
    int n = 0;
    int nextSkip = 0;
    int mode = DVI_MODE_320_256_8;

    UpdateMode(mode);

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
        if (TMRReadTimeMS() > nextSkip) {
            nextSkip = TMRReadTimeMS()+2000;
            GFXFillRect(&vp,0,0,width,height,0);
		    GFXScrollPort(&vp,0,0);
            action++;
        }
        for (int i = 0;i < 100;i++) {
        	TestDrawStuff();
        }

        if (KBDEscapePressed(true)) {                                               // Escaped ?
            CONWriteString("Escape !\r");
        }

        int k = KBDGetKey();
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

// ***************************************************************************************
//
//                                  Horizontal line test code
//
// ***************************************************************************************

static int randx() { return rand() % width; }
static int randy() { return rand() % height; }
static int randColour() { return rand() % 64; }

static int ctr = 0;

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

// ***************************************************************************************
//
//                                  Tests scrolling and frame/rect
//
// ***************************************************************************************

static int nextDraw = 0;

void TestScrollAndRect(void) {
    if (TMRReadTimeMS() < nextDraw) return;
    nextDraw = TMRReadTimeMS()+3;
    GFXScrollPort(&vp,0,0);
    GFXPlot(&vp,0,0,7);GFXPlot(&vp,vp.width-1,0,7);GFXPlot(&vp,0,vp.height-1,7);
    int s = -(ctr % 100);
    GFXScrollPort(&vp,s,s);
    GFXFillRect(&vp,9,9,301,101,ctr & 7);
    ctr++;
}

// ***************************************************************************************
//
//                      Test of all 64 colours in 320x240x64
//
// ***************************************************************************************

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
