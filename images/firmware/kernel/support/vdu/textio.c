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

static uint xCursor = 0;                                                            // Posiiton in character cells in the window.
static uint yCursor = 0;
static uint fgCol = 7;                                                              // Foreground & Background colour
static uint bgCol = 0;

static int  xLeft = 0,yTop = 0,xRight = 79,yBottom = 29;                            // Text window (these are inclusive values)

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

    if (c < 32 || c == 127) return;                                                 // Not a displayable character
    if (x < xLeft || x > xRight || y < yTop || y > yBottom) return;                 // Out of the text window.

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
 * @brief      Clear the screen to the background, inside the text window.
 */
void VDUClearScreen(void) {    
    for (int x = xLeft;x <= xRight;x++) {                                           // Probably quick enough....
        for (int y = yTop;y <= yBottom;y++) {
            _VDURenderCharacter(x,y,' ');
        }
    }
}

/**
 * @brief      Home cursor to top left of current window
 */
void VDUHomeCursor(void) {
    VDUSetTextCursor(xLeft,yTop);
}

/**
 * @brief      Set the cursor position in the text window
 *
 * @param[in]  x     Horizontal character position
 * @param[in]  y     Vertical character position
 */
void VDUSetTextCursor(int x,int y) {
    if (x >= 0 && y >= 0 && x < xRight-xLeft && y <= yBottom-yTop) {
        xCursor = x;yCursor = y;
    }
}

/**
 * @brief      Set the text foreground/background colour
 *
 * @param[in]  colour  Colour in bits 0..6, foreground bit 7 = 0, background bit 7 = 1
 */
void VDUSetTextColour(int colour) {
    if (colour & 0x80) {
        bgCol = colour & 0x7F;
    } else {
        fgCol = colour & 0x7F;
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

#include "support/font_8x8.h"                                                       

uint8_t VDUGetCharacterLineData(int c,int y) {
    if (c < ' ') return 0;
    if (c >= 0x80) return 0xFF;
    return font_8x8[(c - ' ') * 8+y];
}

/**
 * @brief      Character output to text display
 *
 * @param[in]  c     Character to output (non control)
 */
void VDUWriteText(char c) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();            
    _VDURenderCharacter(xCursor+xLeft,yCursor+yTop,c);
    xCursor++;
    if (xCursor == dmi->width/8) VDUWrite(13);
}

/**
 * @brief      Reset the default text colours.
 */
void VDUSetDefaultTextColour(void) {
    VDUSetTextColour(0x07);
    VDUSetTextColour(0x80);
}

/**
 * @brief      Reset the text window
 */
void VDUResetTextWindow(void) {    
    struct DVIModeInformation *dmi = DVIGetModeInformation();            
    xLeft = yTop = 0;
    xRight = (dmi->width >> 3)-1;
    yBottom = (dmi->height >> 3) - 1;
}

/**
 * @brief      Set the text window (cells are inclusive)
 *
 * @param[in]  x1    x Left
 * @param[in]  y1    y Bottom
 * @param[in]  x2    x Right
 * @param[in]  y2    y Top
 */
void VDUSetTextWindow(int x1,int y1,int x2,int y2) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();            
    int w = (dmi->width >> 3)-1;
    int h = (dmi->height >> 3)-1;
    xLeft = x1;yTop = y2;
    xRight = min(w,x2);yBottom = min(h,y1);
}