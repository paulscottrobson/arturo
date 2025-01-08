// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      atomic.h
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      29th December 2024
//      Reviewed :  No
//      Purpose :   Graphics core header (atomic)
//
// ***************************************************************************************
// ***************************************************************************************

#pragma once

#define XXX "Hello"

void GFXASetPort(GFXPort *vp);
void GFXAPlot(int x,int y,int colour);
void GFXAHorizLine(int x1,int x2,int y,int colour);
void GFXAVertLine(int x,int y1,int y2,int colour);
void GFXALine(int x0, int y0, int x1, int y1,int colour);

void GFXAUp(void);
void GFXADown(void);
void GFXALeft(void);
void GFXARight(void);

//
//		Colour bytes
//
//		bit 0-7 	foreground colour
//		bit 8-15 	background colour
//		bit 16-19 	action
//
//						0  		draw in foreground
//						1  		xor with foreground
//						2..14 	reserved
// 						15 		draw in foreground or background (fonts only)
//
#define 	GFXAFGR(n)  	((n) & 0xFF)
#define  	GFXABGR(n)  	(((n) >> 8) & 0xFF)
#define  	GFXACTION(a) 	((a) & 0xF0000)

#define 	GFXA_NORMAL 	(0x00000)
#define 	GFXA_XOR  		(0x10000)
#define 	GFXA_SOLID 		(0xF0000)
