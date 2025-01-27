/**
 * @file       mouse.c
 *
 * @brief      Mouse Pointer functions
 *
 * @author     Paul Robson
 *
 * @date       20/01/2025
 *
 */

#include "common.h"
#include <libraries.h>

static bool isMouseAvailable;                                                       // True if mouse available
static bool isPointerVisible;                                                       // True if the pointer is currently shown, whether drawn or not.
static bool isPointerDrawn;                                                         // True if the pointer is currently drawn on the display.

static struct _SmallBuffer pointerStore;                                            // Storage for background when mouse pointer drawn.

static int  xMouse,yMouse;                                                          // Current mouse position.

static void _GFXUpdateMousePointer(void);
static void _GFXRestoreScreen(void);

/**
 * @brief      Reset the mouse system
 */
void GFXMReset(void) {
    isPointerVisible = true;                                                        // Pointer visible
    isPointerDrawn = false;                                                         // Not currently drawn
    xMouse = yMouse = -99;                                                          // Invalid values.
    isMouseAvailable = MSEMousePresent();                                           // Track mouse available.
}

/**
 * @brief      Update mouse pointer position and screen image if visible. Call
 *             this in the 'main loop' to keep the mouse active.
 */
void GFXUpdateMouse(void) {
    int x,y;
    MSEGetState(&x,&y,NULL,NULL);                                                   // Get position
    if (x != xMouse || y != yMouse) {                                               // Position changed.
        xMouse = x;yMouse = y;                                                      // Update position.
        if (isPointerVisible) _GFXUpdateMousePointer();                             // Repaint if visible.
    }
}

/**
 * @brief      Erase old mouse pointer and draw new one
 */
static void _GFXUpdateMousePointer(void) {
    if (isPointerDrawn) _GFXRestoreScreen();                                        // If pointer drawn, restore screen.
    int scale = DVIGetModeInformation()->height / 240;                              // Vertical scale
    GFXCopyScreenToSmallBuffer(xMouse,yMouse,&pointerStore);                        // Save the background

    GFXFillTriangle(NULL,xMouse,yMouse,xMouse+2,yMouse+7*scale,xMouse+12,yMouse+4*scale,0);     
    GFXFrameTriangle(NULL,xMouse,yMouse,xMouse+2,yMouse+7*scale,xMouse+12,yMouse+4*scale,7);
    isPointerDrawn = true;                                                          // Set pointer drawn flag.
}

/**
 * @brief      Erase current mouse pointer
 */
static void _GFXRestoreScreen(void) {
    if (isPointerDrawn) GFXCopySmallBufferToScreen(&pointerStore);                  // Restore the display
    isPointerDrawn = false;                                                         // No longer drawn
}


/**
 * @brief      If mouse is in viewport and drawn, remove it as going to do some
 *             painting
 *
 * @param      vp    Viewport, or NULL.
 */
void GFXCheckMouse(GFXPort *vp) {
    if (!isPointerDrawn) return;                                                    // Is the pointer drawn, if not, no action.
    if (xMouse < vp->x-32 && yMouse < vp->y-32) return;
    if (xMouse > vp->x+vp->width+32 || yMouse > vp->y+vp->height+32) return;
    _GFXRestoreScreen();                                                            // Restore the mouse pointer.
    xMouse = yMouse = -99;                                                          // Force it's redraw
}