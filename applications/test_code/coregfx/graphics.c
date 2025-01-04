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

void ApplicationRun(void) {
    int n = 0;
    int nextSkip = 0;
    GFXSetMode(DVI_MODE_640_240_8);
    CONWriteString("Graphics Demo Application\r");                                  
    GFXPortInitialise(&vp,15,15,625,225);

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
        if (TMRRead() > nextSkip) {
            nextSkip = TMRRead()+200;
            GFXFillRect(&vp,0,0,640,480,0);
            action++;
        }
        TestDrawStuff();

        if (KBDEscapePressed(true)) {                                               // Escaped ?
            CONWriteString("Escape !\r");
        }

        if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
            TICK50_RESET();                                                         // Reset the tick flag
            if (USBUpdate() == 0) return;                                           // Update USB
            KBDCheckTimer();                                                        // Check for keyboard repeat        
        }
    }
}

// // ***************************************************************************************
// //
// //                                  Sound driver. (dummy)
// //
// // ***************************************************************************************

// int8_t ApplicationGetChannelSample(int channel) {
//     return rand() & 0xFF; 
// }

// ***************************************************************************************
//
//                                  Horizontal line test code
//
// ***************************************************************************************

static int randx() { return rand() % 640; }
static int randy() { return rand() % 240; }
static int randColour() { return rand() % 8; }

static int ctr = 0;

void TestDrawStuff(void) {
    int x = randx(),y = randy(),x1 = randx(),y1 = randy(),x2 = randx(),y2 = randy();
    switch(action) {
        case 0: 
            GFXDrawString(&vp,x,y,"Hello",rand()%FONT_COUNT,randColour(),1);        
        case 1:
            GFXLine(&vp,x,y,x1,y1,randColour());break;
        case 2:
            GFXLine(&vp,0,y,640,y,randColour());break;
        case 3:
            GFXLine(&vp,x,0,x,480,randColour());break;
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
    if (TMRRead() < nextDraw) return;
    nextDraw = TMRRead()+3;
    GFXScrollPort(&vp,0,0);
    GFXPlot(&vp,0,0,7);GFXPlot(&vp,vp.width-1,0,7);GFXPlot(&vp,0,vp.height-1,7);
    int s = -(ctr % 100);
    GFXScrollPort(&vp,s,s);
    GFXFillRect(&vp,9,9,301,101,ctr & 7);
    ctr++;
}

// ***************************************************************************************
//
//                                  Ellipse test
//
// ***************************************************************************************

void TestEllipse(void) {
}

// ***************************************************************************************
//
//                                  Font test
//
// ***************************************************************************************

void TestFonts(void) {
    for (int i = 0;i < FONT_COUNT;i++) {
        char buffer[32];
        int x = i % 6 * 80;
        int y = i / 6 * 24 + 24;
        sprintf(buffer,"%d__iM",i);
        GFXDrawString(&vp,x,y,buffer,i,i % 7 + 1,1);
    }
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
