/**
 * @file   port.c
 *
 * @brief      Graphic Port functions
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"
#include <libraries.h>

/**
 * @brief      Initialise a viewport
 *
 * @param      vp    Viewport to initialise
 * @param[in]  x0    The x0 coordinate
 * @param[in]  y0    The y0 coordinate
 * @param[in]  x1    The x1 coordinate
 * @param[in]  y1    The y1 coordinate
 */
void GFXPortInitialise(GFXPort *vp,int x0,int y0,int x1,int y1) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();
    vp->x = min(dmi->width-1,max(0,min(x0,x1)));                                            // Assign position and size forcing into current mode.
    vp->y = min(dmi->height-1,max(0,min(y0,y1)));
    vp->width = min(dmi->width-vp->x,abs(x1-x0));
    vp->height = min(dmi->height-vp->y,abs(y1-y0));
    vp->xOffset = vp->yOffset = 0;
}

/**
 * @brief      Scroll viewport to given offset.
 *
 *             This doesn't actually change anything, but subsequent draws
 *             affect this.
 *
 * @param      vp    Viewport
 * @param[in]  xo    x Scrolling
 * @param[in]  yo    y Scrolling
 */
void GFXScrollPort(GFXPort *vp,int xo,int yo) {
    vp->xOffset = xo;vp->yOffset = yo;
}

