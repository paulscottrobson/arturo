/**
 * @file       kbd_codes.h
 *
 * @brief      Header file, keyboard codes (same as USB HID ones)
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once
//
//      Bits in the first byte of the data packet.
//
#define KEY_MOD_LCTRL  0x01
#define KEY_MOD_LSHIFT 0x02
#define KEY_MOD_LALT   0x04
#define KEY_MOD_RCTRL  0x10
#define KEY_MOD_RSHIFT 0x20
#define KEY_MOD_RALT    0x40
//
//      Actual mask bits.
//
#define KEY_NOSHIFT     (0)
#define KEY_SHIFT       (KEY_MOD_LSHIFT|KEY_MOD_RSHIFT)
#define KEY_CONTROL     (KEY_MOD_LCTRL|KEY_MOD_RCTRL)
#define KEY_ALT         (KEY_MOD_LALT)
#define KEY_ALTGR       (KEY_MOD_RALT)
//
//      Key sequence to Reboot.
//
#define REBOOT_KEYS     (KEY_MOD_LCTRL|KEY_MOD_LALT|KEY_MOD_RALT)
//
//      Keys A-Z are in order
//
#define KEY_A 0x04          
#define KEY_B (KEY_A+1)
#define KEY_C (KEY_A+2)
#define KEY_D (KEY_A+3)
#define KEY_E (KEY_A+4)
#define KEY_F (KEY_A+5)
#define KEY_G (KEY_A+6)
#define KEY_H (KEY_A+7)
#define KEY_I (KEY_A+8)
#define KEY_J (KEY_A+9)
#define KEY_K (KEY_A+10)
#define KEY_L (KEY_A+11)
#define KEY_M (KEY_A+12)
#define KEY_N (KEY_A+13)
#define KEY_O (KEY_A+14)
#define KEY_P (KEY_A+15)
#define KEY_Q (KEY_A+16)
#define KEY_R (KEY_A+17)
#define KEY_S (KEY_A+18)
#define KEY_T (KEY_A+19)
#define KEY_U (KEY_A+20)
#define KEY_V (KEY_A+21)
#define KEY_W (KEY_A+22)
#define KEY_X (KEY_A+23)
#define KEY_Y (KEY_A+24)
#define KEY_Z (KEY_A+25)
//
//      Number keys are in the tweaked order 1-9,0 matching the keyboard.
//      Two sets, top row and number keypad.
//
#define KEY_1 0x1e          
#define KEY_2 (KEY_1+1)
#define KEY_3 (KEY_1+2)
#define KEY_4 (KEY_1+3)
#define KEY_5 (KEY_1+4)
#define KEY_6 (KEY_1+5)
#define KEY_7 (KEY_1+6)
#define KEY_8 (KEY_1+7)
#define KEY_9 (KEY_1+8)
#define KEY_0 (KEY_1+9)

#define KEY_KP1 0x59            // We don't use these 10, but the USB routine can map them onto $1E...
//
//      Standard keys
//
#define KEY_ENTER 0x28          // Keyboard Return (ENTER)          M / 13
#define KEY_ESC 0x29            // Keyboard ESCAPE                  [ / 27
#define KEY_BACKSPACE 0x2a      // Keyboard DELETE (Backspace)      127
#define KEY_TAB 0x2b            // Keyboard Tab                     I / 9

#define KEY_SPACE 0x2c          // Keyboard Spacebar                
#define KEY_MINUS 0x2d          // Keyboard - and _ 
#define KEY_EQUAL 0x2e          // Keyboard = and +
#define KEY_LEFTBRACE 0x2f      // Keyboard [ and {
#define KEY_RIGHTBRACE 0x30     // Keyboard ] and }
#define KEY_BACKSLASH 0x31      // Keyboard \ and |
#define KEY_HASHTILDE 0x32      // Keyboard Non-US # and ~
#define KEY_SEMICOLON 0x33      // Keyboard ; and :
#define KEY_APOSTROPHE 0x34     // Keyboard ' and "
#define KEY_GRAVE 0x35          // Keyboard ` and ~
#define KEY_COMMA 0x36          // Keyboard , and <
#define KEY_DOT 0x37            // Keyboard . and >
#define KEY_SLASH 0x38          // Keyboard / and ?
//
//      Function keys F1-F10 are supported.
//
#define KEY_F1 0x3a             // Return 129..138
//
//      Control keys (Have roughly position control key values)
//
#define KEY_INSERT 0x49         // Keyboard Insert                  E / 5
#define KEY_HOME 0x4a           // Keyboard Home                    T / 20
#define KEY_PAGEUP 0x4b         // Keyboard Page Up                 R / 18
#define KEY_DELETE 0x4c         // Keyboard Delete Forward          H / 8
#define KEY_END 0x4d            // Keyboard End                     G / 7
#define KEY_PAGEDOWN 0x4e       // Keyboard Page Down               F / 6
#define KEY_RIGHT 0x4f          // Keyboard Right Arrow             D / 4
#define KEY_LEFT 0x50           // Keyboard Left Arrow              A / 1
#define KEY_DOWN 0x51           // Keyboard Down Arrow              S / 19
#define KEY_UP 0x52             // Keyboard Up Arrow                W / 23

#define KEY_102ND 0x64          // Keyboard Non-US \ |   

#define KBD_MAX_KEYCODE (0x65)  // The biggest scancode we store, +1

