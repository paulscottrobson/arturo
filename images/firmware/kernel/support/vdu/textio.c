/**
* @file       textio.c
*
* @brief      Text driver
*
* @author     Paul Robson
*
* @date       27/01/2025
*
*/

#include "common.h"

#include "support/font_8x8.h"

#define FONT_CHAR_WIDTH 8
#define FONT_CHAR_HEIGHT 8
#define FONT_N_CHARS 95
#define FONT_FIRST_ASCII 32

/**
* @brief      Set one pixel on
*
* @param[in]  x     x position
* @param[in]  y     y position
* @param[in]  rgb   RGB 3 bit colour
*/

static void VDUDrawPixel(int x, int y, int rgb) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();                       // Identify mode data.

    if (dmi == NULL) return;
    uint8_t mask;
    if (dmi->bitPlaneDepth == 2) {

      mask = 0x80 >> (2*(x % 4));                             
      for (uint component = 0; component < dmi->bitPlaneCount; ++component) {       // Do each bitplane
        uint8_t *idx = (x / 4) + y * dmi->bytesPerLine + dmi->bitPlane[component];  // Work out byte.

        if (rgb & (1u << component))                                                // Set or clear the bit in the plane.
          *idx = *idx | mask;
        else
          *idx = *idx & (~mask);
        if (rgb & (1u << (component+3)))                                                // Set or clear the bit in the plane.
          *idx = *idx | (mask>>1);
        else
          *idx = *idx & (~(mask>>1));
      }
    } else {
      mask = 0x80 >> (x % 8);                                               // Mask from lower 8 bits.

      for (uint component = 0; component < dmi->bitPlaneCount; ++component) {       // Do each bitplane
        uint8_t *idx = (x / 8) + y * dmi->bytesPerLine + dmi->bitPlane[component];  // Work out byte.

        if (rgb & (1u << component))                                                // Set or clear the bit in the plane.
          *idx = *idx | mask;
        else
          *idx = *idx & (~mask);
      }
    }
}


//
//                              Console setup - very simple here.
//

static uint x0 = 0;                                                                 // Posiiton in pixels
static uint y0 = 0;
static uint fgcol = 7;
static uint bgcol = 0;


    /**
    * @brief      Write one character or control code.
    *
    * @param[in]  c     Character code
    */
void VDUWrite(int c) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();                       // Identify mode data.
    if (dmi == NULL) return;
    switch(c) {
        case 12:                                                                    // Clear Screen.
            for (uint x = 0; x < dmi->width; ++x)
                for (uint y = 0; y < dmi->height; ++y) {
                    VDUDrawPixel(x, y, 0);
            }
            x0 = y0 = 0;                                                            // Home cursor
            break;
        case 8:
        	  if (x0 >= 8) {	    
        	    x0 -= 8;
        	  } else if (y0 >= 8) {
        	    y0 -= 8;
        	    x0 = dmi->width - 8;
        	  }
            break;
        case 10:
        case 13:                                                                    // New line
            x0 = 0;y0 = y0 + 8;
            if (y0 == dmi->height) {                                                // Scrolling.
                y0 = y0 - 8;
                for (int i = 0;i < dmi->bitPlaneCount;i++) {
                    uint8_t *s = dmi->bitPlane[i];
                    memcpy(s,s+8*dmi->bytesPerLine,dmi->bytesPerLine*(dmi->height-8));
                    memset(s+dmi->bytesPerLine*(dmi->height-8),0,8*dmi->bytesPerLine);
                }
            }
            break;

        default:
            if (c>=FONT_FIRST_ASCII && c<FONT_FIRST_ASCII+FONT_N_CHARS) {           // ASCII character set
                for (int y = y0; y < y0 + 8; ++y) {
                    uint8_t font_bits = font_8x8[(c - FONT_FIRST_ASCII) * FONT_CHAR_HEIGHT+y-y0];
                    for (int i = 0; i < 8; ++i) {
                        VDUDrawPixel(x0 + i, y, font_bits & (0x80 >> i) ? fgcol : bgcol);
                    }
                }
                x0 = x0 + 8;
                if (x0 == dmi->width) VDUWrite(13);
            }
            break;
    }
}


/**
 * @brief      Bridge from CONWrite to VDUWrite. 
 * 
 * This is only compiled if ARTURO_PROCESS_CONSOLE is set to 1.
 *
 * @param[in]  c     Character to write
 */

#if ARTURO_PROCESS_CONSOLE==1

void CONWrite(int c) {
    VDUWrite(c);
}

#endif
