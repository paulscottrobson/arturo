/**
 * @file       vdu.h
 *
 * @brief      Header file for vdu driver
 *
 * @author     Paul Robson
 *
 * @date       27/01/2025
 *
 */

#pragma once

struct _GraphicWindow {
	int xLeft,yBottom,xRight,yTop;
};

extern struct _GraphicWindow window;

void VDUWrite(int c);
void VDUWriteString(const char *fmt, ...);
void VDUPlotCommand(int cmd,int x,int y);

// -------------------------------------------

void VDUCursor(int c);
void VDUWriteText(uint8_t c);
void VDUClearScreen(void);
void VDUSetTextCursor(int x,int y);
void VDUHomeCursor(void);

void VDUSetGraphicsCursor(int x,int y);
void VDUSetGraphicsOrigin(int x,int y);
uint8_t VDUGetCharacterLineData(int c,int y);
void VDUDefineCharacter(int c,uint8_t *gData);

void VDUSetDefaultTextColour(void);
void VDUSetTextColour(int colour);
void VDUResetTextWindow(void);   
void VDUSetTextWindow(int x1,int y1,int x2,int y2);
void VDUSetGraphicsWindow(int x1,int y1,int x2,int y2);
void VDUSetDefaultGraphicColour(void);
void VDUResetGraphicsWindow(void);
void VDUPlotDispatch(int cmd,int *xCoord,int *yCoord);                                           


void GFXASetControlBits(int c);
void VDUAPlot(int x,int y);
void VDUAHorizLine(int x1,int x2,int y);
void VDUAVertLine(int x,int y1,int y2);
void VDUALine(int x0, int y0, int x1, int y1);
void VDUAFillRect(int x0,int y0,int x1,int y1);
void VDUAFrameRect(int x0,int y0,int x1,int y1);
void VDUAFillEllipse(int x0,int y0,int x1,int y1);
void VDUAFrameEllipse(int x0,int y0,int x1,int y1);
void VDUAFillTriangle(int x0,int y0,int x1,int y1,int x2,int y2);
void VDUAFrameTriangle(int x0,int y0,int x1,int y1,int x2,int y2);

void VDUAUp(void);
void VDUADown(void);
void VDUALeft(void);
void VDUARight(void);

#define GFXC_NOENDPOINT 	(0x01)
#define GFXC_DOTTED 		(0x02)