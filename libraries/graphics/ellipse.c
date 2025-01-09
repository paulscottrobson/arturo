

//
//      Name :      ellipse.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
// 					Akanksha Rai (Midpoint algorithm)
//      Date :      29th December 2024
//      Reviewed :  No
//      Purpose :   Ellipse Drawing functions
//



#include "common.h"
#include <libraries.h>
#include "include/atomic.h"

static int rx,ry,xc,yc;

static void _GFXFramePart(int x,int y,int colour);
static void _GFXDrawEllipse(int colour, bool fill);
static void _GFXFrameEllipseMain(GFXPort *vp,int x0,int y0,int x1,int y1,int colour,bool fill);
static void _GFXDrawEllipseMain(GFXPort *vp,int x0,int y0,int x1,int y1,int colour,bool fill);


//
//						Wrapper functions minimising code duplication
//


void GFXFrameEllipse(GFXPort *vp,int x0,int y0,int x1,int y1,int colour) {
	_GFXDrawEllipseMain(vp,x0,y0,x1,y1,colour,false);
}

void GFXFillEllipse(GFXPort *vp,int x0,int y0,int x1,int y1,int colour) {
	_GFXDrawEllipseMain(vp,x0,y0,x1,y1,colour,true);
}


//
//						Main drawing program, supporting fill & frame
//


static void _GFXDrawEllipseMain(GFXPort *vp,int x0,int y0,int x1,int y1,int colour,bool fill) {
	GFXASetPort(vp);
	rx = abs(x0-x1)/2;ry = abs(y0-y1)/2;
	xc = (x0+x1)/2;yc = (y0+y1)/2;
	_GFXDrawEllipse(colour,fill);
}


//
//									Draw the frame part
//


static void _GFXFramePart(int x,int y,int colour) {
	GFXAPlot(xc+x,yc+y,colour);
	if (x != 0) {  																// If at 0 horizontal only do once
		GFXAPlot(xc-x,yc+y,colour);
	}
	if (y != 0) {  																// If at 0 vertical only do once.
		GFXAPlot(xc+x,yc-y,colour);
		GFXAPlot(xc-x,yc-y,colour);
	}
}


//
//                                  Draw the line
//


static void _GFXLinePart(int x,int y,int colour) {
    GFXAHorizLine(xc-x,xc+x,yc+y,colour);
    if (yc != 0) {                                                              // Don't redraw the middle.
        GFXAHorizLine(xc-x,xc+x,yc-y,colour);
    }
}


//
//								Midpoint Ellipse Algorithm
//


static void _GFXDrawEllipse(int colour, bool fill) {
    float dx, dy, d1, d2, x, y;
    x = 0;y = ry;
    d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;
 
    while (dx < dy) 
    {
        if (fill) {
            _GFXLinePart(x,y,colour);
        } else {
            _GFXFramePart(x,y,colour);
        }
        if (d1 < 0)
        {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        } else {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }
 
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) + 
         ((rx * rx) * ((y - 1) * (y - 1))) -
          (rx * rx * ry * ry);
 
    while (y >= 0)
    {
        if (fill) {
            _GFXLinePart(x,y,colour);
        } else {
            _GFXFramePart(x,y,colour);
        }
        if (d2 > 0) 
        {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        } else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}
 
 
