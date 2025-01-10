/**
 * @file       keyboard.c
 *
 * @brief      Keyboard event processor.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "artsim.h"

/**
 * @brief      Convert an SDL event to one suitable for the keyboard system
 *
 * @param[in]  scanCode   The scan code 
 * @param[in]  modifiers  The modifiers
 * @param[in]  isDown     Indicates if key down
 */
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
