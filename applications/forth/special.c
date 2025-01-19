/* SOD32 the Stack Oriented Design computer.
   Copyright 1994, L.C. Benschop, Eindhoven, The Netherlands.
   The program is released under the GNU General Public License version 2.
   There is NO WARRANTY.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sod32.h"
#include "common.h"
#include <libraries.h>

#define SWAP() swap_mem(addr&CELLMASK,len+3);
#define CLIP() do {addr&=MEMMASK;len&=MEMMASK;if(addr+len>MEMSIZE)len=MEMSIZE-addr;} while(0)
#define FILEID(n) do{if (((n) & 0x80000000)==0)fp=(n);else fp=0;}while(0);

char *filemodes[]={"r","rb","w","wb","r+","r+b"};
char filename[256];

int make_name(char *addr,UNS32 len)
{
 int i;
 for(i=0;i<len && i<256;i++) {
  filename[i]=addr[i];
 }
 filename[i]='\0';
 return 1; 
}

int lastkey;
int getch(void)
{
  int k;
  if (lastkey != 0) {
    k=lastkey;
    lastkey = 0;
  } else {
    do {
      check_timer();
      if (interrupt == 100) return 0;
      k = KBDGetKey();
    } while (k==0);
  }
  return k;
}

int kbhit(void)
{
  if (lastkey != 0)
    return -1;
  else {
    lastkey = KBDGetKey();
    return lastkey != 0;
  };
}

void putch(int c)
{
    VDUWrite(c);
}

void check_timer(void)
{
    if (HASTICK50_FIRED()) {                                                    // Time to do a 50Hz tick (Don't use this for timing !)
      TICK50_RESET();                                                         // Reset the tick flag
      if (USBUpdate() == 0) {
	interrupt = 100; return;	
      }// Update USB
      KBDCheckTimer();
      if (KBDEscapePressed(true)) {                                               // Escaped ?
	interrupt = 12;
      } else {
	interrupt = 16;
      }
    // Check for keyboard repeat
    }
}
  

/* Replacement for fgets as the one in AgDev appears to be broken,
   does not detect EOF on real machine */
static char * my_fgets(char *s, unsigned int maxlen, UNS32 f)
{
  unsigned int i;
  char c;
  for(i=0; i<maxlen-1;) {
    if (FIORead(f,&c,1) <= 0) break;
    if (c=='\r') continue;
    s[i] = c;
    i++;
    if (c=='\n') break;
  }
  s[i] = 0;
  if (i==0)
    return 0;
  else
    return s;
}


void do_os(void)
{
 UNS32 addr,len,ior=0;
 int res;
 UNS32 fp;
 UNS32 code=CELL(save_sp);save_sp+=4;
 switch(code) {
  case 0: /*exit*/ interrupt = 101; break;
  case 1: /*read char */ save_sp-=4;CELL(save_sp)=getch();break; 
  case 2: /*print char*/ putch(CELL(save_sp));save_sp+=4;break;
  case 3: /*check key */ save_sp-=4;CELL(save_sp)=-kbhit();break;
  case 4: /* set-term */ save_sp+=4;break;
  case 7: /*open-file*/  save_sp+=4;len=CELL(save_sp);addr=CELL(save_sp+4);
    CLIP(); SWAP(); if(!make_name((char*)(mem+addr),len)) {
      ior=-202;
      SWAP(); goto end;
    } SWAP();
    {
      if(CELL(save_sp-4)>5){ior=-203;goto end;}
      //CONWriteString("Filename: %s\n",filename);
      fp=FIOOpen(filename);
      //ONWriteString("Open res: %d\n",fp);
      if(fp & 0x80000000)ior=200;
      CELL(save_sp+4)=fp;                         
    } goto end;   
 case 8: /*close-file*/  FILEID(CELL(save_sp));FIOClose(fp); 
  			 goto end;;
 case 9: /*write-line*/ save_sp+=8;FILEID(CELL(save_sp-8)); len=CELL(save_sp-4);
    addr=CELL(save_sp);CLIP(); SWAP();
    FIOWrite(fp,(char*)(mem+addr),len);SWAP();
    FIOWrite(fp,"\r\n",2);
    goto end; 
 case 10: /*read-line*/ FILEID(CELL(save_sp)); len=CELL(save_sp+4)+2;
   addr=CELL(save_sp+8);CLIP(); SWAP();
   if (my_fgets((char*)(mem+addr),len,fp)==NULL) {
     SWAP();
     //CONWriteString("READ-LINE returns NULL\n");
     res = -1;
   } else {
     res = strlen((char*)(mem+addr));
     if(*((char*)(mem+addr)+res-1)=='\n') res--; 
     //CONWriteString("READ-LINE returns string length %d\n",res);
     SWAP();
   }
   if(res<0) {
     res=0;
     CELL(save_sp+4)=0; 
   } else {
     CELL(save_sp+4)=0xffffffff;
   }
   CELL(save_sp+8)=res;
   goto end;
 case 11: /*write-file*/save_sp+=8;FILEID(CELL(save_sp-8)); len=CELL(save_sp-4);
    addr=CELL(save_sp);CLIP(); SWAP();
    res = FIOWrite(fp,(char*)(mem+addr),len);SWAP(); if(res<0)ior=-200; 
    goto end; 
 case 12: /*read-file*/ save_sp+=4;FILEID(CELL(save_sp-4)); len=CELL(save_sp);
   addr=CELL(save_sp+4);CLIP(); SWAP();
   res=FIORead(fp,(char*)(mem+addr),len);SWAP(); 
   CELL(save_sp+4)=res;if(res<0) ior=-200;goto end;
  case 13: /*delete-file*/ save_sp+=4;len=CELL(save_sp-4);addr=CELL(save_sp);
    CLIP(); SWAP(); if(!make_name((char*)(mem+addr),len)) {ior=-202;
      SWAP(); goto end;} SWAP(); FIODeleteFile(filename); goto end;
#if 0			 
 case 14: /*reposition-file*/ save_sp+=8;FILEID(CELL(save_sp-8)); 
                          fseek(fp,CELL(save_sp),SEEK_SET);goto end;
  
 case 15: /*file-position*/ save_sp-=8;FILEID(CELL(save_sp+8));
                          CELL(save_sp+8)=ftell(fp);CELL(save_sp+4)=0;
                          goto end;
 case 17: /* file-size*/ save_sp-=8;FILEID(CELL(save_sp+8));
                          {long oldpos;
                           oldpos=ftell(fp); 
                           fseek(fp,0L,SEEK_END);
                           CELL(save_sp+8)=ftell(fp);CELL(save_sp+4)=0;
                           fseek(fp,oldpos,SEEK_SET);
                          }
                          goto end;
#endif			  
 } return;
 end: CELL(save_sp)=ior;
}

void do_special(UNS32 code) /* execute a special instruction */
{
 switch(code) {
  case 0: /* sp@ */ save_sp-=4;CELL(save_sp)=save_sp+4;break;
  case 1: /* sp! */ save_sp=CELL(save_sp)&CELLMASK;break;
  case 2: /* rp@ */ save_sp-=4;CELL(save_sp)=save_rp;break;
  case 3: /* rp! */ save_rp=CELL(save_sp)&CELLMASK;save_sp+=4;break;
  case 32: /* trap0 */ do_os();break; 
  case 50: /* setalarm */ save_sp+=4;break;
 }
}

