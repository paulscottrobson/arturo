// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      port.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      28th December 2024
//      Reviewed :  No
//      Purpose :   Graphic Port functions
//
// ***************************************************************************************
// ***************************************************************************************

#include "common.h"

// ***************************************************************************************
//
//									Initialise a Graphics Port
//
// ***************************************************************************************

void GFXPortInitialise(GFXPort *vp,int x0,int y0,int x1,int y1) {
	struct DVIModeInformation *dmi = DVIGetModeInformation();
	vp->x = min(dmi->width-1,max(0,min(x0,x1)));  											// Assign position and size forcing into current mode.
	vp->y = min(dmi->height-1,max(0,min(y0,y1)));
	vp->width = min(dmi->width-vp->x,abs(x1-x0));
	vp->height = min(dmi->height-vp->y,abs(y1-y0));
	vp->xOffset = vp->yOffset = 0;
}

// ***************************************************************************************
//
//							Scroll port offset to new position
//
// ***************************************************************************************

void GFXScrollPort(GFXPort *vp,int xo,int yo) {
	vp->xOffset = xo;vp->yOffset = yo;
}


