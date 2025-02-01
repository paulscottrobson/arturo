/**
* @file       scaling.c
*
* @brief      Scaling/Origin/Window graphics
*
* @author     Paul Robson
*
* @date       29/01/2025
*
*/

#include "common.h"

struct _GraphicWindow window;                                                       // Graphics window (note 0,0 is bottom left) in physical coordinates.

static int xOrigin = 0,yOrigin = 0;                                                 // Origin position, this is in logical coordinates.
static int xScale = 1,yScale = 1;                                                   // Scale (divide by this for logical -> physical)
static int xLogicalExtent,yLogicalExtent;                                           // The extent of the logical coordinates.
static int xLastLogical,yLastLogical;                                               // Last logical
static int gColMode,fgrGraphic,bgrGraphic;                                          // Graphic mode, foreground, background

static int xCoord[3],yCoord[3];                                                     // Coordinate buffer (PHYSICAL coordinates)

/**
 * @brief      Set the default graphics colours.
 */
void VDUSetDefaultGraphicColour(void) {
    VDUSetGraphicsColour(0,7);                                                      // GCOL 0,7
    VDUSetGraphicsWindow(0x80,0);                                                   // GCOL 128,0
}

/**
 * @brief      Set the graphic mode and colour (this is GCOL)
 *
 * @param[in]  mode    The mode
 * @param[in]  colour  The colour
 */

void VDUSetGraphicColour(int mode,int colour) {
    gColMode = mode;                                                                // Save mode. According to MOS1.2 this is the same mode for both
    if (colour >= 0x80) {                                                           // If bit 7 set, background
        bgrGraphic = colour & 0x7F;
    } else {                                                                        // If bit 7 clear, foreground
        fgrGraphic = colour & 0x7F;
    }
}

/**
 * @brief      Reset the graphics window
 */
void VDUResetGraphicsWindow(void) {
    VDUSetGraphicsWindow(0,0,9999,9999);                                            // This works because VDUSetGraphicsWindow clips.
}

void VDUSetGraphicsCursor(int x,int y) {
    // TODO: Set Graphics Cursor to x,y
}

/**
 * @brief      Set the graphics origin
 *
 * @param[in]  x     Logical x position of origin
 * @param[in]  y     Logical y position of origin
 */
void VDUSetGraphicsOrigin(int x,int y) {
    xOrigin = x;yOrigin = y;
}

/**
 * @brief      Set Graphics Window
 *
 * @param[in]  x1    Left X Logical
 * @param[in]  y1    Bottom Y Logical
 * @param[in]  x2    Right X Logical
 * @param[in]  y2    Top Y Logical
 */
void VDUSetGraphicsWindow(int x1,int y1,int x2,int y2) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();                       // Get the display information.

    xScale = (dmi->width == 640) ? 1 : 2;                                           // Currently we only have 640 and 320 across.
    yScale = (dmi->height == 480) ? 1 : 2;                                          // Support 480,240 and 256 down.

    xLogicalExtent = (dmi->width << xScale)-1;                                      // Work out the logical extent.   
    yLogicalExtent = (dmi->height << yScale)-1;

    x1 = max(0,x1);y1 = max(0,y1);                                                  // Clip window coordinates to logical extent.
    x2 = min(x2,xLogicalExtent);y2 = min(y2,yLogicalExtent);

    x1 = x1 >> xScale;y1 = y1 >> yScale;x2 = x2 >> xScale;y2 = y2 >> yScale;        // Convert to physical coordinates 

    window.xLeft = x1;window.yBottom = y1;                                          // Set the window
    window.xRight = x2;window.yTop = y2;                                  

    printf("Window:%d,%d - %d,%d\n",window.xLeft,window.yBottom,window.xRight,window.yTop);
}

/**
 * @brief      Do a Plot command
 *
 * @param[in]  cmd   Command ID
 * @param[in]  x     Logical X coordinate
 * @param[in]  y     Logical Y coordinate
 */
void VDUPlotCommand(int cmd,int x,int y) {

    if ((cmd & 4) == 0) {                                                           // Is it a relative movement.
        x += xLastLogical;y += yLastLogical;                                        // If so, offset from the last logical position
    }

    xLastLogical = x;yLastLogical = y;                                              // Update the last logical position.

    x += xOrigin; y += yOrigin;                                                     // Adjust for origin.

    xCoord[2] = xCoord[1];yCoord[2] = yCoord[1];                                    // Push x and y cordinates down the 3 level store
    xCoord[1] = xCoord[0];yCoord[1] = yCoord[0];
    xCoord[0] = x >> xScale;yCoord[0] = y >> yScale;                                // Add the latest coordinate.

    printf("Plot: %d %d,%d\n",cmd,xCoord[0],yCoord[0]);

    // TODO: Work out colours to use from FGR/INV/BGR none if not move and mode.
    // TODO: Set control bits.

    VDUPlotDispatch(cmd,xCoord,yCoord);                                             // Go do it
}