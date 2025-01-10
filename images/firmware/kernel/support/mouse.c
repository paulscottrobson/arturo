/**
 * @file       mouse.c
 *
 * @brief      Mouse code.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#include "common.h"

static uint16_t xCursor, yCursor;                                                // Current position
static uint8_t buttonState;
static uint8_t scrollWheelState;
static bool hasMouse = false;

/**
 * @brief      Initialise mouse systm
 */
void MSEInitialise(void) {
    xCursor = yCursor = 0;
    buttonState = 0;
    scrollWheelState = 0;
}


/**
 * @brief      Enable the mouse
 */
void MSEEnableMouse(void) {
    hasMouse = true;
}

/**
 * @brief      Check mouse is plugged in.
 *
 * @return     true if physical mouse present
 */
bool MSEMousePresent(void) {
    return hasMouse;
}


/**
 * @brief      Set cursor position (pixels)
 *
 * @param[in]  x     x position
 * @param[in]  y     y position
 */
void MSESetPosition(int x,int y) {
    xCursor = x;
    yCursor = y;
}

/**
 * @brief      Move mouse location on display by offset from current
 *
 * @param[in]  dx    x position
 * @param[in]  dy    y position
 */

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


/**
 * @brief      Update scroll wheel
 *
 * @param[in]  ds    Scroll wheel data
 */
void MSEUpdateScrollWheel(int8_t ds) {
    scrollWheelState += ds;
}


/**
 * @brief      Update the buttons current state
 *
 * @param[in]  bs    One bit per button.
 */
void MSEUpdateButtonState(uint8_t bs) {
    buttonState = bs;
}


/**
 * @brief      Read current stte of mouse
 *
 * @param      pX                 pointer to lnt variablex, x pos
 * @param      pY                 pointer to int variabley, y pos
 * @param      pButtonState       pointer to int, state of buttons
 * @param      pScrollWheelState  poninter to int
 */
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
