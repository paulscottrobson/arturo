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
void VDUPlotCommand(int cmd,int x,int y);
void VDUSetGraphicsColour(int mode,int colour);

