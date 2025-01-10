/**
 * @file       atomic.c
 *
 * @brief      Lowest level graphics routines
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#include "common.h"
#include <libraries.h>
#include "include/atomic.h"

static inline void _GFXDrawBitmap1(int colour);
static inline void _GFXDrawBitmap3(int colour);
static inline void _GFXDrawBitmap6(int colour);
static inline void _GFXDrawBitmap(int colour);

static void _GFXAValidate(void);

//
//      These functions are atomic. They assume once you've selected the viewport
//      the mode, port will not change for the duration of any function that uses them
//      e.g. to draw rectangles you set the viewport and draw multiple horizontal lines
//      that sort of thing :)
//

static GFXPort *_currentPort = NULL;                                                // Current viewport
static struct DVIModeInformation *_dmi = NULL;                                      // Current mode information.
static int xPixel,yPixel;                                                           // Pixel position in current window
static bool dataValid;                                                              // True if data is valid.
static uint8_t bitMask;                                                             // Bitmask (when data is valid)
static uint8_t *pl0,*pl1,*pl2;                                                      // Bitplane pointers.
static int width,height;                                                            // Size

#define CONV_X(x)       ((x)+(_currentPort->xOffset))
#define CONV_Y(y)       ((y)+(_currentPort->yOffset))
#define OFFWINDOWH(x)   ((x) < 0 || (x) >= width)
#define OFFWINDOWV(y)   ((y) < 0 || (y) >= height)
#define OFFWINDOW(x,y)  (OFFWINDOWH(x) || OFFWINDOWV(y))

static GFXPort _fullScreen;                                                         // Used for full screen viewport

/**
 * @brief      Set the current mode, clear the screen
 *
 * @param[in]  mode  Screen mode
 */
void GFXSetMode(int mode) {
    DVISetMode(mode);                                                               // Set the mode.
    _dmi = DVIGetModeInformation();                                                 // Get mode information
    for (int i = 0;i < _dmi->bitPlaneCount;i++) {                                   // Clear all the bit planes.
        memset(_dmi->bitPlane[i],0,_dmi->bytesPerLine*_dmi->height);
    }
}

/**
 * @brief      Set the current working viewport
 *
 * @param      vp    Viewport or NULL
 */
void GFXASetPort(GFXPort *vp) {
    _currentPort = vp;                                                              // Save viewport
    _dmi = DVIGetModeInformation();                                                 // Record current information
    if (_currentPort == NULL) {                                                     // NULL implies whole screen.
        GFXPortInitialise(&_fullScreen,0,0,_dmi->width-1,_dmi->height-1);
        _currentPort = &_fullScreen;
    }
    width = _currentPort->width;height = _currentPort->height;                      // Temporary variables.
    dataValid = false;                                                              // Nothing is set up.
}

/**
 * @brief      Plot pixel in current viewport
 *
 * @param[in]  x       x coordinate
 * @param[in]  y       y coordinate
 * @param[in]  colour  The colour
 */
void GFXAPlot(int x,int y,int colour) {
    xPixel = CONV_X(x);yPixel = CONV_Y(y);                                          // Update the pixel positions.
    _GFXAValidate();                                                                // Validate the position.
    if (dataValid) _GFXDrawBitmap(colour);                                          // Draw pixel if valid.
}


/**
 * @brief      Validate it (e.g. check on screen and in viewport)
 */
