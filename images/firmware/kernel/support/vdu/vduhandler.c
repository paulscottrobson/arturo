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
