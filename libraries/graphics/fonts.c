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
#include <libraries.h>
#include "include/atomic.h"

// ***************************************************************************************
//
//		                    Include the font data in Flash memory. 
//
// ***************************************************************************************

#include "fontstructures.h"
#include "fonts/fontdata.h"
#include "fonts/fontindex.h"

static void _DrawCharacterFromBitmap(GFXPort *vp,int xPos,int yPos,int w,int h,uint8_t *bitmapData,int colour,int scale) {
	uint8_t bitMask = 0x80;                                                         // Bit to check.
	GFXASetPort(vp);  																// Set the viewport

	for (int y = 0;y < h;y++) {                     			                    // For each line of actual data.
	    for (int x = 0;x < w;x++) {        			            	                // Each horizontal pixel ?
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
}

// ***************************************************************************************
//
//          Draw a single character, return the x adjustment to the next character.
//
// ***************************************************************************************

static int _DrawCharacter(GFXPort *vp,int xPos,int yPos,int ch,int fontid,int colour,int scale) {	

	if (fontid < 0 || fontid >= FONT_COUNT) return 0;       	                   	// Unknown font
	const FONTInfo *font = _dictionary[fontid];   									// Get the font data for the font.

	if (ch >= FONT_FIRST_UDG && ch <= FONT_LAST_UDG) {   							// So, it might be a UDG
		_DrawCharacterFromBitmap(vp,xPos,yPos-8*scale,8,8,							// Draw it using that UDG. it sort of works :)
										CONGetUDGGraphicData(ch),colour,scale);	
		return 8;  																	// Return the horizontal advance (which is 8)
	}

	if (ch < font->first || ch > font->last) return 0; 								// Unknown character
	FONTGlyph *glyph = &(font->glyph[ch - font->first]);   							// Point to the glyph
	if (glyph->bitmapOffset == 0xFFFF) return 0;  									// No bitmap offset here, character not in glyph set.
	uint8_t *bitmapData = font->bitmap+glyph->bitmapOffset;  						// This points to the actual bits

	xPos = xPos + scale * glyph->xOffset;  											// Drawing pos for font.
	yPos = yPos + scale * glyph->yOffset;
	_DrawCharacterFromBitmap(vp,xPos,yPos,glyph->width,glyph->height,bitmapData,colour,scale);
	return glyph->xAdvance;
}


// ***************************************************************************************
//
//                                      Draw font
//
// ***************************************************************************************

void GFXDrawString(GFXPort *vp,int xPos,int yPos,char *s,int font,int colour,int scale) {
	int xWidth;
	DVIGetScreenExtent(&xWidth,NULL);  												// No point in drawing if off right of viewport/screen
	if (vp != NULL) xWidth = vp->width;
	while (*s != '\0' && xPos < xWidth) {
		unsigned char c = (unsigned char)(*s++);
		int w = _DrawCharacter(vp,xPos,yPos,c,font,colour,scale);
		xPos += w;
	}
}

