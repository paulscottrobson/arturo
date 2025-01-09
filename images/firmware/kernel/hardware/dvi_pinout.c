

//
//		Name : 		dvi_pinout.c
//		Author :	Paul Robson (paul@robsons.org.uk)
//		Date : 		18th December 2024
//		Reviewed :	No
//		Purpose :	DVI HDMI setup function.
//



#include "common.h"

#include "dvi.h"
#include "dvi_serialiser.h"
#include "common_dvi_pin_configs.h"


//
//					Neo6502 / RP2040PC Serialiser & Access function
//


static struct dvi_serialiser_cfg pico_neo6502_cfg = {
	.pio = DVI_DEFAULT_PIO_INST,
	.sm_tmds = {0, 1, 2},
	.pins_tmds = {14, 18, 16},
	.pins_clk = 12,
	.invert_diffpairs = true
};

struct dvi_serialiser_cfg *DVIGetHDMIConfig(void) {
	return &pico_neo6502_cfg;
}