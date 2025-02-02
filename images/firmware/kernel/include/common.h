/**
 * @file       common.h
 *
 * @brief      Header file, all headers
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>

#define min(a,b)		((a) < (b) ? (a):(b))
#define max(a,b)		((a) > (b) ? (a):(b))

#include <support/__config.h>
//
//		Used building for the PICO
//
#ifdef PICO
#include "ff.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "hardware/vreg.h"
#include "hardware/watchdog.h"
#include "hardware/pwm.h"
#else
//
//		Outside that we use standard libraries.
//
#include <stdint.h>
#include <stdbool.h>
#endif

#include "interface/gamepad.h"
#include "interface/dvi_common.h"
#include "interface/miscellany.h"
#include "interface/sound.h"
#include "interface/mouse.h"
#include "interface/fileio.h"
#include "interface/usb_interface.h"
#include "interface/vdu_internal.h"

#ifdef PICO
#include "internal.h"
#endif
#include "support.h"



