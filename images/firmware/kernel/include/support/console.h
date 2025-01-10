/**
 * @file       console.h
 *
 * @brief      Header file console debugging
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once
//
//      Standard colours RGB format, same as the BBC Micro.
//
#define CON_COL_BLACK 0x0
#define CON_COL_RED 0x1
#define CON_COL_GREEN 0x2
#define CON_COL_YELLOW 0x3
#define CON_COL_BLUE 0x4
#define CON_COL_MAGENTA 0x5
#define CON_COL_CYAN 0x6
#define CON_COL_WHITE 0x7
//
//      Range of UDG for 8x8 fonts only.
//
#define FONT_FIRST_UDG      (224)
#define FONT_LAST_UDG       (255)
//
//      Console output functionality.
//
void CONInitialise(void);
void CONWrite(int c);
void CONWriteString(const char *fmt, ...);
void CONDrawPixel(int x, int y, int rgb);
void CONSetColour(int foreground,int background);
void CONEnableConsole(bool isOn);
void CONDefineUDG(int udg,uint8_t * bitData);
uint8_t *CONGetUDGGraphicData(int udg);