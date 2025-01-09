/**
 * @file 
 *
 * @brief      
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */


//
//		Name : 		main.c
//		Author :	Paul Robson (paul@robsons.org.uk)
//		Date : 		18th December 2024
//		Reviewed :	No
//		Purpose :	Main Program.
//



#include "common.h"


//
//                        Display unused RAM memory. This may be RP2040 only.
//


static void _showRAMAvailable(void) {
    extern char __StackLimit, __bss_end__;
    int ramLeft =  &__StackLimit  - &__bss_end__;
    CONWriteString("SRAM memory free %dk\r",ramLeft >> 10);
}


//
//										Start the kernel
//


int main() {
	DVIStart();																	// Start DVI running.
	CONInitialise();  															// Initialise the console.
    LOGODrawTitle();                                                            // Olimex Logo.
    LOGOTextHeader();                                                           // Write text header
    _showRAMAvailable();                                                        // Show RAM free.
    SNDInitialise(ARTURO_MONO_SOUND != 0);                                      // Start the sound system, seperate channels according to config.
    SNDMuteAllChannels();                                                       // Mute all channels
    MSEInitialise();                                                            // Initialise the mouse system
    CTLInitialise();                                                            // Initialise the gamepad system
    USBInitialise();                                                            // Initialise keyboard & USB system.
    USBKeyboardEvent(0,0xFF,0);                                                 // Announce keyboard has been reset
    USBSynchronise();                                                           // Synchronise storage
    TMRStartTickInterrupt();                                                    // Start the tick interrupt.
    FIOInitialise();                                                            // Initialise the file system (core)
    LOCSetLocale(ARTURO_KBD_LOCALE);                                            // Default Locale
    CONWriteString("Booting application...\r\r");
    ApplicationRun();                                                           // Run application
    while (true) {}
}
