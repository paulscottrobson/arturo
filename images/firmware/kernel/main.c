/**
 * @file       main.c
 *
 * @brief      Main Program.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"

/**
 * @brief      Shows the ram available.
 * 
 * May only work on RP2040
 */
static void _showRAMAvailable(void) {
    extern char __StackLimit, __bss_end__;
    int ramLeft =  &__StackLimit  - &__bss_end__;
    CONWriteString("SRAM memory free %dk\r\n",ramLeft >> 10);
}


/**
 * @brief      Start the main Kernel
 *
 * @return     Doesn't !
 */
int main() {
	DVIStart();																	// Start DVI running.
    CONWrite(22);CONWrite(DVI_MODE_640_240_8);                                  // Switch mode.
    _showRAMAvailable();                                                        // Show RAM free.
    SNDInitialise(ARTURO_MONO_SOUND != 0);                                      // Start the sound system, seperate channels according to config.
    SNDMuteAllChannels();                                                       // Mute all channels
    MSEInitialise();                                                            // Initialise the mouse system
    CTLInitialise();                                                            // Initialise the gamepad system
    USBInitialise();                                                            // Initialise keyboard & USB system.
    USBKeyboardEvent(0,0xFF,0);                                                 // Reset the keyboard support system
    USBSynchronise();                                                           // Synchronise storage
    TMRStartTickInterrupt();                                                    // Start the tick interrupt.
    FIOInitialise();                                                            // Initialise the file system (core)
    LOCSetLocale(ARTURO_KBD_LOCALE);                                            // Default Locale
    CONWriteString("Booting application...\r\n\r\n");
    ApplicationRun();                                                           // Run application
    while (true) {}
}