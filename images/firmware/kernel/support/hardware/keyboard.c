/**
 * @file       keyboard.c
 *
 * @brief      Converts keyboard events to a queue and key state array.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"

#define MAX_QUEUE_SIZE (64)                                                     // Max size of keyboard queue.

//
//      Bit patterns for the key states. These represent the key codes (see kbdcodes.h)
//      at 8 states per byte, so keycode 0 is byte 0 bit 0, keycode 7 is byte 0 bit 7
//      keycode 8 is byte 1 bit 0 etc.
//
static uint8_t keyboardState[KBD_MAX_KEYCODE+1];
static uint8_t keyboardModifiers;
//
//      Queue of ASCII keycode presses.
//
static uint8_t queue[MAX_QUEUE_SIZE+1];
static uint8_t queueSize = 0;

static uint8_t currentASCII = 0,currentKeyCode = 0;                             // Current key pressed.
static uint32_t nextRepeat = 9999;                                              // Time of next repeat.
static bool    escapeFlag = false;                                              // Set when escape pressed.

static uint8_t KBDMapToASCII(uint8_t keyCode,uint8_t modifiers);
static uint8_t KBDDefaultASCIIKeys(uint8_t keyCode,uint8_t isShift);
static uint8_t KBDDefaultControlKeys(uint8_t keyCode,uint8_t isShift);


/**
 * @brief      Handle event from USB handler
 *
 * @param[in]  isDown     Indicates if down
 * @param[in]  keyCode    The key code, $FF resets
 * @param[in]  modifiers  The modifiers held at the time of pressing
 */
void KBDReceiveEvent(uint8_t isDown,uint8_t keyCode,uint8_t modifiers) {

    if (isDown && keyCode == KEY_ESC) {                                         // Pressed ESC
        escapeFlag = true;
    }

    if (keyCode == 0xFF) {                                                      // Reset request
        queueSize = 0;                                                          // Empty keyboard queue
        for (unsigned int i = 0;i < sizeof(keyboardState);i++) {                // No keys down.
            keyboardState[i] = 0;
        escapeFlag = false;                                                     // Reset escape.
        }
        return;
    }

    if (keyCode != 0 && keyCode < KBD_MAX_KEYCODE) {                            // Legitimate keycode.
        if (isDown) {
            keyboardState[keyCode] = 0xFF;                                      // Set down flag.
            keyboardModifiers = modifiers;                                      // Copy modifiers
            uint8_t ascii = KBDMapToASCII(keyCode,modifiers);                   // What key ?
            if (ascii != 0) {
                currentASCII = ascii;                                           // Remember code and time.
                currentKeyCode = keyCode;
                KBDInsertQueue(ascii);                                          // Push in the queue
                nextRepeat = TMRReadTimeMS()+KBD_REPEAT_START;                  // Slow start and faster repeat.
            }
        } else {
            keyboardState[keyCode] = 0x00;                                      // Clear flag
            keyboardModifiers = 0x00;                                           // Clear Modifiers
            if (keyCode == currentKeyCode) currentASCII = 0;                    // Autorepeat off, key released.
        }
    }
}


/**
 * @brief      Called externally to do autorepeat
 */
void KBDCheckTimer(void) {
    #if ARTURO_PROCESS_KEYS != 0                                                // If not processing keys this is a no-operation.
    if (currentASCII != 0) {                                                    // Key pressed
        if (TMRReadTimeMS() >= nextRepeat) {                                    // Time up ?
            KBDInsertQueue(currentASCII);                                       // Put in queue
            nextRepeat = TMRReadTimeMS()+KBD_REPEAT_AFTER;                      // Quicker repeat after first time.
        }
    }
    #endif
}



/**
 * @brief      Access key states
 *
 * @return     Returns a uint8_t *array indicating the state of each key
 */
uint8_t *KBDGetStateArray(void) {
    return keyboardState;
}

/**
 * @brief      Get the most recent state of the modifiers
 *
 * @return     Array of bits
 */
int KBDGetModifiers(void) {
    return keyboardModifiers;
};



/**
 * @brief      Read the faux controller that uses keyboard keys.
 *
 * @return     pointer to CTLState structure showing gamepad button states.
 */
static CTLState kbCt;                                                               // Used to return result
#define CHECK(k,v)  (keyboardState[k] ? (v) : 0)

CTLState *KBDReadController(void) {
    kbCt.dx = CHECK(KEY_A,-1) + CHECK(KEY_D,1);                                     // WASD direction
    kbCt.dy = CHECK(KEY_W,-1) + CHECK(KEY_S,1);
    kbCt.a = CHECK(KEY_L,1) != 0;                                                   // IJKL buttons
    kbCt.b = CHECK(KEY_K,1) != 0;
    kbCt.x = CHECK(KEY_I,1) != 0;
    kbCt.y = CHECK(KEY_J,1) != 0;
    return &kbCt;
}


/**
 * @brief      Check if escape pressed
 *
 * @param[in]  resetEscape  Reset the flag on read
 *
 * @return     true if Escape pressed
 */
int KBDEscapePressed(int resetEscape) {
    bool state = escapeFlag;
    if (resetEscape) escapeFlag = false;
    return state;
}


/**
 * @brief      Insert key into keyboard queue
 *
 * @param[in]  ascii  The ascii value
 */
