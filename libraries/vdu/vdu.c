/**
 * @file       vdu.c
 *
 * @brief      VDU driver, control codes including plot &c.
 *
 * @author     Lennart Benschop
 *
 * @date       18/01/2025
 *
 */


#include "common.h"
#include <libraries.h>

/* Number of parameter byes expected for each control code 0..31
   See https://beebwiki.mdfs.net/VDU */
static const uint8_t vdu_param_bytes[] = {
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 2, 5, 0, 0, 1, 9, 8, 5, 0, 0, 4, 4, 0, 2,
};

char last_cmd = 0;
unsigned char params[9];
int param_index = 0;
int expect_params = 0;
int vdu_fg = 3;
int vdu_bg = 0;

GFXPort vp = {0,0,640,240,0,0};


static void switch_ctrl(unsigned char c)
{
  switch(c) {
  case 8:
  case 10:
  case 12:
    CONWrite(c);
    break;
  case 17:
    if (params[0] > 128)
      vdu_bg = params[0] & 0x3f;
    else
      vdu_fg = params[0] & 0x3f;
    CONSetColour(vdu_fg, vdu_bg);
    break;
  case 22:
    if (params[0] <= 4) {
      if (params[0] == 2) {
	DVISetMonoColour(7,0);
      }
      DVISetMode(params[0]);
      CONWrite(12);
      vdu_fg = 7;
      vdu_bg = 0;
      CONSetColour(vdu_fg, vdu_bg);      
    }
    {
      struct DVIModeInformation *dmi = DVIGetModeInformation();
      vp.x = 0;
      vp.y = 0;
      vp.width = dmi->width;
      vp.height = dmi->height;
      vp.xOffset = 0;
      vp.yOffset = 0;
    }
    break;
  case 23:
    if (params[0] >= 128) {
      CONDefineUDG((params[0]&31)+224, &params[1]);
    }
    break;
  case 30:
    CONSetCursor(0,0);
    break;
  case 31:
    CONSetCursor(params[0],params[1]);
    break;
  }
}

/**
 * @brief      Write one character or control code.
 *
 * @param[in]  c     Character code
 */
void VDUWrite(unsigned char c)
{
  if (expect_params) {
    params[param_index++] = c;
    if (param_index == expect_params) {
      expect_params = 0;
      param_index = 0;
      switch_ctrl(last_cmd);
    }
  } else {
    if (c<=31) {
      if (vdu_param_bytes[c]) {
	last_cmd = c;
	expect_params = vdu_param_bytes[c];
      } else {
	switch_ctrl(c);
      }
    } else if (c>=128) {
      CONWrite(224 + (c&0x1f));
    } else {
      CONWrite(c);
    }	                      
  }
}

/**
 * @brief      Write string to VDU
 *
 * @param[in]  fmt        The format (as printf)
 * @param[in]  <unnamed>  Varags paramete
 */
void VDUWriteString(const char *fmt, ...)
{
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, 128, fmt, args);
    uint8_t *p = (uint8_t *)buf;
    while (*p != '\0') VDUWrite(*p++);
    va_end(args);
}

/**
 * @brief       Get console text cursor position.
 *
 * @param[out]  x  column of the cursor.
 * @param[out]  y  row of the cursor.
 */
void VDUGetCursor(int *x, int *y)
{
  CONGetCursor(x,y);
}

/**
 * @brief       Invert character cell at cursor position to make cursore visible.
 */
void VDUSwitchCursor(void)
{
  int x,y;
  CONGetCursor(&x,&y);
  GFXFillRect(&vp,x*8,y*8,x*8+7,y*8+7,0x10000+63);
}
