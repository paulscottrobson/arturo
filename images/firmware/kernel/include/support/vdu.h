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

void VDUNewLine(void);	
void VDUWriteText(char c);
void VDUBackspace(void);
void VDUClearScreen(void);
void VDUSetCursor(int x,int y);
uint8_t VDUGetCharacterLineData(int c,int y);