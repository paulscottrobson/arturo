/**
 * @file       gamepad_drivers.h
 *
 * @brief      Header file, hardware specific drivers
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#pragma once

//
//      List of drivers for gamepads, all should have the same signature.
//
int  CTLDriverSNESType(int command,CTLState *cs,struct _CTL_MessageData *msg);