void KBDInsertQueue(int ascii) {
    if (queueSize < MAX_QUEUE_SIZE) {                                           // Do we have a full queue ?
        queue[queueSize] = ascii;                                               // If not insert it.
        queueSize++;
    }
}


/**
 * @brief      Check keyboard queue
 *
 * @return     Returns non -zero if key available
 */
int KBDIsKeyAvailable(void) {
    return queueSize != 0;
}


/**
 * @brief      Get the first key in the queue
 *
 * @return     ASCII value or 0 if no key
 */
int KBDGetKey(void) {
    if (queueSize == 0) return 0;                                               // Queue empty.
    uint8_t key = queue[0];
    for (uint8_t i = 0;i < queueSize;i++) queue[i] = queue[i+1];                // Dequeue it
    queueSize--;
    return key;
}


/**
 * @brief      Coonvert key code and modifiers to ASCII, apply localisation
 *
 * @param[in]  keyCode    The key code
 * @param[in]  modifiers  The modifiers
 *
 * @return     { description_of_the_return_value }
 */
static uint8_t KBDMapToASCII(uint8_t keyCode,uint8_t modifiers) {
    uint8_t ascii = 0;
    uint8_t isShift = (modifiers & KEY_SHIFT) != 0;
    uint8_t isControl = (modifiers & KEY_CONTROL) != 0;

    if (keyCode >= KEY_A && keyCode < KEY_A+26) {                               // Handle alphabet.
        ascii = keyCode - KEY_A + 'A';                                          // Make ASCII
        if (!isShift) ascii += 'a'-'A';                                         // Handle shift
        if (isControl) ascii &= 0x1F;                                           // Handle control
    }

    if (ascii == 0 && keyCode >= KEY_1 && keyCode < KEY_1+10) {                 // Handle numbers - slightly mangled.
        ascii = (keyCode == KEY_1+9) ? '0' : keyCode - KEY_1 + '1';             // because the order on the keyboard isn't 0-9!
        if (isShift) ascii = ")!@#$%^&*("[ascii - '0'];                         // Standard US mapping. of the top row
    }

    if (ascii == 0) {                                                           // This maps all the other ASCII keys.
        ascii = KBDDefaultASCIIKeys(keyCode,modifiers);
    }
    if (ascii == 0) {                                                           // This maps all the control keys
        ascii = KBDDefaultControlKeys(keyCode,modifiers);
    }

    return LOCLocaleMapping(ascii,keyCode,modifiers);                           // Special mapping for locales.
}


//
//                      Work out ASCII keys (including space)
//


#define KEY(code,normal,shifted) code,normal,shifted

static const uint8_t defaultShift[] = {
    KEY(KEY_MINUS,'-','_'),         KEY(KEY_EQUAL,'=','+'),         KEY(KEY_LEFTBRACE,'[','{'),
    KEY(KEY_RIGHTBRACE,']','}'),    KEY(KEY_BACKSLASH,'\\','|'),    KEY(KEY_HASHTILDE,'#','~'),
    KEY(KEY_SEMICOLON,';',':'),     KEY(KEY_APOSTROPHE,'\'','"'),   KEY(KEY_GRAVE,'`','~'),
    KEY(KEY_COMMA,',','<'),         KEY(KEY_DOT,'.','>'),           KEY(KEY_SLASH,'/','?'),
    KEY(KEY_SPACE,' ',' '),         KEY(KEY_102ND,'\\','|'),
    0
};

/**
 * @brief      Work out standard ASCII keys
 *
 * @param[in]  keyCode  The key code used
 * @param[in]  isShift  Indicates if shift pressed
 *
 * @return     ASCII value
 */
static uint8_t KBDDefaultASCIIKeys(uint8_t keyCode,uint8_t isShift) {
    uint8_t ascii = 0;
    uint8_t index = 0;
    while (defaultShift[index] != 0 && defaultShift[index] != keyCode)          // Look up in table of standard mappings (US)
        index += 3;
    if (defaultShift[index] == keyCode) {                                       // found a match.
        ascii = isShift ? defaultShift[index+2] : defaultShift[index+1];
    }
    return ascii;
}

/**
 * @brief      Work out control keys
 *
 * @param[in]  keyCode  The key code
 * @param[in]  isShift  Indicates if shift
 *
 * @return     ASCII / Control code
 */


static const uint8_t defaultControlKeys[] = {
    KEY_LEFT,CC_LEFT,KEY_RIGHT,CC_RIGHT,KEY_INSERT,CC_INSERT,
    KEY_PAGEDOWN,CC_PAGEDOWN,KEY_END,CC_END,KEY_DELETE,CC_DELETE,
    KEY_TAB,CC_TAB,KEY_ENTER,CC_ENTER,KEY_PAGEUP,CC_PAGEUP,KEY_DOWN,CC_DOWN,
    KEY_HOME,CC_HOME,KEY_UP,CC_UP,KEY_ESC,CC_ESC,
    KEY_BACKSPACE, CC_BACKSPACE, 0
};

static uint8_t KBDDefaultControlKeys(uint8_t keyCode,uint8_t isShift) {
    uint8_t index = 0;
    while (defaultControlKeys[index] != 0) {
        if (defaultControlKeys[index] == keyCode) {
            return defaultControlKeys[index+1];
        }
        index += 2;
    }
    return 0;
}

