/**
 * @file       miscellany.h
 *
 * @brief      Header file, miscellany
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once
//
//      Main execution
//
void ApplicationRun(void);
//
//      Timer/Interrupt functions.
//
int TMRReadTimeMS(void);
//
//      Set every 20ms
//
extern bool tick50HzHasFired;
//
//      Macros for detecting the tick and resetting it.
//
#define HASTICK50_FIRED()   (tick50HzHasFired)
#define TICK50_RESET()      { tick50HzHasFired = false; }