static void _GFXAValidate(void) {
    dataValid = false;
    if (OFFWINDOW(xPixel,yPixel)) return;                                           // No, we can't do anything.
    int xc = xPixel+_currentPort->x;                                                // Physical position on screen
    int yc = yPixel+_currentPort->y;
    int offset;
    if (_dmi->bitPlaneDepth == 2) {
            offset = (xc >> 2) + (yc * _dmi->bytesPerLine);                         // Work out the offset on the bitmap planes.
            bitMask = (0xC0 >> (2*(xc & 3)));                                       // Work out the bitmask for the current pixel.
    } else {
            offset = (xc >> 3) + (yc * _dmi->bytesPerLine);                         // Work out the offset on the bitmap planes.
            bitMask = (0x80 >> (xc & 7));                                           // Work out the bitmask for the current pixel.
    }
    pl0 = _dmi->bitPlane[0]+offset;                                                 // Set up bitmap plane pointers.
    pl1 = _dmi->bitPlane[1]+offset;
    pl2 = _dmi->bitPlane[2]+offset;

    dataValid = true;                                                               // We have valid data
}


/**
 * @brief      Draw horizontal line
 *
 * @param[in]  x1      The x1 coordinate
 * @param[in]  x2      The x2 coordinate
 * @param[in]  y       Y coordinate
 * @param[in]  colour  The colour
 */
