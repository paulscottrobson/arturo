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

void VDUWrite(int c);
void VDUWriteString(const char *fmt, ...);

void VDUCursor(int c);
void VDUWriteText(char c);
void VDUClearScreen(void);
void VDUSetTextCursor(int x,int y);
void VDUHomeCursor(void);

void VDUSetGraphicsCursor(int x,int y);
void VDUSetGraphicsOrigin(int x,int y);
uint8_t VDUGetCharacterLineData(int c,int y);

void VDUSetDefaultTextColour(void);
void VDUSetTextColour(int colour);
void VDUResetTextWindow(void);   
void VDUSetTextWindow(int x1,int y1,int x2,int y2);
void VDUSetDefaultGraphicColour(void);
void VDUResetGraphicsWindow(void);
