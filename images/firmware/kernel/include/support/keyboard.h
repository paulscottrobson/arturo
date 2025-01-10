/**
 * @file       keyboard.h
 *
 * @brief      Header file, keyboard support
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once

#define KBD_REPEAT_START    (900)                                               // Time for first repeat (1ms units)
#define KBD_REPEAT_AFTER    (120)                                               // Time for subsequent repeats (1ms units)

void KBDReceiveEvent(uint8_t isDown,uint8_t keyCode,uint8_t modifiers);
void KBDCheckTimer(void);
int KBDGetModifiers(void);
void KBDInsertQueue(int ascii);
int KBDIsKeyAvailable(void);
uint8_t *KBDGetStateArray(void);
int KBDGetKey(void);
int KBDEscapePressed(int resetEscape);
CTLState *KBDReadController(void);
