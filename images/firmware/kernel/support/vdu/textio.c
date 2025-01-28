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

#define FONT_N_CHARS 95
#define FONT_FIRST_ASCII 32


static uint xCursor = 0;                                                            // Posiiton in pixels
static uint yCursor = 0;
static uint fgCol = 7;                                                              // Foreground & Background colour
static uint bgCol = 0;


/**
 * @brief      Convert a pixel pattern to the byte to write to the plane
 *             adjusting for foreground and background colours.
 *
 * @param[in]  line   Pixel Data
 * @param[in]  plane  Bitplane.
 *
 * @return     Modified pixel data.
 */
static uint8_t _VDUMapToBitplaneByte(uint8_t line,int plane) {
    uint8_t mask = 1 << plane;
    if ((fgCol & mask) == (bgCol & mask)) {                                         // Same bit pattern.
        return (fgCol & mask) ? 0xFF:0x00;
    }
    return (fgCol & mask) ? line : line ^ 0xFF;
}

/**
 * @brief      Output a character onto the display, text mode, current fgr/bgr
 *
 * @param[in]  x     x Coordinate
 * @param[in]  y     y Coordinate
 * @param[in]  c     Character to write
 */
static void _VDURenderCharacter(int x,int y,int c) {

    static const uint8_t _pixelMap[16] = {                                          // Convert 4 bit pixel to extended 8 bit byte.
        0x00,0x03,0x0C,0x0F,0x30,0x33,0x3C,0x3F,
        0xC0,0xC3,0xCC,0xCF,0xF0,0xF3,0xFC,0xFF
    };

    // TODO: Check if in text window (also on screen), also check for 0-31

    struct DVIModeInformation *dmi = DVIGetModeInformation();            
    for (int plane = 0;plane < dmi->bitPlaneCount;plane++) {                        // Do all three planes.
        for (int yChar = 0;yChar < 8;yChar++) {                                     // Each line in a bit plane.
            uint8_t pixels = VDUGetCharacterLineData(c,yChar);                      // Get the character line data.
            uint8_t *p = dmi->bitPlane[plane] + (y*8+yChar)*dmi->bytesPerLine;      // Position in bitmap.
            if (dmi->bitPlaneDepth == 1) {                                          // Handle 8 bits per bitmap (2,8 colours)
                *(p+x) = _VDUMapToBitplaneByte(pixels,plane);
            } else {                                                                // Handle 4 bits per bitmap (64 colours)
                *(p+x*2) = _VDUMapToBitplaneByte(_pixelMap[pixels >> 4],plane);
                *(p+x*2+1) = _VDUMapToBitplaneByte(_pixelMap[pixels & 0x0F],plane);
            }
        }
    }
}

/**
 * @brief      Clear the screen to the background
 */
void VDUClearScreen(void) {    
    struct DVIModeInformation *dmi = DVIGetModeInformation();            
    for (int x = 0;x < dmi->width>>3;x++) {
        for (int y = 0;y < dmi->height>>3;y++) {
            _VDURenderCharacter(x,y,x+y*3);
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
    return font_8x8[(c - FONT_FIRST_ASCII) * 8+y];
}

/**
 * @brief      Character output to text display
 *
 * @param[in]  c     Character to output (non control)
 */
void VDUWriteText(char c) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();            
    _VDURenderCharacter(xCursor,yCursor,c);
    xCursor++;
    if (xCursor == dmi->width/8) VDUWrite(13);
}

