// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      fonts.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//                  Andrew S. Owen (Font Design)
//      Date :      30th December 2024
//      Reviewed :  No
//      Purpose :   Monochrome Font Rendering
//
// ***************************************************************************************
// ***************************************************************************************

#include "common.h"
#include "include/atomic.h"

// ***************************************************************************************
//
//		                    Include the font data in Flash memory. 
//
// ***************************************************************************************

#include "fontstructures.h"
#include "fonts/fontdata.h"
#include "fonts/fontindex.h"

// ***************************************************************************************
//
//          Draw a single character, return the x adjustment to the next character.
//
// ***************************************************************************************

static int _DrawCharacter(GFXPort *vp,int xPos,int yPos,int ch,int fontid,int colour,int scale) {	

	if (fontid < 0 || fontid >= FONT_COUNT) return 0;                               // Unknown font
	const FONTInfo *font = _dictionary[fontid];   									// Get the font data	
	if (ch < font->first || ch > font->last) return 0;                   	        // Unknown character.
	FONTGlyph *glyph = &(font->glyph[ch - font->first]);   							// Point to the glyph

	uint8_t *bitmapData = font->bitmap+glyph->bitmapOffset;  						// This points to the actual bits
	uint8_t bitMask = 0x80;                                                         // Bit to check.

	GFXASetPort(vp);  																// Set the viewport

	xPos = xPos + scale * glyph->xOffset;
	yPos = yPos + scale * glyph->yOffset;
	// Background ?

	for (int y = 0;y < glyph->height;y++) {                                         // For each line of actual data.
	    for (int x = 0;x < glyph->width;x++) {                                      // Each horizontal pixel ?
	        if (((*bitmapData) & bitMask) != 0) {                                   // Plot bitmap.
	            if (scale == 1) {                                                   // Pixel or Rectangle.
	                GFXAPlot(xPos+x*scale,yPos,colour);
	            } else {
	                GFXFillRect(vp,xPos+x*scale,yPos,xPos+x*scale+scale-1,yPos+scale-1,colour);
	            }
	        }            
	        bitMask >>= 1;                                                          // Next bit
	        if (bitMask == 0) {
	            bitMask = 0x80;bitmapData++;
	        }
	    }
	    yPos += scale;                                                              // Next line down.
	}
	return glyph->xAdvance;
}

// ***************************************************************************************
//
//                                      Draw font
//
// ***************************************************************************************

void GFXDrawString(GFXPort *vp,int xPos,int yPos,char *s,int font,int colour,int scale) {
	while (*s != '\0' && xPos < vp->width) {
		int w = _DrawCharacter(vp,xPos,yPos,*s++,font,colour,scale);
		xPos += w;
	}
}

