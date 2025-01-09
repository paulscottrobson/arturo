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
//      Name :      keyboard.c
//      Authors :   Paul Robson (paul@robsons.org.uk)
//      Date :      24th December 2024
//      Reviewed :  No
//      Purpose :   Process scan codes.
//



#include "artsim.h"


//
//              Convert an SDL event to one suitable for the keyboard system
//


void KBDProcessEvent(int scanCode,int modifiers,bool isDown) {
    int procMod = 0;
    //
    //      Convert the modifiers.
    //
    if (modifiers & (KMOD_LSHIFT|KMOD_RSHIFT)) procMod |= KEY_SHIFT;
    if (modifiers & (KMOD_LCTRL|KMOD_RCTRL)) procMod |= KEY_CONTROL;
    if (modifiers & KMOD_LALT) procMod |= KEY_ALT;
    if (modifiers & KMOD_RALT) procMod |= KEY_ALTGR;
    //
    //      Handily the scan code *is* the USB code, so we don't have to process
    //      it. This may be different in the future, so if the keyboard suddenly
    //      stops working this might be why.
    //
    KBDReceiveEvent(isDown,scanCode,procMod);
}
