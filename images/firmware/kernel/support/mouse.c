

//
//      Name :      mouse.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//                  Sascha Schneider (schneider.sascha@mein.gmx)
//      Date :      20th December 2024
//      Reviewed :  No
//      Purpose :   Mouse code.
//



#include "common.h"

static uint16_t xCursor, yCursor;                                                // Current position
static uint8_t buttonState;
static uint8_t scrollWheelState;
static bool hasMouse = false;


//
//                              Initialise mouse
//


void MSEInitialise(void) {
    xCursor = yCursor = 0;
    buttonState = 0;
    scrollWheelState = 0;
}


//
//                              Set mouse presence flag
//


void MSEEnableMouse(void) {
    hasMouse = true;
}

bool MSEMousePresent(void) {
    return hasMouse;
}


//
//                              Set cursor position
//


void MSESetPosition(int x,int y) {
    xCursor = x;
    yCursor = y;
}

void MSEOffsetPosition(int8_t dx, int8_t dy) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();                      
    if(dx < 0 && xCursor < abs(dx)) {
        xCursor = 0;
    } else {
        xCursor += dx;
    }
    if(dy < 0 && yCursor < abs(dy)) {
        yCursor = 0;
    } else {
        yCursor += dy;
    }
    if(xCursor > dmi->width) xCursor = dmi->width;
    if(yCursor > dmi->height) yCursor = dmi->height;
}


//
//                              Update mouse scroll wheel state
//


void MSEUpdateScrollWheel(int8_t ds) {
    scrollWheelState += ds;
}


//
//                              Update mouse button state
//


void MSEUpdateButtonState(uint8_t bs) {
    buttonState = bs;
}


//
//                              Get mouse position, button state and scroll wheel state
//


void MSEGetState(int *pX, int *pY, int *pButtonState, int *pScrollWheelState) {
    if (hasMouse) {
        *pX = xCursor;
        *pY = yCursor;
        *pButtonState = buttonState;
        *pScrollWheelState = scrollWheelState;
    } else {
        *pX = *pY = *pButtonState = *pScrollWheelState = 0;
    }
}

