/**
 * @file       ellipse.c
 *
 * @brief      Ellipse Drawing functions
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include <libraries.h>
#include "include/atomic.h"

static int rx,ry,xc,yc;

static void _GFXFramePart(int x,int y,int colour);
static void _GFXDrawEllipse(int colour, bool fill);
static void _GFXFrameEllipseMain(GFXPort *vp,int x0,int y0,int x1,int y1,int colour,bool fill);
static void _GFXDrawEllipseMain(GFXPort *vp,int x0,int y0,int x1,int y1,int colour,bool fill);


/**
 * @brief      Draw an outline ellipse
 *
 * @param      vp      Viewport or NULL
 * @param[in]  x0      The x0 coordinate
 * @param[in]  y0      The y0 coordinate
 * @param[in]  x1      The x1 coordinate
 * @param[in]  y1      The y1 coordinate
 * @param[in]  colour  Colour
 */
void GFXFrameEllipse(GFXPort *vp,int x0,int y0,int x1,int y1,int colour) {
    _GFXDrawEllipseMain(vp,x0,y0,x1,y1,colour,false);
}

/**
 * @brief      Draw a filled in ellipse.
 *
 * @param      vp      Viewport or NULL
 * @param[in]  x0      The x0 coordinate
 * @param[in]  y0      The y0 coordinate
 * @param[in]  x1      The x1 coordinate
 * @param[in]  y1      The y1 coordinate
 * @param[in]  colour  Colour
 */
void GFXFillEllipse(GFXPort *vp,int x0,int y0,int x1,int y1,int colour) {
    _GFXDrawEllipseMain(vp,x0,y0,x1,y1,colour,true);
}


/**
 * @brief      Run main draw ellipse
 *
 * @param      vp      Viewport or NULL
 * @param[in]  x0      The x0 coordinate
 * @param[in]  y0      The y0 coordinate
 * @param[in]  x1      The x1 coordinate
 * @param[in]  y1      The y1 coordinate
 * @param[in]  colour  Colour
 * @param[in]  fill    In fill ?
 */
static void _GFXDrawEllipseMain(GFXPort *vp,int x0,int y0,int x1,int y1,int colour,bool fill) {
    GFXASetPort(vp);
    rx = abs(x0-x1)/2;ry = abs(y0-y1)/2;
    xc = (x0+x1)/2;yc = (y0+y1)/2;
    _GFXDrawEllipse(colour,fill);
}

/**
 * @brief      Draw the frame part of the ellipse
 *
 * @param[in]  x       x Coordinate
 * @param[in]  y       y Coordinate
 * @param[in]  colour  colour
 */
static void _GFXFramePart(int x,int y,int colour) {
    GFXAPlot(xc+x,yc+y,colour);
    if (x != 0) {                                                               // If at 0 horizontal only do once
        GFXAPlot(xc-x,yc+y,colour);
    }
    if (y != 0) {                                                               // If at 0 vertical only do once.
        GFXAPlot(xc+x,yc-y,colour);
        GFXAPlot(xc-x,yc-y,colour);
    }
}

/**
 * @brief      Draw the line for the filled eillipse
 *
 * @param[in]  x       x Coordinate
 * @param[in]  y       y Coordinate
 * @param[in]  colour  colour
 */
static void _GFXLinePart(int x,int y,int colour) {
    GFXAHorizLine(xc-x,xc+x,yc+y,colour);
    if (yc != 0) {                                                              // Don't redraw the middle.
        GFXAHorizLine(xc-x,xc+x,yc-y,colour);
    }
}

/**
 * @brief      Midpoint Ellipse Algorithm
 *
 * @param[in]  colour  The colour to use
 * @param[in]  fill    True if solid fill
 */
static void _GFXDrawEllipse(int colour, bool fill) {
    float dx, dy, d1, d2, x, y;
    x = 0;y = ry;
    d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    while (dx < dy)
    {
        if (fill) {
            _GFXLinePart(x,y,colour);
        } else {
            _GFXFramePart(x,y,colour);
        }
        if (d1 < 0)
        {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        } else {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) +
         ((rx * rx) * ((y - 1) * (y - 1))) -
          (rx * rx * ry * ry);

    while (y >= 0)
    {
        if (fill) {
            _GFXLinePart(x,y,colour);
        } else {
            _GFXFramePart(x,y,colour);
        }
        if (d2 > 0)
        {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        } else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}

