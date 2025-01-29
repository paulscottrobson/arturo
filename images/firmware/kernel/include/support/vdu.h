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
void VDUPlotCommand(int cmd,int x,int y);
void VDUPlotDispatch(int cmd,int *xCoord,int *yCoord);                                           


void GFXAPlot(int x,int y);
void GFXAHorizLine(int x1,int x2,int y);
void GFXAVertLine(int x,int y1,int y2);
void GFXALine(int x0, int y0, int x1, int y1);

void GFXAUp(void);
void GFXADown(void);
void GFXALeft(void);
void GFXARight(void);
