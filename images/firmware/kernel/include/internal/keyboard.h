/**
 * @file       keyboard.h
 *
 * @brief      Header file, keyboard internals
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#pragma once
//
//      If this constant is set to non-zero, the full keyboard decoding is used. Otherwise
//      the user can handle the Keyboard events from the usb driver themselves.
//
#if     ARTURO_PROCESS_KEYS != 0
#define USBKeyboardEvent(x,y,z) KBDReceiveEvent(x,y,z)
#endif

