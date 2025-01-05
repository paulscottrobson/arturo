// ***************************************************************************************
// ***************************************************************************************
//
//		Name : 		miscellany.h
//		Author :	Paul Robson (paul@robsons.org.uk)
//		Date : 		18th December 2024
//		Reviewed :	No
//		Purpose :	Odds and sods
//
// ***************************************************************************************
// ***************************************************************************************

#pragma once
//
//		Main execution
//
void ApplicationRun(void);
//
//		Timer/Interrupt functions.
//
int TMRReadTimeMS(void);
//
//		Set every 20ms
//
extern bool tick50HzHasFired;
//
//		Macros for detecting the tick and resetting it.
//
#define HASTICK50_FIRED()	(tick50HzHasFired)
#define TICK50_RESET() 		{ tick50HzHasFired = false; }
