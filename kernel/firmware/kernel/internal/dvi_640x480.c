/**
 * @file       dvi_640x480.c
 *
 * @brief      DVI Driver 640x480 frame size.
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

struct dvi_serialiser_cfg *DVIGetHDMIConfig(void);


//
//        Frame buffer, these are 3 planar bitmaps for 640x480
//

#define FRAME_WIDTH 640                                                             // Not the *pixels*, it's the display setting.
#define FRAME_HEIGHT 480

#define PLANE_SIZE(x,y) ((x) * (y) / 8)                                             // Memory usage one bitplane x by y

//
//    Memory requirements for video RAM. Normally the max is 640x240x8 colours, but we do allow 640x480x8 colours
//    but it has to be enable as it uses 2 x the RAM, which is about half of a RP2040
//
#if DVI_SUPPORT_640_480_8==1
#define VIDEO_BYTES (PLANE_SIZE(640,480) * 3)
#else
#define VIDEO_BYTES (PLANE_SIZE(640,240) * 3)
#endif

static uint8_t framebuf[VIDEO_BYTES];                                               // Bitplane memory

struct dvi_inst dvi0;                                                               // PicoDVI structure
struct DVIModeInformation dvi_modeInfo;                                             // Mode information structure.

/**
 * @brief      Get mode information
 *
 * @return     Pointer to a mode information structure
 */
struct DVIModeInformation *DVIGetModeInformation(void) {
    return &dvi_modeInfo;
}

/**
 * @brief      Gets the screen size and mode
 *
 * @param      pWidth   if not NULL, store width in int here
 * @param      pHeight  if not NULL, store height in int here
 *
 * @return     current mode
 */
int  DVIGetScreenExtent(int *pWidth,int *pHeight) {
    if (pWidth != NULL) *pWidth = dvi_modeInfo.width;
    if (pHeight != NULL) *pHeight = dvi_modeInfo.height;
    return dvi_modeInfo.mode;
}


//
//                            Set current mode
//


/**
 * @brief      Set current mode
 *
 * @param[in]  mode  The mode to set.
 *
 * @return     0 if switched ok.
 */
bool DVISetMode(int mode) {
    bool supported = true;

    #if DVI_SUPPORT_640_480_8 == 0                                                  // We do not suport this mode.
    if (mode == DVI_MODE_640_480_8) {                                               // If we are asking for it.
        mode = DVI_MODE_640_480_2;                                                  // Use the 2 colour mode we do support.
    }
    #endif

    dvi_modeInfo.mode = mode;                                                       // Record mode

    switch(mode) {

        case DVI_MODE_640_480_8:                                                    // 640x480x8 information if enabled.
            dvi_modeInfo.width = 640;dvi_modeInfo.height = 480;
            dvi_modeInfo.bitPlaneCount = 3;
            dvi_modeInfo.bitPlaneSize = PLANE_SIZE(640,480);
            dvi_modeInfo.bitPlaneDepth = 1;
            for (int i = 0;i <dvi_modeInfo.bitPlaneCount;i++)
                dvi_modeInfo.bitPlane[i] = framebuf + dvi_modeInfo.bitPlaneSize * i;
            dvi_modeInfo.bytesPerLine = dvi_modeInfo.width / 8;
        break;

        case DVI_MODE_640_240_8:                                                    // 640x240x8 information.
            dvi_modeInfo.width = 640;dvi_modeInfo.height = 240;
            dvi_modeInfo.bitPlaneCount = 3;
            dvi_modeInfo.bitPlaneSize = PLANE_SIZE(640,240);
            dvi_modeInfo.bitPlaneDepth = 1;
            for (int i = 0;i <dvi_modeInfo.bitPlaneCount;i++)
                dvi_modeInfo.bitPlane[i] = framebuf + dvi_modeInfo.bitPlaneSize * i;
            dvi_modeInfo.bytesPerLine = dvi_modeInfo.width / 8;
        break;

        case DVI_MODE_640_480_2:                                                    // 640x480x2 information.
            dvi_modeInfo.width = 640;dvi_modeInfo.height = 480;
            dvi_modeInfo.bitPlaneCount = 1;
            dvi_modeInfo.bitPlaneSize = PLANE_SIZE(640,480);
            dvi_modeInfo.bitPlaneDepth = 1;
            dvi_modeInfo.bitPlane[0] = framebuf;
            dvi_modeInfo.bytesPerLine = dvi_modeInfo.width / 8;
        break;

        case DVI_MODE_320_240_8:                                                    // 320x240x8 information.
            dvi_modeInfo.width = 320;dvi_modeInfo.height = 240;
            dvi_modeInfo.bitPlaneCount = 3;
            dvi_modeInfo.bitPlaneSize = PLANE_SIZE(320,240);
            dvi_modeInfo.bitPlaneDepth = 1;
            for (int i = 0;i <dvi_modeInfo.bitPlaneCount;i++)
                dvi_modeInfo.bitPlane[i] = framebuf + dvi_modeInfo.bitPlaneSize * i;
            dvi_modeInfo.bytesPerLine = dvi_modeInfo.width / 8;
        break;

        case DVI_MODE_320_256_8:                                                    // 320x256x8 information.
            dvi_modeInfo.width = 320;dvi_modeInfo.height = 256;
            dvi_modeInfo.bitPlaneCount = 3;
            dvi_modeInfo.bitPlaneSize = PLANE_SIZE(320,256);
            dvi_modeInfo.bitPlaneDepth = 1;
            for (int i = 0;i <dvi_modeInfo.bitPlaneCount;i++)
            dvi_modeInfo.bitPlane[i] = framebuf + dvi_modeInfo.bitPlaneSize * i;
            dvi_modeInfo.bytesPerLine = dvi_modeInfo.width / 8;
        break;

        case DVI_MODE_320_240_64:                                                   // 320x240x64 information.
            dvi_modeInfo.width = 320;dvi_modeInfo.height = 240;
            dvi_modeInfo.bitPlaneCount = 3;
            dvi_modeInfo.bitPlaneSize = PLANE_SIZE(320,240)*2;
            dvi_modeInfo.bitPlaneDepth = 2;
            for (int i = 0;i <dvi_modeInfo.bitPlaneCount;i++)
            dvi_modeInfo.bitPlane[i] = framebuf + dvi_modeInfo.bitPlaneSize * i;
            dvi_modeInfo.bytesPerLine = dvi_modeInfo.width / 4;        
            break;
        default:
            supported = false;
            dvi_modeInfo.mode = -1;                                                 // Failed.
            break;
        }
    return supported;
}


