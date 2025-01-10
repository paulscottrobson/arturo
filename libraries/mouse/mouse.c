/**
 * @file       mouse.c
 *
 * @brief      Mouse routines
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#include "common.h"
#include <libraries.h>

void MMGRDrawPointer(int xp,int yp,int size) {
    for (int i = 0;i < size;i++) {
        bool tbLine = (i == 0) || (i == size-1);
        GFXHLine(NULL,xp+i/2,xp+i*2,yp+i,tbLine ? 7 : 0);
        if (!tbLine) {
            GFXPlot(NULL,xp+i/2,yp+i,7);
            GFXPlot(NULL,xp+i*2,yp+i,7);
        }
    }
}