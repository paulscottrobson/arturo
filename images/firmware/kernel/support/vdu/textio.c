/**
* @file       textio.c
*
* @brief      Text mode driver
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

static uint xCursor = 0;                                                            // Posiiton in pixels
static uint yCursor = 0;
static uint fgCol = 7;                                                              // Foreground & Background colour
static uint bgCol = 0;

/**
 * @brief      Clear the screen to the background
 */
void VDUClearScreen(void) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();            
    for (uint x = 0; x < dmi->width; ++x) {
        for (uint y = 0; y < dmi->height; ++y) {
            VDUDrawPixel(x, y, bgCol);
        }
    }
}

/**
 * @brief      Set the cursor position
 *
 * @param[in]  x     Horizontal character position
 * @param[in]  y     Vertical character position
 */
void VDUSetCursor(int x,int y) {
    xCursor = x;yCursor = y;
}

/**
 * @brief      Backspace (8)
 */
void VDUBackspace(void) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();            
    if (xCursor > 0) {	    
        xCursor --;
    } else if (yCursor > 0) {
        yCursor --;
        xCursor = dmi->width/8-1;
    }
}

/**
 * @brief      New line (13)
 */
void VDUNewLine(void) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();            
    xCursor = 0;yCursor++;
    if (yCursor == dmi->height/8) {                                                // Scrolling.
        yCursor--;
        for (int i = 0;i < dmi->bitPlaneCount;i++) {
            uint8_t *s = dmi->bitPlane[i];
            memcpy(s,s+8*dmi->bytesPerLine,dmi->bytesPerLine*(dmi->height-8));
            memset(s+dmi->bytesPerLine*(dmi->height-8),0,8*dmi->bytesPerLine);
        }
    }
}

/**
 * @brief      Get the line data for line y of character c, in l-r bit format
 *
 *             (e.g. 0x80 is the leftmost bit)
 *
 * @param[in]  c     Character 0-255
 * @param[in]  y     Vertical position within character 0-7
 *
 * @return     { description_of_the_return_value }
 */
uint8_t VDUGetCharacterLineData(int c,int y) {
    if (c < ' ') return 0;
    if (c >= 0x80) return 0xFF;
    return font_8x8[(c - FONT_FIRST_ASCII) * FONT_CHAR_HEIGHT+y];
}

/**
 * @brief      Character output to text display
 *
 * @param[in]  c     Character to output (non control)
 */
void VDUWriteText(char c) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();            
    for (int y = yCursor*8; y < yCursor*8 + 8; ++y) {
        uint8_t font_bits = VDUGetCharacterLineData(c,y-yCursor*8);
        for (int i = 0; i < 8; ++i) {
            VDUDrawPixel(xCursor*8 + i, y, font_bits & (0x80 >> i) ? fgCol : bgCol);
        }
    }
    xCursor++;
    if (xCursor == dmi->width/8) VDUWrite(13);
}

