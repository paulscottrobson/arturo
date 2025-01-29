/**
 * @file       atomic.c
 *
 * @brief      Lowest level graphics routines
 *
 * @author     Paul Robson
 *
 * @date       29/01/2025
 *
 */

#include "common.h"

static inline void _GFXDrawBitmap1(void);
static inline void _GFXDrawBitmap3(void);
static inline void _GFXDrawBitmap6(void);
static inline void _GFXDrawBitmap(void);

static void _GFXAValidate(void);

static struct DVIModeInformation *_dmi = NULL;                                      // Current mode information.
static int xPixel,yPixel;                                                           // Pixel position in current window
static bool dataValid;                                                              // True if data is valid.
static uint8_t bitMask;                                                             // Bitmask (when data is valid)
static uint8_t *pl0,*pl1,*pl2;                                                      // Bitplane pointers.
static uint8_t colour = 7;                                                          // Drawing colour
static uint8_t action = 0;                                                          // What to do.

#define OFFWINDOWH(x)   ((x) < window.xLeft || (x) > window.xRight)
#define OFFWINDOWV(y)   ((y) < window.yBottom || (y) > window.yTop)
#define OFFWINDOW(x,y)  (OFFWINDOWH(x) || OFFWINDOWV(y))

/**
 * @brief      Validate it (e.g. check on screen and in viewport)
 */
static void _GFXAValidate(void) {
    dataValid = false;
    if (OFFWINDOW(xPixel,yPixel)) return;                                           // No, we can't do anything.

    int offset;
    if (_dmi->bitPlaneDepth == 2) {
            offset = (xPixel >> 2) + ((_dmi->height-1-yPixel) * _dmi->bytesPerLine);// Work out the offset on the bitmap planes.
            bitMask = (0xC0 >> (2*(xPixel & 3)));                                   // Work out the bitmask for the current pixel.
    } else {
            offset = (xPixel >> 3) + ((_dmi->height-1-yPixel) * _dmi->bytesPerLine);// Work out the offset on the bitmap planes.
            bitMask = (0x80 >> (xPixel & 7));                                       // Work out the bitmask for the current pixel.
    }
    pl0 = _dmi->bitPlane[0]+offset;                                                 // Set up bitmap plane pointers.
    pl1 = _dmi->bitPlane[1]+offset;
    pl2 = _dmi->bitPlane[2]+offset;

    dataValid = true;                                                               // We have valid data
}

/**
 * @brief      Plot pixel in current viewport
 *
 * @param[in]  x       x coordinate
 * @param[in]  y       y coordinate
 */
void GFXAPlot(int x,int y) {
    _dmi = DVIGetModeInformation();                                                 // Get mode information
    xPixel = x;yPixel = y;                                                          // Update the pixel positions.
    _GFXAValidate();                                                                // Validate the position.
    if (dataValid) _GFXDrawBitmap();                                                // Draw pixel if valid.
}


/**
 * @brief      Draw horizontal line
 *
 * @param[in]  x1      The x1 coordinate
 * @param[in]  x2      The x2 coordinate
 * @param[in]  y       Y coordinate
 */
void GFXAHorizLine(int x1,int x2,int y) {
    _dmi = DVIGetModeInformation();                                                 // Get mode information
    int ppb = _dmi->bitPlaneDepth==2 ? 4 : 8;
    if (OFFWINDOWV(y)) return;                                                      // Vertically out of range => no line.
    if (x1 >= x2) { int n = x1;x1 = x2;x2 = n; }                                    // Sort the x coordinates into order.
    if (x2 < window.xLeft || x1 >= window.xRight) return;                           // On screen area (e.g. lower off right, higher off left)
    x1 = max(x1,window.xLeft);x2 = min(x2,window.xRight);                           // Trim horizontal line to port.
    xPixel = x1;yPixel = y;dataValid = false;                                       // First pixel.
    _GFXAValidate();
    int pixelCount = x2-x1+1;                                                       // Pixels to draw

    //
    //      First, we go to a byte boundary, if there are enough pixels.
    //
    while (pixelCount > 0 && (bitMask & 0x80) == 0) {                               // Shift until reached byte boundary
        _GFXDrawBitmap();
        GFXARight();
        pixelCount--;
    }
    //
    //      While on a byte boundary, if there are enough pixels, do whole bytes. I did consider doing it in longs at this point.
    //
    while (pixelCount >= ppb) {                                                     // Now do it byte chunks.
        bitMask = 0xFF;_GFXDrawBitmap();                                            // This does the line in whole bytes.
        pl0++;pl1++;pl2++;                                                          // Advance pointer
        pixelCount -= ppb;                                                          // 8 fewer pixels
        xPixel += ppb;                                                              // Keep the position up to date, doesn't really matter.
    }
    //
    //      Do any remaining single pixels
    //
    bitMask = _dmi->bitPlaneDepth == 2? 0xC0 : 0x80;                                // We know we are on a byte boundary
    while (pixelCount-- > 0) {                                                      // Draw any remaining pixels.
        _GFXDrawBitmap();
        GFXARight();
    }
}