void GFXAHorizLine(int x1,int x2,int y,int colour) {
    int ppb = _dmi->bitPlaneDepth==2 ? 4 : 8;
    x1 = CONV_X(x1);x2 = CONV_X(x2);y = CONV_Y(y);                                  // Convert to physical pixels in window.
    if (OFFWINDOWV(y)) return;                                                      // Vertically out of range => no line.
    if (x1 >= x2) { int n = x1;x1 = x2;x2 = n; }                                    // Sort the x coordinates into order.
    if (x2 < 0 || x1 >= width) return;                                              // On screen area (e.g. lower off right, higher off left)
    x1 = max(x1,0);x2 = min(x2,width-1);                                            // Trim horizontal line to port.
    xPixel = x1;yPixel = y;dataValid = false;                                       // First pixel.
    _GFXAValidate();
    int pixelCount = x2-x1+1;                                                       // Pixels to draw

    //
    //      First, we go to a byte boundary, if there are enough pixels.
    //
    while (pixelCount > 0 && (bitMask & 0x80) == 0) {                               // Shift until reached byte boundary
        _GFXDrawBitmap(colour);
        GFXARight();
        pixelCount--;
    }
    //
    //      While on a byte boundary, if there are enough pixels, do whole bytes. I did consider doing it in longs at this point.
    //
    while (pixelCount >= ppb) {                                                     // Now do it byte chunks.
        bitMask = 0xFF;_GFXDrawBitmap(colour);                                      // This does the line in whole bytes.
        pl0++;pl1++;pl2++;                                                          // Advance pointer
        pixelCount -= ppb;                                                          // 8 fewer pixels
        xPixel += ppb;                                                              // Keep the position up to date, doesn't really matter.
    }
    //
    //      Do any remaining single pixels
    //
    bitMask = _dmi->bitPlaneDepth == 2? 0xC0 : 0x80;                                // We know we are on a byte boundary
    while (pixelCount-- > 0) {                                                      // Draw any remaining pixels.
        _GFXDrawBitmap(colour);
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
void GFXAVertLine(int x,int y1,int y2,int colour) {
    x = CONV_X(x);y1 = CONV_Y(y1);y2 = CONV_Y(y2);                                  // Convert coordinates.
    if (OFFWINDOWH(x)) return;                                                      // Off screen.
    if (y1 > y2) { int n = y1;y1 = y2;y2 = n; }                                     // Sort y coordinates
    if (y2 < 0 || y1 >= height) return;                                             // Wholly off top or bottom.
    y1 = max(y1,0);y2 = max(y2,height-1);                                           // Clip into region.
    xPixel = x;yPixel = y1;dataValid = false;                                       // Set start and validate
    _GFXAValidate();
    int pixelCount = y2-y1+1;                                                       // Pixels to draw
    while (pixelCount-- > 0) {                                                      // Shift until reached byte boundary
        _GFXDrawBitmap(colour);GFXADown();
    }
}


/**
 * @brief      Move current up
 */
void GFXAUp(void) {
    yPixel--;                                                                       // Pixel up
    pl0 -= _dmi->bytesPerLine;                                                      // Shift pointers to next line up.
    pl1 -= _dmi->bytesPerLine;
    pl2 -= _dmi->bytesPerLine;
    if (dataValid) dataValid = (yPixel >= 0);                                       // Still in window
}

/**
 * @brief      Move current down
 */
void GFXADown(void) {
    yPixel++;                                                                       // Pixel down
    pl0 += _dmi->bytesPerLine;                                                      // Shift pointers to next line down
    pl1 += _dmi->bytesPerLine;
    pl2 += _dmi->bytesPerLine;
    if (dataValid) dataValid = (yPixel < height);                                   // Still in window
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
    if (dataValid) dataValid = (xPixel < width);                                    // Still in window
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
void GFXALine(int x0, int y0, int x1, int y1,int colour) {
    if (y0 == y1) {                                                                 // Use the horizontal one.
        GFXAHorizLine(x0,x1,y1,colour);
        return;
    }
    if (x0 == x1) {                                                                 // Use the vertical one.
         GFXAVertLine(x0,y0,y1,colour);
         return;
    }

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
            _GFXDrawBitmap(colour);                                                 // If valid, then draw line.
        }
        if (xPixel == x1 && yPixel == y1) return;                                   // Completed line.

        int e2 = 2 * error;
        if (e2 >= dy) {
            error = error + dy;
            if (sx < 0) { GFXALeft(); } else { GFXARight(); }
        }
        if (e2 <= dx) {
            error = error + dx;
            if (sy < 0) { GFXAUp(); } else { GFXADown();}
        }
    }
}


/**
 * @brief      Draw bitmap dispatched
 *
 * @param[in]  colour  colour to use
 */
static inline void _GFXDrawBitmap(int colour) {
    if (!dataValid) return;                                                         // Not valid drawing.
    if (_dmi->bitPlaneCount == 1) {                                                 // Draw the bitmap.
        _GFXDrawBitmap1(colour);
    } else if (_dmi->bitPlaneDepth == 2) {
        _GFXDrawBitmap6(colour);
    } else {
        _GFXDrawBitmap3(colour);
    }
}


/**
 * @brief      Draw bitmap for 3 plane
 *
 * @param[in]  colour  The colour
 */
static inline void _GFXDrawBitmap3(int colour) {

    switch(GFXACTION(colour)) {

        case GFXA_NORMAL:                                                           // Standard draw
            *pl0 = ((*pl0) & (~bitMask)) | ((colour & 1) ? bitMask:0);
            *pl1 = ((*pl1) & (~bitMask)) | ((colour & 2) ? bitMask:0);
            *pl2 = ((*pl2) & (~bitMask)) | ((colour & 4) ? bitMask:0);
            break;

        case GFXA_XOR:                                                              // XOR Draw
            *pl0 ^= ((colour & 1) ? bitMask:0);
            *pl1 ^= ((colour & 2) ? bitMask:0);
            *pl2 ^= ((colour & 4) ? bitMask:0);
            break;
    }
}


/**
 * @brief      Draw bitmap for a 1 plane monochrome
 *
 * @param[in]  colour  The colour
 */
static inline void _GFXDrawBitmap1(int colour) {

    switch(GFXACTION(colour)) {

        case GFXA_NORMAL:                                                           // Standard draw
            *pl0 = ((*pl0) & (~bitMask)) | ((colour & 1) ? bitMask:0);
            break;

        case GFXA_XOR:                                                              // XOR Draw
            *pl0 ^= (colour & 1) ? bitMask:0;
            break;
    }
}


/**
 * @brief      Draw bitmap for a 1 plane 6 bit colour display
 *
 * @param[in]  colour  The colour
 */
static inline void _GFXDrawBitmap6(int colour) {


    switch(GFXACTION(colour)) {

        case GFXA_NORMAL:                                                           // Standard draw
            *pl0 =  ((*pl0) & (~bitMask)) | ((colour & 1) ? bitMask & 0xAA : 0) |
                    ((colour & 8)  ? bitMask & 0x55:0);
            *pl1 =  ((*pl1) & (~bitMask)) | ((colour & 2) ? bitMask & 0xAA : 0) |
                    ((colour & 16) ? bitMask & 0x55 :0);
            *pl2 =  ((*pl2) & (~bitMask)) | ((colour & 4) ? bitMask & 0xAA : 0) |
                    ((colour & 32) ? bitMask & 0x55 :0);
             break;

        case GFXA_XOR:                                                              // XOR Draw
            *pl0 ^= (((colour & 1) ? bitMask & 0xAA:0) | ((colour & 8) ? bitMask & 0x55:0));
            *pl1 ^= (((colour & 2) ? bitMask & 0xAA:0) | ((colour & 16) ? bitMask & 0x55:0));
            *pl2 ^= (((colour & 4) ? bitMask & 0xAA:0) | ((colour & 32) ? bitMask & 0x55:0));
            break;
    }

}

/**
 * @brief      Save part of the screen to the buffer object
 *
 * @param[in]  xPos    x coordinate (raw pixels)
 * @param[in]  yPos    y coordinate (raw pixels)
 * @param      buffer  buffer storage object
 */
void GFXCopyScreenToSmallBuffer(int xPos,int yPos,struct _SmallBuffer *buffer) {
    if (xPos < 0 || yPos < 0 || xPos >= _dmi->width || yPos >= _dmi->height) {      // If off screen
        buffer->storePosition = -1;                                                 // e.g. no saved screen
        return;
    }
    buffer->storePosition = (xPos >> 3) + _dmi->bytesPerLine * yPos;                // Position on screen.
    buffer->ySize = min(16,_dmi->height-yPos);                                      // How many lines to take.

    int pixelsPerLine = min(24,_dmi->width-yPos);                                   // How many pixels do we need to store horizontally to ensure 16 pixels
    buffer->xBytesPerLine = (pixelsPerLine + 7) >> 3;                               // How many bytes do we store (includes incomplete)
    if (_dmi->bitPlaneDepth == 2) buffer->xBytesPerLine *= 2;                       // Double for 64 colour mode (e.g. more pixels)()
    for (int p = 0;p < _dmi->bitPlaneCount;p++) {                                   // Set up pointers to storage for each plane
        buffer->storage[p] = buffer->storageMemory + (16 * 3 * p);                  // If 64 colour mode we have plenty of storage.
    }
    for (int y = 0;y < buffer->ySize;y++) {                                         // Copy data out of bitplanes into storage.
        for (int x = 0;x < buffer->xBytesPerLine;x++) {
            for (int c = 0;c < _dmi->bitPlaneCount;c++) {
                buffer->storage[c][x + y * buffer->xBytesPerLine] = *(_dmi->bitPlane[c] + buffer->storePosition + x + y * _dmi->bytesPerLine);
            }
        }
    }
    //printf("%d %d %d\n",buffer->storePosition,buffer->xBytesPerLine,buffer->ySize);
}

/**
 * @brief      Copy small buffer back to screen
 *
 * @param      buffer  Small buffer structure.
 */
void GFXCopySmallBufferToScreen(struct _SmallBuffer *buffer) {
    if (buffer->storePosition < 0) return;                                          // Nothing saved
    for (int y = 0;y < buffer->ySize;y++) {                                         // Copy data out of storage to bitmap planes
        for (int x = 0;x < buffer->xBytesPerLine;x++) {
            for (int c = 0;c < _dmi->bitPlaneCount;c++) {
            *(_dmi->bitPlane[c] + buffer->storePosition + x + y * _dmi->bytesPerLine) = buffer->storage[c][x + y * buffer->xBytesPerLine];
            }
        }
    }
    
}
