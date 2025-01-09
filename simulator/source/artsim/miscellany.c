/**
 * @file miscellany.c
 *
 * @brief      Miscellaneous simulator routines
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


#include <artsim.h>


/**
 * @brief      Get elapsed time since start
 *
 * @return     time in 1khz ticks
 */
int TMRReadTimeMS(void) {
    return SDL_GetTicks();
}