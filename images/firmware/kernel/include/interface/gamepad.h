/**
 * @file       gamepad.h
 *
 * @brief      Header file, gamepad code
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#pragma once

//
//      Structure containing the controller state. Members with _ in front of 
//      their identifier should be considered private.
//      
//      We simplify controllers so they have one D-Pad and 4 fire buttons.
//


typedef struct _controller_status {
    int         _hardwareID;                                                        // 16 bit hardware ID built from dev_addr & instance    
    int         _hardwareTypeID;                                                    // Combined VID & PID of the device.

    int         dx,dy;                                                              // Direction of the main controller
    bool        a,b,x,y;                                                            // Buttons. These should map with the SNES controller
                                                                                    // So a PSX would be Circle (A) Cross (X) Triangle (X) Square (Y)
} CTLState;

CTLState *CTLReadController(int n);
int  CTLControllerCount(void);

