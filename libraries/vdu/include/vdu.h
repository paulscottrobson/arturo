/**
 * @file       vdus.h
 *
 * @brief      Header file, graphics functions
 *
 * @author     Lennart Benschop
 *
 * @date       18/01/2025
 *
 */

#pragma once

void VDUWrite(unsigned char c);
void VDUWriteString(const char *fmt, ...);
void VDUGetCursor(int *x, int *y);





  
