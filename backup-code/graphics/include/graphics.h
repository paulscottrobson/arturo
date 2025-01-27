/**
 * @file       graphics.h
 *
 * @brief      Header file, graphics functions
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once

#include "fontinclude.h"

//
//      A drawing area on the display.
//
typedef struct _GFXPort {
    int x,y,width,height;                                                           // Viewport position, pixels. 
    int xOffset,yOffset;                                                            // Positional offset when drawing.
} GFXPort;

void GFXSetMode(int mode);

void GFXPortInitialise(GFXPort *vp,int x0,int y0,int x1,int y1);
void GFXScrollPort(GFXPort *vp,int xo,int yo);

void GFXClearBackground(void);
void GFXPlot(GFXPort *vp,int x,int y,int colour);
void GFXLine(GFXPort *vp,int x0, int y0, int x1, int y1,int colour);
void GFXHLine(GFXPort *vp,int x0, int x1, int y,int colour);
void GFXVLine(GFXPort *vp,int x, int y0, int y1,int colour);
void GFXFrameRect(GFXPort *vp,int x0,int x1,int y0,int y1,int colour);
void GFXFillRect(GFXPort *vp,int x0,int y0,int x1,int y1,int colour);
void GFXFillEllipse(GFXPort *vp,int x0,int y0,int x1,int y1,int colour);
void GFXFrameEllipse(GFXPort *vp,int x0,int y0,int x1,int y1,int colour);
void GFXFillTriangle(GFXPort *vp,int x0,int y0,int x1,int y1,int x2,int y2,int colour);
void GFXFrameTriangle(GFXPort *vp,int x0,int y0,int x1,int y1,int x2,int y2,int colour);

void GFXDrawString(GFXPort *vp,int xPos,int yPos,char *s,int font,int colour,int scale);
void GFXGetStringExtent(char *s,int font,int scale,int *w, int *y1,int *y2);

//
//      An area designed to store a max 16x16 pixel screen part (for cursors etc)
//
struct _SmallBuffer {
    int storePosition;                                                              // Start offset on display (-1 = no data)
    int xBytesPerLine;                                                              // Bytes stored per line.
    int ySize;                                                                      // Number of lines of data
    uint8_t *storage[3];                                                            // Buffer for the background planes
    uint8_t storageMemory[3 * (3 * 16)];                                            // Used memory.
};


void GFXCopyScreenToSmallBuffer(int xPos,int yPos,struct _SmallBuffer *buffer);
void GFXCopySmallBufferToScreen(struct _SmallBuffer *buffer);

void GFXUpdateMouse(void);