/**
 * @brief      Set monochrome mode fgr and bgr
 *
 * @param[in]  fg    The foreground (unchanged if -ve)
 * @param[in]  bg    The background (unchanged if -ve)
 */
static uint8_t mono_fg_bg = 0x07;                                                   // Current colours for 640x480x2 colour

void DVISetMonoColour(int fg, int bg) {
    printf("Setting colour to %d %d\n",fg,bg);
    if (fg >= 0) mono_fg_bg = (mono_fg_bg & 0xF8) | fg;
    if (bg >= 0) mono_fg_bg = (mono_fg_bg & 0x07) | (bg << 3);
}


//
//
//                  The main line renderer
//
//

static uint8_t _buffer[80];                                                         // Buffer for line
static uint16_t _mapping[256];                                                      // Table mapping 320 bits to 640 bits
static uint32_t all_zero[20];
static uint32_t all_one[20];

void __not_in_flash("main") dvi_core1_main() {

    uint32_t *tmdsbuf;
    dvi_register_irqs_this_core(&dvi0, DMA_IRQ_0);
    dvi_start(&dvi0);
    uint y = -1;
    uint y0 = 0;
    uint y1 = 0;
    //
    //    This table maps an 8 bit bit pattern into a 'double width' 16 bit pattern.
    //
    for (int i = 0;i < 256;i++) {
        _mapping[i] = 0;
        for (int b = 0;b < 8;b++) {
            if (i & (1 << b)) _mapping[i] |= (3 << (b*2));
        }
        _mapping[i] = (_mapping[i] >> 8) | (_mapping[i] << 8);
    }
    for (int i=0; i<20; i++) {
        all_one[i] = 0xffffffff;
        all_zero[i] = 0;
    }
    while (true) {
        y = (y + 1) % FRAME_HEIGHT;
        switch(dvi_modeInfo.mode) {
            //
            //    Mode is 640x240x8 colours as 3 bitplanes.
            //
            case DVI_MODE_640_240_8:
                queue_remove_blocking_u32(&dvi0.q_tmds_free, &tmdsbuf);
                for (uint component = 0; component < 3; ++component) {
                tmds_encode_custom_1bpp((const uint32_t*)(framebuf+(y/2)*640/8 + component * dvi_modeInfo.bitPlaneSize),
                                        tmdsbuf + (2-component) * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD,   // The (2-x) here makes it BGR Acordn standard
                                        FRAME_WIDTH);
                }
                queue_add_blocking_u32(&dvi0.q_tmds_valid, &tmdsbuf);
                break;

            //
            //    Mode is 640x480x8 colours as 3 bitplanes.
            //
            case DVI_MODE_640_480_8:
                queue_remove_blocking_u32(&dvi0.q_tmds_free, &tmdsbuf);
                for (uint component = 0; component < 3; ++component) {
                tmds_encode_custom_1bpp((const uint32_t*)(framebuf+y*640/8 + component * dvi_modeInfo.bitPlaneSize),
                                        tmdsbuf + (2-component) * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD,   // The (2-x) here makes it BGR Acordn standard
                                        FRAME_WIDTH);
                }
                queue_add_blocking_u32(&dvi0.q_tmds_valid, &tmdsbuf);
                break;

            //
            //    Mode is 320x240x8 colours as 3 bitplanes, also supports 320x256x8 for BBC Micro compatibility
            //
            case DVI_MODE_320_240_8:
            case DVI_MODE_320_256_8:
                queue_remove_blocking_u32(&dvi0.q_tmds_free, &tmdsbuf);
                for (uint component = 0; component < 3; ++component) {
                    uint8_t *_source = framebuf+(y0)*320/8 + component * dvi_modeInfo.bitPlaneSize;
                    uint16_t *_target = (uint16_t *)_buffer;

                    for (int i = 0;i < 320/8;i++) {
                    *_target++ = _mapping[*_source++];
                    }

                    tmds_encode_custom_1bpp((const uint32_t*)_buffer,
                                            tmdsbuf + (2-component) * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD,   // The (2-x) here makes it BGR Acordn standard
                                            FRAME_WIDTH);
                }
                queue_add_blocking_u32(&dvi0.q_tmds_valid, &tmdsbuf);

                if (dvi_modeInfo.mode == DVI_MODE_320_256_8 && (y1==14))  {         // This squeezes 256 lines into 480 :)
                    y0 += 1; y1 = 0;
                } else {
                    if (y1 & 1) {
                        y0+=1;
                    }
                    y1 +=1;
                }
                if (y == 479) {
                    y0 = 0;y1 = 0;
                }
                break;

            //
            //    Mode is 640x480x1 colour as 1 bitplanes.
            //
            case DVI_MODE_640_480_2:
                queue_remove_blocking_u32(&dvi0.q_tmds_free, &tmdsbuf);
                uint32_t * _source = (uint32_t*)(framebuf+y*640/8);
                uint32_t * _target = (uint32_t*) _buffer;
                    for (int i = 0; i < 20; i++) {
                        *_target++ = ~*_source++;
                    }

                for (uint component = 0; component < 3; ++component) {              // 3 bitplanes RGB, each is bitplane, ~bitplane, all 0 all 1
                    uint32_t *_target = NULL;
                    switch ((mono_fg_bg >> component) & 0x11) {
                        case 0x00:
                            _target = all_zero;break;
                        case 0x01:
                            _target = (uint32_t*)(framebuf+y*640/8);break;
                        case 0x10:
                            _target = (uint32_t  *)_buffer;break;
                        case 0x11:
                            _target = all_one;break;
                    }
                    tmds_encode_custom_1bpp(
                        (uint32_t*)_target,
                        tmdsbuf + (2-component) * FRAME_WIDTH/DVI_SYMBOLS_PER_WORD, // The (2-x) here makes it BGR Acordn standard
                        FRAME_WIDTH);
                }
                queue_add_blocking_u32(&dvi0.q_tmds_valid, &tmdsbuf);
                break;
            //
            //    Mode is 320x240x64 colours
            //
            case DVI_MODE_320_240_64:
                queue_remove_blocking_u32(&dvi0.q_tmds_free, &tmdsbuf);
                for (uint component = 0; component < 3; ++component) {
                    tmds_encode_custom_2bpp((const uint32_t*)(framebuf+(y/2)*640/8 + component * dvi_modeInfo.bitPlaneSize),
                                            tmdsbuf + (2-component) * FRAME_WIDTH / DVI_SYMBOLS_PER_WORD,   // The (2-x) here makes it BGR Acordn standard
                                            FRAME_WIDTH);
                }
                queue_add_blocking_u32(&dvi0.q_tmds_valid, &tmdsbuf);
            break;

            default:
                break;
        }
    }
}


/**
 * @brief      Start the DVI driver on Core1.
 */
void DVIStart(void) {
    DVISetMode(DVI_MODE_640_240_8);
    //DVISetMode(DVI_MODE_320_240_8);
    //DVISetMode(DVI_MODE_320_256_8);
    //DVISetMode(DVI_MODE_640_480_2);
    //DVISetMode(DVI_MODE_320_240_64);

    vreg_set_voltage(VREG_VSEL);                                                    // Set CPU voltage
    sleep_ms(10);                                                                   // Let it settle for 0.01s
    set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);                                // Set the DVI compatible clock speed
    setup_default_uart();                                                           // Initialise the UART

    dvi0.timing = &DVI_TIMING;                                                      // Select timing and pinout
    dvi0.ser_cfg = *DVIGetHDMIConfig();

    dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());    // Initialise DVI
    multicore_launch_core1(dvi_core1_main);                                         // Run DVI driver on core #1
}
