/**
 * @file   drawing.c
 *
 * @brief      Drawing functions
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include <libraries.h>
#include "include/atomic.h"

/**
 * @brief      Draw a framed rectangle
 *
 * @param      vp      Viewport or NULL
 * @param[in]  x0      The x0 coordinate
 * @param[in]  y0      The y0 coordinate
 * @param[in]  x1      The x1 coordinate
 * @param[in]  y1      The y1 coordinate
 * @param[in]  colour  The colour
 */
void GFXFrameRect(GFXPort *vp,int x0,int y0,int x1,int y1,int colour) {
    GFXASetPort(vp);
    if (y0 > y1) { int n = y0;y0 = y1;y1 = n; }                                     // Sort horizontally.
    GFXAHorizLine(x0,x1,y0,colour);                                                 // Top line.
    if (y1 != y0) GFXAHorizLine(x0,x1,y1,colour);                                   // Bottom line, if not a one line frame.
    for (int y = y0+1;y < y1;y++) {
        GFXAPlot(x0,y,colour);
        if (x0 != x1) GFXAPlot(x1,y,colour);
    }
}


/**
 * @brief      Draw a solid rectangle
 *
 * @param      vp      Viewport or NULL
 * @param[in]  x0      The x0 coordinate
 * @param[in]  y0      The y0 coordinate
 * @param[in]  x1      The x1 coordinate
 * @param[in]  y1      The y1 coordinate
 * @param[in]  colour  The colour
 */
void GFXFillRect(GFXPort *vp,int x0,int y0,int x1,int y1,int colour) {
    GFXASetPort(vp);
    for (int y = y0;y <= y1;y++) {
        GFXAHorizLine(x0,x1,y,colour);
    }
}

/**
 * @brief      Plot a point
 *
 * @param      vp      Viewport or NULL
 * @param[in]  x       x Point
 * @param[in]  y       y Point
 * @param[in]  colour  The colour
 */
void GFXPlot(GFXPort *vp,int x,int y,int colour) {
    GFXASetPort(vp);
    GFXAPlot(x,y,colour);
}

/**
 * @brief      Draw a line
 *
 * @param      vp      Viewport or NULL
 * @param[in]  x0      The x0 coordinate
 * @param[in]  y0      The y0 coordinate
 * @param[in]  x1      The x1 coordinate
 * @param[in]  y1      The y1 coordinate
 * @param[in]  colour  The colour
 */
void GFXLine(GFXPort *vp,int x0, int y0, int x1, int y1,int colour) {
    GFXASetPort(vp);
    GFXALine(x0,y0,x1,y1,colour);
}

/**
 * @brief      Draw a horizontal line
 *
 * @param      vp      Viewport or NULL
 * @param[in]  x0      The x0 coordinate
 * @param[in]  y0      The y0 coordinate
 * @param[in]  x1      The x1 coordinate
 * @param[in]  y1      The y1 coordinate
 * @param[in]  colour  The colour
 */

void GFXHLine(GFXPort *vp,int x0, int x1, int y,int colour) {
    GFXASetPort(vp);
    GFXAHorizLine(x0,x1,y,colour);
}

/**
 * @brief      Draw a vertictal line
 *
 * @param      vp      Viewport or NULL
 * @param[in]  x0      The x0 coordinate
 * @param[in]  y0      The y0 coordinate
 * @param[in]  x1      The x1 coordinate
 * @param[in]  y1      The y1 coordinate
 * @param[in]  colour  The colour
 */
void GFXVLine(GFXPort *vp,int x, int y0, int y1,int colour) {
    GFXASetPort(vp);
    GFXAHorizLine(x,y0,y1,colour);
}
