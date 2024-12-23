// ***************************************************************************************
// ***************************************************************************************
//
//      Name :      miscellany.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      23rd December 2024
//      Reviewed :  No
//      Purpose :   Miscellaneous stuff.
//
// ***************************************************************************************
// ***************************************************************************************

#include <artsim.h>

// *******************************************************************************************************************************
//
//												Get elapsed time in 100Hz ticks.
//
// *******************************************************************************************************************************

int TMRTimer(void) {
	return SDL_GetTicks()/10;
}