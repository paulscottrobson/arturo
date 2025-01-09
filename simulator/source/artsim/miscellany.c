/**
 * @file 
 *
 * @brief      
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


//
//      Name :      miscellany.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      23rd December 2024
//      Reviewed :  No
//      Purpose :   Miscellaneous stuff.
//



#include <artsim.h>


//
//                                              Get elapsed time in 1kHz ticks.
//


int TMRReadTimeMS(void) {
    return SDL_GetTicks();
}