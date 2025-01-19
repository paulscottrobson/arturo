/* SOD32 the Stack Oriented Design computer.
   Copyright 1994, L.C. Benschop, Eindhoven, The Netherlands.
   The program is released under the GNU General Public License version 2.
   There is NO WARRANTY.
*/

#include "image_data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sod32.h"

UNS8 mem[MEMSIZE+3];

UNS32 save_sp,save_ip,save_rp,interrupt;

void load_image(void)
{
  UNS32 len = sizeof(image_data);
  memcpy(mem, image_data,len);
  swap_mem(0,len); 
}
 


void ApplicationRun(void)
{
  do {
    load_image();
    virtual_machine();
  } while (interrupt != 100);
}
