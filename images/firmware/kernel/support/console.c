/**
 * @file       console.c
 *
 * @brief      Console output code, for debugging primarily.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"

#include "support/font_8x8.h"

#define FONT_CHAR_WIDTH 8
#define FONT_CHAR_HEIGHT 8
#define FONT_N_CHARS 95
#define FONT_FIRST_ASCII 32

static uint8_t udgData[(FONT_LAST_UDG-FONT_FIRST_UDG+1) * 8];                       // the data for the 32 (currently) 8x8 UDGs


/**
 * @brief      Set one pixel on
 *
 * @param[in]  x     x position
 * @param[in]  y     y position
 * @param[in]  rgb   RGB 3 bit colour
 */
void CONDrawPixel(int x, int y, int rgb) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();                       // Identify mode data.
    if (dmi == NULL) return;
    uint8_t mask;
    if (dmi->bitPlaneDepth == 2) {

      mask = 0x80 >> (2*(x % 4));                             for (uint component = 0; component < dmi->bitPlaneCount; ++component) {       // Do each bitplane
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
static uint fgcol = CON_COL_WHITE;                                                  // Text colour
static uint bgcol = CON_COL_BLACK;
static bool conIsEnabled = true;                                                    // Console is enabled

/**
 * @brief      Console system initialise
 */
void CONInitialise(void) {
    CONEnableConsole(true);
    CONWrite(12);                                                                   // CHR(12) is clear screen.
    CONSetColour(CON_COL_GREEN,CON_COL_BLACK);
}


/**
 * @brief      Enable / Disable console output
 *
 * @param[in]  isOn  Consol is on ?
 */
void CONEnableConsole(int isOn) {
    conIsEnabled = (isOn != 0);
}

/**
 * @brief      Set console colours
 *
 * @param[in]  foreground  The foreground
 * @param[in]  background  The background
 */
void CONSetColour(int foreground,int background) {
    fgcol = foreground;bgcol = background;
}


/**
 * @brief      Write one character or control code.
 *
 * @param[in]  c     Character code
 */
void CONWrite(int c) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();                       // Identify mode data.
    if (dmi == NULL) return;
    if (!conIsEnabled) return;
    switch(c) {
        case 12:                                                                    // Clear Screen.
            for (uint x = 0; x < dmi->width; ++x)
                for (uint y = 0; y < dmi->height; ++y)
                    CONDrawPixel(x, y, CON_COL_BLACK);
            x0 = y0 = 0;                                                            // Home cursor
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
            bool isUDG = (c >= FONT_FIRST_UDG && c <= FONT_LAST_UDG);
            if ((c>=FONT_FIRST_ASCII && c<FONT_FIRST_ASCII+FONT_N_CHARS)||isUDG) {  // ASCII character set or UDGs
                for (int y = y0; y < y0 + 8; ++y) {
                    uint8_t font_bits = isUDG ? CONGetUDGGraphicData(c)[y-y0] : font_8x8[(c - FONT_FIRST_ASCII) * FONT_CHAR_HEIGHT+y-y0];
                    for (int i = 0; i < 8; ++i) {
                        CONDrawPixel(x0 + i, y, font_bits & (0x80 >> i) ? fgcol : bgcol);
                        }
                    }
                x0 = x0 + 8;
                if (x0 == dmi->width) CONWrite(13);
            }
            break;
    }
}

/**
 * @brief      UDG Definition
 *
 * @param[in]  udg      UDG Code (224-255)
 * @param      bitData  8 bytes of pixel data
 */
void CONDefineUDG(int udg,uint8_t *bitData) {
    if (udg >= FONT_FIRST_UDG && udg <= FONT_LAST_UDG) {
        uint8_t *udgRAMData = udgData + (udg-FONT_FIRST_UDG) * 8;                   // Address of RAM
        for (int i = 0;i < 8;i++) *udgRAMData++ = *bitData++;                       // Copy the bitmap data there.
    }
}


/**
 * @brief      Read the data for a specific UDG
 *
 * @param[in]  c     Character code
 *
 * @return     Address of first character data.
 */
uint8_t *CONGetUDGGraphicData(int c) {
    static uint8_t _duffUDG[] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};         // Used for a bad graphic.
    if (c < FONT_FIRST_UDG || c > FONT_LAST_UDG) return _duffUDG;                   // Bad UDG return solid block graphic
    return udgData + (c - FONT_FIRST_UDG) * 8;                                      // Return offset to graphic
}

/**
 * @brief      Write string to debug console
 *
 * @param[in]  fmt        The format (as printf)
 * @param[in]  <unnamed>  Varags paramete
 */
void CONWriteString(const char *fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 128, fmt, args);
    uint8_t *p = (uint8_t *)buf;
    while (*p != '\0') CONWrite(*p++);
    va_end(args);
}