/**
 * @file       console.c
 *
 * @brief      Console output code, for debugging primarily.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"

/**
 * @brief      Support function for CONWrite which allows printf
 *
 * @param[in]  fmt        format string
 * @param[in]  <unnamed>  Assorted parameters.
 */

void CONWriteString(const char *fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 128, fmt, args);
    uint8_t *p = (uint8_t *)buf;
    while (*p != '\0') CONWrite(*p++);
    va_end(args);
}
