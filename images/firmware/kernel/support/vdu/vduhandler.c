/**
* @file       vduhandler.c
*
* @brief      Handles and dispatches VDU Commands
*
* @author     Paul Robson
*
* @date       27/01/2025
*
*/

#include "common.h"

/*
        This table is the number of additional bytes needed for each VDU command
*/

const uint8_t _VDUCommandLengths[32] = {
        0,                                                                          // 0 does nothing
        1,                                                                          // 1 sends character to the printer
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,                                                // 2-16 are all single byte VDU commands
        1,                                                                          // 17 sets text colour (COLOUR)
        2,                                                                          // 18 sets background colour (GCOL)
        5,                                                                          // 19 redefines colour
        0,0,                                                                        // 20,21 single byte
        1,                                                                          // 22 sets the mode (MODE)
        9,                                                                          // 23 defines UDG & controls 6845
        8,                                                                          // 24 sets the graphics windows
        5,                                                                          // 25 is PLOT 
        0,0,                                                                        // 26,27 are single byte
        4,                                                                          // 28 sets the text window
        4,                                                                          // 29 sets the graphics origin
        0,                                                                          // 30 is single byte
        2                                                                           // 31 is move text cursor
};

/**
* @brief      Write one character or control code (bodge version)
*
* @param[in]  c     Character code
*/

void VDUWrite(int c) {
    struct DVIModeInformation *dmi = DVIGetModeInformation();                       // Identify mode data.
    if (dmi == NULL) return;
    switch(c) {
        case 12:                                                                    // Clear Screen.
            VDUClearScreen();
            VDUSetCursor(0,0);
            break;

        case 8:
            VDUBackspace();
            break;

        case 13:                                                                    // New line
            VDUNewLine();
            break;

        default:
            VDUWriteText(c);
            break;
    }
}


/**
 * @brief      Support function for VDUWrite which allows printf
 *
 * @param[in]  fmt        format string
 * @param[in]  <unnamed>  Assorted parameters.
 */

void VDUWriteString(const char *fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 128, fmt, args);
    uint8_t *p = (uint8_t *)buf;
    while (*p != '\0') VDUWrite(*p++);
    va_end(args);
}

/**
 * @brief      Bridge from CONWrite to VDUWrite. 
 * 
 * This is only compiled if ARTURO_PROCESS_CONSOLE is set to 1.
 *
 * @param[in]  c     Character to write
 */

#if ARTURO_PROCESS_CONSOLE==1

void CONWrite(int c) {
    VDUWrite(c);
}

#endif
