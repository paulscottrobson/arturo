// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      graphics.h
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      28th December 2024
//      Reviewed :  No
//      Purpose :   Graphics core header.
//
// ***************************************************************************************
// ***************************************************************************************

#pragma once

#include "fontinclude.h"

//
//		A drawing area on the display.
//
typedef struct _GFXPort {
	int x,y,width,height;  															// Viewport position, pixels. 
	int xOffset,yOffset;  															// Positional offset when drawing.
} GFXPort;

void GFXSetMode(int mode);

void GFXPortInitialise(GFXPort *vp,int x0,int y0,int x1,int y1);
void GFXScrollPort(GFXPort *vp,int xo,int yo);

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