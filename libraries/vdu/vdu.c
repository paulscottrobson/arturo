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

typedef struct _Point {
  short x, y;
} Point_t;

Point_t p1, p2, p3;

int max_x=1279, max_y=959, scale_x=1, scale_y=2;


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
int gfx_fg = 7;
int gfx_bg = 0;
int mono_fg = 7;
int mono_bg = 0;

GFXPort vp = {0,0,640,240,0,0};

#define SCALE_X(x) ((x) >> scale_x)
#define SCALE_Y(y) ((max_y - (y)) >> scale_y)

static void do_plot(unsigned char mode, short x, short y)
{
  unsigned int colour;
  //CONWriteString("PLOT %d,%d,%d\n",mode,x,y);
  if (mode & 0x4) {
    p1.x = x;
    p1.y = y;
  } else {
    p1.x = p2.x + x;
    p1.y = p2.y + y;
  }
  if ((mode & 0x3)) {
    switch (mode & 0x3) {
    case 1: colour = gfx_fg; break;
    case 2: colour = 0x10000+63; break;
    case 3: colour = gfx_bg; break;
    }
    switch (mode >> 3) {
    case 0: /* Line */
      GFXLine(&vp, SCALE_X(p2.x), SCALE_Y(p2.y),
	      SCALE_X(p1.x), SCALE_Y(p1.y),colour);
      break;
    case 8: /* Plot single dot */
      GFXPlot(&vp, SCALE_X(p1.x), SCALE_Y(p1.y), colour);
      break;
    case 10: /* Triangle */
      GFXFillTriangle(&vp, SCALE_X(p3.x), SCALE_Y(p3.y),
		      SCALE_X(p2.x), SCALE_Y(p2.y),
		      SCALE_X(p1.x), SCALE_Y(p1.y),colour);
      break;
    }
      
  }
  p3 = p2;
  p2 = p1;
}


static void switch_ctrl(unsigned char c)
{
  switch(c) {
  case 8:  /* backspace */
  case 10: /* Newline */
  case 12: /* clear screen */
    CONWrite(c);
    break;
  case 17: /* Set text colour */
    if (params[0] >= 128)
      vdu_bg = params[0] & 0x3f;
    else
      vdu_fg = params[0] & 0x3f;
    CONSetColour(vdu_fg, vdu_bg);
    break;
  case 18: /* Set graphics colour */
    if (params[1] >= 128)
      gfx_bg = params[1] & 0x3f;
    else
      gfx_fg = params[1] & 0x3f;      
    break;
  case 19: /* Set palette */
    if (params[0] == 0)
      mono_bg = params[1];
    else
      mono_fg = params[1];
    DVISetMonoColour(mono_fg,mono_bg);      
    break; 
  case 22: /* Switch mode */
    if (params[0] <= 4) {
      if (params[0] == 2) {
	DVISetMonoColour(7,0);
      }
      DVISetMode(params[0]);
      CONWrite(12);
      vdu_fg = 7;
      vdu_bg = 0;
      gfx_fg = 7;
      gfx_bg = 0;
      mono_fg = 7;
      mono_bg = 0;
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
      max_x = 1279;
      if (dmi->height == 256) max_y = 1023; else max_y = 959;
      if (dmi->width == 320) scale_x = 2; else scale_x = 1;
      if (dmi->height < 400) scale_y = 2; else scale_y = 1;
      p1.x=0; p1.y=0;
      p2.x=0; p2.y=0; 
      p3.x=0; p3.y=0;       
    }
    break;
  case 23: /* Define UDG */
    if (params[0] >= 128) {
      CONDefineUDG((params[0]&31)+224, &params[1]);
    }
    break;
  case 25: /* PLOT */
    do_plot(params[0],
	    (short)(params[1] + 256*params[2]),
	    (short)(params[3] + 256*params[4]));
    break;
  case 30: /* HOME cursor */
    CONSetCursor(0,0);
    break;
  case 31: /* TAB(x,y) */
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
