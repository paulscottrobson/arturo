

//
//      Name :      drawing.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      29th December 2024
//      Reviewed :  No
//      Purpose :   Drawing functions
//



#include "common.h"
#include <libraries.h>
#include "include/atomic.h"


//
//										Draw a frame
//


void GFXFrameRect(GFXPort *vp,int x0,int y0,int x1,int y1,int colour) {
	GFXASetPort(vp);
	if (y0 > y1) { int n = y0;y0 = y1;y1 = n; }  									// Sort horizontally.
	GFXAHorizLine(x0,x1,y0,colour);  												// Top line.
	if (y1 != y0) GFXAHorizLine(x0,x1,y1,colour);									// Bottom line, if not a one line frame.
	for (int y = y0+1;y < y1;y++) {
		GFXAPlot(x0,y,colour);
		if (x0 != x1) GFXAPlot(x1,y,colour);
	}
}


//
//										Draw a rectangle
//


void GFXFillRect(GFXPort *vp,int x0,int y0,int x1,int y1,int colour) {
	GFXASetPort(vp);
	for (int y = y0;y <= y1;y++) {
		GFXAHorizLine(x0,x1,y,colour);
	}
}


//
//								Wrappers for atomic functions
//


void GFXPlot(GFXPort *vp,int x,int y,int colour) {
	GFXASetPort(vp);
	GFXAPlot(x,y,colour);
}

void GFXLine(GFXPort *vp,int x0, int y0, int x1, int y1,int colour) {
	GFXASetPort(vp);
	GFXALine(x0,y0,x1,y1,colour);	
}

void GFXHLine(GFXPort *vp,int x0, int x1, int y,int colour) {
	GFXASetPort(vp);
	GFXAHorizLine(x0,x1,y,colour);	
}

void GFXVLine(GFXPort *vp,int x, int y0, int y1,int colour) {
	GFXASetPort(vp);
	GFXAHorizLine(x,y0,y1,colour);	
}

