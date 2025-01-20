/**
 * @file       fonts.c
 *
 * @brief      Monochrome Font Rendering
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include <libraries.h>
#include "include/atomic.h"

#include "fontstructures.h"
#include "fonts/fontdata.h"
#include "fonts/fontindex.h"

/**
 * @brief      Draws a character from bitmap on the display.
 *
 * @param      vp          Viewport or NULL
 * @param[in]  xPos        X Position
 * @param[in]  yPos        Y Position
 * @param[in]  w           Bitmap width
 * @param[in]  h           Bitmap height
 * @param      bitmapData  The bitmap data
 * @param[in]  colour      The colour
 * @param[in]  scale       The scale
 */
static void _DrawCharacterFromBitmap(GFXPort *vp,int xPos,int yPos,int w,int h,uint8_t *bitmapData,int colour,int scale) {
    uint8_t bitMask = 0x80;                                                         // Bit to check.
    GFXCHECKMOUSE(vp);
    GFXASetPort(vp);                                                                // Set the viewport

    for (int y = 0;y < h;y++) {                                                     // For each line of actual data.
        for (int x = 0;x < w;x++) {                                                 // Each horizontal pixel ?
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


/**
 * @brief      Draws a character on the viewport
 *
 * @param      vp      Viewport or NULL
 * @param[in]  xPos    X position
 * @param[in]  yPos    Y position
 * @param[in]  ch      Character
 * @param[in]  fontid  Font id
 * @param[in]  colour  colour
 * @param[in]  scale   Scale
 *
 * @return     The pixel width of the font (advance to next, includes spacing)
 */
static int _DrawCharacter(GFXPort *vp,int xPos,int yPos,int ch,int fontid,int colour,int scale) {

    if (fontid < 0 || fontid >= FONT_COUNT) return 0;                               // Unknown font
    const FONTInfo *font = _dictionary[fontid];                                     // Get the font data for the font.

    if (ch >= FONT_FIRST_UDG && ch <= FONT_LAST_UDG) {                              // So, it might be a UDG
        _DrawCharacterFromBitmap(vp,xPos,yPos-8*scale,8,8,                          // Draw it using that UDG. it sort of works :)
                                        CONGetUDGGraphicData(ch),colour,scale);
        return 8;                                                                   // Return the horizontal advance (which is 8)
    }

    if (ch < font->first || ch > font->last) return 0;                              // Unknown character
    FONTGlyph *glyph = &(font->glyph[ch - font->first]);                            // Point to the glyph
    if (glyph->bitmapOffset == 0xFFFF) return 0;                                    // No bitmap offset here, character not in glyph set.
    uint8_t *bitmapData = font->bitmap+glyph->bitmapOffset;                         // This points to the actual bits

    xPos = xPos + scale * glyph->xOffset;                                           // Drawing pos for font.
    yPos = yPos + scale * glyph->yOffset;
    _DrawCharacterFromBitmap(vp,xPos,yPos,glyph->width,glyph->height,bitmapData,colour,scale);
    return glyph->xAdvance*scale;
}


/**
 * @brief      Draw a string on the viewport
 *
 * @param      vp      Viewport or NULL
 * @param[in]  xPos    X position
 * @param[in]  yPos    Y position
 * @param      s       ASCIIZ string
 * @param[in]  font    Font ID
 * @param[in]  colour  Dolour
 * @param[in]  scale   Scale
 */
void GFXDrawString(GFXPort *vp,int xPos,int yPos,char *s,int font,int colour,int scale) {
    int xWidth;
    if (GFXACTION(colour) == GFXA_SOLID) {                                          // Solid background
        int w,y1,y2;
        GFXGetStringExtent(s,font,scale,&w,&y1,&y2);                                // Get bounding box
        GFXFillRect(vp,xPos,yPos+y1,xPos+w,yPos+y2,(colour >> 8) & 0xFF);
        colour = colour & 0xFFFFF;
    }
    DVIGetScreenExtent(&xWidth,NULL);                                               // No point in drawing if off right of viewport/screen
    if (vp != NULL) xWidth = vp->width;
    while (*s != '\0' && xPos < xWidth) {
        unsigned char c = (unsigned char)(*s++);
        int w = _DrawCharacter(vp,xPos,yPos,c,font,colour,scale);
        xPos += w;
    }
}

/**
 * @brief      Get the extent of the string (e.g. how many pixels it takes)
 *
 * @param      s       ASCIIZ string
 * @param[in]  fontid  font id
 * @param[in]  scale   scale
 * @param      w       Address of int to store width
 * @param      y1      Address of int to store height above baseline
 * @param      y2      Address of int to store height below baseline.
 */
void GFXGetStringExtent(char *s,int fontid,int scale,int *w, int *y1,int *y2) {
    uint8_t ch;
    int wx,y1x,y2x;
    w = (w == NULL) ? &wx : w;                                                      // In case the targets are NULLs.
    y1 = (y1 == NULL) ? &y1x : y1;y2 = (y2 == NULL) ? &y2x:y2;
    *w = 0;*y1 = 0;*y2 = 0;                                                         // W is additive ,
    while (ch = (uint8_t)(*s++), ch != '\0') {                                      // Scan the string
        if (ch >= FONT_FIRST_UDG && ch <= FONT_LAST_UDG) {                          // UDGs are a special case
            *w += scale * 8;
            *y1 = min(*y1,-scale);
        } else {
            if (fontid >= 0 && fontid < FONT_COUNT) {                               // Known font.
                const FONTInfo *font = _dictionary[fontid];                         // Get the font data for the font.
                if (ch >= font->first && ch < font->last) {                         // Valid character.
                    FONTGlyph *glyph = &(font->glyph[ch - font->first]);            // Point to the glyph
                    *w += glyph->xAdvance * scale;
                    *y1 = min(*y1,glyph->yOffset*scale);
                    *y2 = max(*y2,glyph->yOffset*scale+glyph->height * scale);
                }
            }
        }
    }
}
