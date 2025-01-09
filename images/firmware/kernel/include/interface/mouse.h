/**
 * @file mouse.h
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
void MSEUpdateScrollWheel(int8_t ds);
void MSEUpdateButtonState(uint8_t bs);
void MSEOffsetPosition(int8_t dx, int8_t dy);