/**
 * @brief      Vertical line
 *
 * @param[in]  x       X coordinate
 * @param[in]  y1      y1 coordinate
 * @param[in]  y2      y2 coordinate
 * @param[in]  colour  colour
 */
void GFXAVertLine(int x,int y1,int y2) {
    _dmi = DVIGetModeInformation();                                                 // Get mode information
    if (OFFWINDOWH(x)) return;                                                      // Off screen.
    if (y1 > y2) { int n = y1;y1 = y2;y2 = n; }                                     // Sort y coordinates
    if (y2 < window.yBottom || y1 >= window.yTop) return;                           // Wholly off top or bottom.
    y1 = max(y1,window.yBottom);y2 = min(y2,window.yTop);                           // Clip into region.
    xPixel = x;yPixel = y1;dataValid = false;                                       // Set start and validate
    _GFXAValidate();
    int pixelCount = y2-y1+1;                                                       // Pixels to draw
    while (pixelCount-- > 0) {                                                      // Shift until reached byte boundary
        _GFXDrawBitmap();GFXAUp();
    }
}

/**
 * @brief      Move current up
 */
void GFXAUp(void) {
    yPixel++;                                                                       // Pixel up
    pl0 -= _dmi->bytesPerLine;                                                      // Shift pointers to next line up.
    pl1 -= _dmi->bytesPerLine;
    pl2 -= _dmi->bytesPerLine;
    if (dataValid) dataValid = (yPixel <= window.yTop);                             // Still in window
}

/**
 * @brief      Move current down
 */
void GFXADown(void) {
    yPixel--;                                                                       // Pixel down
    pl0 += _dmi->bytesPerLine;                                                      // Shift pointers to next line down
    pl1 += _dmi->bytesPerLine;
    pl2 += _dmi->bytesPerLine;
    if (dataValid) dataValid = (yPixel >= window.yBottom);                          // Still in window
}

/**
 * @brief      Move current left
 */
void GFXALeft(void) {
    xPixel--;                                                                       // Pixel left
    if (_dmi->bitPlaneDepth == 2) {
      bitMask = (bitMask << 2) & 0xFF;                                              // Shift bitmap left
      if (bitMask == 0) {                                                           // Off the left side.
        bitMask = 0x03;                                                             // Reset bitmap
        pl0--;pl1--;pl2--;                                                          // Bump plane pointers
      }
    } else {

      bitMask = (bitMask << 1) & 0xFF;                                              // Shift bitmap left
      if (bitMask == 0) {                                                           // Off the left side.
        bitMask = 0x01;                                                             // Reset bitmap
        pl0--;pl1--;pl2--;                                                          // Bump plane pointers
      }
    }
    if (dataValid) dataValid = (xPixel >= 0);                                       // Still in window
}

/**
 * @brief      Move current right
 */
void GFXARight(void) {
    xPixel++;                                                                       // Pixel right
    if (_dmi->bitPlaneDepth == 2) {
      bitMask >>= 2;                                                                // Shift bitmap right
      if (bitMask == 0) {                                                           // Off the right side.
        bitMask = 0xC0;                                                             // Reset bitmap
        pl0++;pl1++;pl2++;                                                          // Bump plane pointers
      }
    } else {
      bitMask >>= 1;                                                                // Shift bitmap right
      if (bitMask == 0) {                                                           // Off the right side.
        bitMask = 0x80;                                                             // Reset bitmap
        pl0++;pl1++;pl2++;                                                          // Bump plane pointers
      }
    }
    if (dataValid) dataValid = (xPixel < window.xRight);                            // Still in window
}

