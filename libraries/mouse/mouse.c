

//
//      Name :      mouse.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      8th January 2025
//      Reviewed :  No
//      Purpose :   Mouse routines 
//



#include "common.h"
#include <libraries.h>

void MMGRDrawPointer(int xp,int yp,int size) {
	for (int i = 0;i < size;i++) {
		bool tbLine = (i == 0) || (i == size-1);
		GFXHLine(NULL,xp+i/2,xp+i*2,yp+i,tbLine ? 7 : 0);
		if (!tbLine) {
			GFXPlot(NULL,xp+i/2,yp+i,7);
			GFXPlot(NULL,xp+i*2,yp+i,7);
		}
	}
}