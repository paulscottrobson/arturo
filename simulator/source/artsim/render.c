/**
 * @file  render.c
 * 
 * @brief      Display Rendering
 *
 * @author     Paul Robson
 *
 * @date       07/01/2025
 *
 */

#include <artsim.h>

static uint8_t redPlane[640*480/8];
static uint8_t greenPlane[640*480/8];
static uint8_t bluePlane[640*480/8];

static struct DVIModeInformation dvi_modeInfo;
static int mode = DVI_MODE_640_240_8;


//
//                            Mode Palettes
//


static int palette[8] = { 0x000,0xF00,0x0F0,0xFF0,0x00F,0xF0F,0x0FF,0xFFF };

static int palette_mono[2] = {3, 4};

static int palette_64[64] = {
  0x000, 0x500, 0xA00, 0xF00,
  0x050, 0x550, 0xA50, 0xF50,
  0x0A0, 0x5A0, 0xAA0, 0xFA0,
  0x0F0, 0x5F0, 0xAF0, 0xFF0,

  0x005, 0x505, 0xA05, 0xF05,
  0x055, 0x555, 0xA55, 0xF55,
  0x0A5, 0x5A5, 0xAA5, 0xFA5,
  0x0F5, 0x5F5, 0xAF5, 0xFF5,

  0x00A, 0x50A, 0xA0A, 0xF0A,
  0x05A, 0x55A, 0xA5A, 0xF5A,
  0x0AA, 0x5AA, 0xAAA, 0xFAA,
  0x0FA, 0x5FA, 0xAFA, 0xFFA,
  
  0x00F, 0x50F, 0xA0F, 0xF0F,
  0x05F, 0x55F, 0xA5F, 0xF5F,
  0x0AF, 0x5AF, 0xAAF, 0xFAF,
  0x0FF, 0x5FF, 0xAFF, 0xFFF,
};

/**
 * @brief      Set colour for monochrome modes
 *
 * @param[in]  fg    The foreground
 * @param[in]  bg    The background
 */
void DVISetMonoColour(int fg, int bg)
{
  palette_mono[0] = fg & 0x7;
  palette_mono[1] = bg & 0x7;
}

/**
 * @brief      Set the current mode
 *
 * @param[in]  mode  The mode, an integer value
 *
 * @return     true if successful
 */
bool DVISetMode(int mode) {
  dvi_modeInfo.mode = mode;                             // Record mode
  dvi_modeInfo.bitPlaneCount = 3;
  dvi_modeInfo.bitPlane[0] = redPlane;
  dvi_modeInfo.bitPlane[1] = greenPlane;
  dvi_modeInfo.bitPlane[2] = bluePlane;
  dvi_modeInfo.bitPlaneDepth = 1;

  switch(mode) {
    case DVI_MODE_640_240_8:                            // 640x480x8 information.
      dvi_modeInfo.width = 640;dvi_modeInfo.height = 240;
      dvi_modeInfo.bitPlaneSize = sizeof(redPlane);
      break;
    case DVI_MODE_320_240_8:                            // 320x240x8 information.
      dvi_modeInfo.width = 320;dvi_modeInfo.height = 240;
      dvi_modeInfo.bitPlaneSize = sizeof(redPlane);
      break;
    case DVI_MODE_640_480_2:                            // 640x480x2 information.
      dvi_modeInfo.width = 640;dvi_modeInfo.height = 480;
      dvi_modeInfo.bitPlaneSize = sizeof(redPlane);
      dvi_modeInfo.bitPlaneCount = 1;
      break;
    case DVI_MODE_320_240_64:                           // 320x240x64 information.
      dvi_modeInfo.width = 320;dvi_modeInfo.height = 240;
      dvi_modeInfo.bitPlaneSize = sizeof(redPlane);
      dvi_modeInfo.bitPlaneDepth = 2;
      break;
    case DVI_MODE_320_256_8:                            // 320x256x8 information.
      dvi_modeInfo.width = 320;dvi_modeInfo.height = 256;
      dvi_modeInfo.bitPlaneSize = sizeof(redPlane);
      break;

    #if DVI_SUPPORT_640_480_8 == 1                      // Controllable (memory constraints)
    case DVI_MODE_640_480_8:                            // 640x480x8 information.
      dvi_modeInfo.width = 640;dvi_modeInfo.height = 480;
      dvi_modeInfo.bitPlaneSize = sizeof(redPlane);
      break;
    #endif

    default:
      dvi_modeInfo.mode = -1;                         // Failed.
  }
  dvi_modeInfo.bytesPerLine = dvi_modeInfo.width / 8 * dvi_modeInfo.bitPlaneDepth;                // Calculate bytes per line.  return &modeInfo;
  return true;
}

/**
 * @brief      Get information about the current mode
 *
 * @return     DVIModeInformation structure pointer with information about how the mode is set up.
 */
struct DVIModeInformation *DVIGetModeInformation(void) {
  return &dvi_modeInfo;
}


/**
 * @brief      Shorthand way of getting screen size, can return width and/or height in 2 referenced
 *
 * @param      pWidth   pointer to width or NULL
 * @param      pHeight  pointer to height or NULL
 *
 * @return     the current mode number
 */
int  DVIGetScreenExtent(int *pWidth,int *pHeight) { 
  if (pWidth != NULL) *pWidth = dvi_modeInfo.width;
  if (pHeight != NULL) *pHeight = dvi_modeInfo.height;
  return dvi_modeInfo.mode;
}

/**
 * @brief      Render the display
 *
 * @param      surface  The surface to render it on
 */
void RNDRender(SDL_Surface *surface) {  
  struct DVIModeInformation *dm = DVIGetModeInformation();
  uint8_t *pr,*pg,*pb,r,g,b;
  SDL_Rect rc;
  rc.x = rc.y = 0;rc.w = AS_SCALE*640+16;rc.h = AS_SCALE*480+16;
  SYSRectangle(&rc,0);
  for (int y = 0;y < dm->height;y++) {
    rc.w = AS_SCALE * 640/dm->width;
    rc.h = AS_SCALE * 480/dm->height;
    rc.y = y*rc.h+8;
    pr = dm->bitPlane[0]+y*dm->bytesPerLine;
    pg = dm->bitPlane[1]+y*dm->bytesPerLine;
    pb = dm->bitPlane[2]+y*dm->bytesPerLine;
    for (int x = 0;x < dm->width;x+= 8/dm->bitPlaneDepth) {
      rc.x = x*rc.w+8;
      r = *pr++;g = *pg++;b = *pb++;
      if (dm->bitPlaneDepth == 1) {
        for (int bt = 0;bt < 8;bt++) {
        uint8_t c = ((r & 0x80) >> 7)+((g & 0x80) >> 6)+((b & 0x80) >> 5);
        if (dm->bitPlaneCount == 1) c = (r & 0x80) ? palette_mono[0] : palette_mono[1];
        SYSRectangle(&rc,palette[c]);
        r <<= 1;g <<= 1;b <<= 1;
        rc.x += rc.w;
        }
      } else {
        for (int bt = 0;bt < 8;bt++) {
        uint8_t c = ((r & 0xC0) >> 6)+((g & 0xC0) >> 4)+((b & 0xC0) >> 2);
        SYSRectangle(&rc,palette_64[c]);
        r <<= 2;g <<= 2;b <<= 2;
        rc.x += rc.w;         
        }       
      }
    }
  } 
}
