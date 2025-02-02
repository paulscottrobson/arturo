/**
* @file       plot.c
*
* @brief      Plot Dispatcher
*
* @author     Paul Robson
*
* @date       29/01/2025
*
*/

#include "common.h"

/**
 * @brief      Dispatch PLOT command
 *
 * @param[in]  cmd     Command (lower 3 bits stripped)
 * @param      xCoord  Array of x coordinates, [0] most recent
 * @param      yCoord  Array of y coordinates, [0] most recent
 */
void VDUPlotDispatch(int cmd,int *xCoord,int *yCoord) {
	printf("%d %d,%d %d,%d %d,%d\n",cmd,xCoord[0],yCoord[0],xCoord[1],yCoord[1],xCoord[2],yCoord[2]);
	switch(cmd) {
		case 0:  																	// 0-31 Line drawers. Variants are set previously.
		case 8:
		case 16:
		case 24:
			VDUALine(xCoord[1],yCoord[1],xCoord[0],yCoord[0]);
			break;

		case 64:																	// 64-71 plot point
			VDUAPlot(xCoord[0],yCoord[0]);
			break;

		case 80:  																	// 80-87 filled triangle.
			VDUAFillTriangle(xCoord[2],yCoord[2],xCoord[1],yCoord[1],xCoord[0],yCoord[0]);
			break;
	}
}