/**
 * @brief      Line drawing (simple Bresenham)
 *
 * @param[in]  x0      The x0 coordinate
 * @param[in]  y0      The y0 coordinate
 * @param[in]  x1      The x1 coordinate
 * @param[in]  y1      The y1 coordinate
 * @param[in]  colour  The colour
 */
void GFXALine(int x0, int y0, int x1, int y1) {
    if (y0 == y1) {                                                                 // Use the horizontal one.
        GFXAHorizLine(x0,x1,y1);
        return;
    }
    if (x0 == x1) {                                                                 // Use the vertical one.
         GFXAVertLine(x0,y0,y1);
         return;
    }

    _dmi = DVIGetModeInformation();                                                 // Get mode information

    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx + dy;

    xPixel = x0;yPixel = y0;                                                        // Start at x0,y0

    _GFXAValidate();                                                                // Validate the current
    while(1) {
        if (!dataValid) _GFXAValidate();                                            // Try to validate if invaluid
        if (dataValid) {
            _GFXDrawBitmap();                                                       // If valid, then draw line.
        }
        if (xPixel == x1 && yPixel == y1) return;                                   // Completed line.

        int e2 = 2 * error;
        if (e2 >= dy) {
            error = error + dy;
            if (sx < 0) { GFXALeft(); } else { GFXARight(); }
        }
        if (e2 <= dx) {
            error = error + dx;
            if (sy < 0) { GFXADown(); } else { GFXAUp();}
        }
    }
}

/**
 * @brief      Draw bitmap dispatched
 */
static inline void _GFXDrawBitmap(void) {
    if (!dataValid) return;                                                         // Not valid drawing.
    if (_dmi->bitPlaneCount == 1) {                                                 // Draw the bitmap.
        _GFXDrawBitmap1();
    } else if (_dmi->bitPlaneDepth == 2) {
        _GFXDrawBitmap6();
    } else {
        _GFXDrawBitmap3();
    }
}

/**
 * @brief      Draw bitmap for 3 plane
 */
static inline void _GFXDrawBitmap3(void) {

    switch(action) {

        case 0:                                                                     // Standard draw
            *pl0 = ((*pl0) & (~bitMask)) | ((colour & 1) ? bitMask:0);
            *pl1 = ((*pl1) & (~bitMask)) | ((colour & 2) ? bitMask:0);
            *pl2 = ((*pl2) & (~bitMask)) | ((colour & 4) ? bitMask:0);
            break;

        case 1:                                                                     // XOR Draw
            *pl0 ^= ((colour & 1) ? bitMask:0);
            *pl1 ^= ((colour & 2) ? bitMask:0);
            *pl2 ^= ((colour & 4) ? bitMask:0);
            break;
    }
}

/**
 * @brief      Draw bitmap for a 1 plane monochrome
 */
static inline void _GFXDrawBitmap1(void) {

    switch(action) {

        case 0:                                                                     // Standard draw
            *pl0 = ((*pl0) & (~bitMask)) | ((colour & 1) ? bitMask:0);
            break;

        case 1:                                                                     // XOR Draw
            *pl0 ^= (colour & 1) ? bitMask:0;
            break;
    }
}


/**
 * @brief      Draw bitmap for a 1 plane 6 bit colour display
 */
static inline void _GFXDrawBitmap6(void) {


    switch(action) {

        case 0:                                                                     // Standard draw
            *pl0 =  ((*pl0) & (~bitMask)) | ((colour & 1) ? bitMask & 0xAA : 0) |
                    ((colour & 8)  ? bitMask & 0x55:0);
            *pl1 =  ((*pl1) & (~bitMask)) | ((colour & 2) ? bitMask & 0xAA : 0) |
                    ((colour & 16) ? bitMask & 0x55 :0);
            *pl2 =  ((*pl2) & (~bitMask)) | ((colour & 4) ? bitMask & 0xAA : 0) |
                    ((colour & 32) ? bitMask & 0x55 :0);
             break;

        case 1:                                                                     // XOR Draw
            *pl0 ^= (((colour & 1) ? bitMask & 0xAA:0) | ((colour & 8) ? bitMask & 0x55:0));
            *pl1 ^= (((colour & 2) ? bitMask & 0xAA:0) | ((colour & 16) ? bitMask & 0x55:0));
            *pl2 ^= (((colour & 4) ? bitMask & 0xAA:0) | ((colour & 32) ? bitMask & 0x55:0));
            break;
    }

}
