/**
 * @file       dvi_pinout.c
 *
 * @brief      DVI HDMI setup function.
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include "common.h"

#include "dvi.h"
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"


static struct dvi_serialiser_cfg pico_neo6502_cfg = {
    .pio = DVI_DEFAULT_PIO_INST,
    .sm_tmds = {0, 1, 2},
    .pins_tmds = {14, 18, 16},
    .pins_clk = 12,
    .invert_diffpairs = true
};

/**
 * @brief      Get the connection for the HDMI driver
 *
 * @return     HDMI configuration structure for Pico/Neo6502
 */
struct dvi_serialiser_cfg *DVIGetHDMIConfig(void) {
    return &pico_neo6502_cfg;
}