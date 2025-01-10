/**
 * @file       timing.c
 *
 * @brief      Timing stuff
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"

struct repeating_timer timer;
bool tick50HzHasFired = false;

/**
 * @brief      Set the tick flag
 *
 *             This is optimised out because gcc doesn't recognise side effects
 *             on extern
 */
#pragma GCC push_options
#pragma GCC optimize ("O0")
static void _tmrSetTick(void) {
    tick50HzHasFired = true;                                                    // Set tick flag.
}
#pragma GCC pop_options

/**
 * @brief      50Hz callback function
 *
 * @param      t     repeating timer information
 *
 * @return     true (continue)
 */
static bool Tick50Callback(struct repeating_timer *t) {
    _tmrSetTick();                                                              // Sets the flag.
    return true;
}


/**
 * @brief      Set up the 50Hz timer
 */
void TMRStartTickInterrupt(void) {
    add_repeating_timer_ms(20, Tick50Callback, NULL, &timer);                   // Call above function every 20ms.
}


/**
 * @brief      Read the 1000 Hz Clock
 *
 * @return     Number of ms since power up
 */
int TMRReadTimeMS(void) {
    uint32_t time32 = (uint32_t)(time_us_64() >> 9);                            // divide by 512
    return (time32 * 131) >> 8;                                                // Error of about 0.07%
}