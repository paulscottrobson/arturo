/**
 * @file       mouse.h
 *
 * @brief      Header file, mouse hardware
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#pragma once

bool MSEMousePresent(void);
void MSESetPosition(int x, int y);
void MSEGetState(int *pX, int *pY, int *pButtonState, int *pScrollWheelState);

//
//		Exposed for simulator.
//
void MSEInitialise(void);
void MSEEnableMouse(void);
void MSEUpdateScrollWheel(int ds);
void MSEUpdateButtonState(int bs);
void MSEOffsetPosition(int dx, int dy);


