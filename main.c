/* z81/xz81, Linux console and X ZX81/ZX80 emulators.
 * Copyright (C) 1994 Ian Collier. z81 changes (C) 1995-2001 Russell Marks.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <vga.h>
#include <vgakeyboard.h>
#include "common.h"
#include "sound.h"
#include "z80.h"
#include "allmain.h"



int hsize=ZX_VID_VGA_WIDTH,vsize=ZX_VID_VGA_HEIGHT;
unsigned char *vptr;


void exit_program(void)
{
#ifdef OSS_SOUND_SUPPORT
sound_end();
#endif
zxpclose();
keyboard_close();
vga_setmode(TEXT);
exit(0);
}


/* redraw the screen */
void update_scrn(void)
{
int x,y,a,mask;
unsigned char *ptr,*optr,d;

for(y=0;y<ZX_VID_VGA_HEIGHT;y++)
  {
  ptr=scrnbmp+(y+ZX_VID_VGA_YOFS)*ZX_VID_FULLWIDTH/8+ZX_VID_VGA_XOFS/8;
  optr=scrnbmp_old+(ptr-scrnbmp);
  for(x=0;x<ZX_VID_VGA_WIDTH;x+=8,ptr++,optr++)
    {
    d=*ptr;
    
    if(d!=*optr || refresh_screen)
      {
      if(invert_screen)
        d=~d;
      for(a=0,mask=128;a<8;a++,mask>>=1)
        vptr[y*320+x+a]=((d&mask)?0:15);
      }
    }
  }

/* now, copy new to old for next time */
memcpy(scrnbmp_old,scrnbmp,ZX_VID_FULLHEIGHT*ZX_VID_FULLWIDTH/8);

refresh_screen=0;
}


/* read keyboard and update keyports[] */
void check_events()
{
int b,y;
char *keymap=keyboard_getstate();

keyboard_update();

if(keyboard_keypressed(SCANCODE_ESCAPE) && !ignore_esc)
  reset81();

if(keyboard_keypressed(SCANCODE_F10))
  {
  while(keyboard_keypressed(SCANCODE_F10)) usleep(20000),keyboard_update();
  exit_program();	/* F10 = quit */
  }

if(keyboard_keypressed(SCANCODE_F1))
  {
  help=!help;
  while(keyboard_keypressed(SCANCODE_F1)) usleep(20000),keyboard_update();
  }

/* ugly, but there's no pleasant way to do this */

/* XXX ideally want to support DEL etc. */

for(y=0;y<8;y++)		/* 8 half-rows */
  {
  b=0;	/* we set bits in b as appropriate */
  switch(y)	/* below comments given in order b1->b5 */
    {
    /* left-hand side */
    case 0:	/* sft,z,x,c,v */
      if(keymap[SCANCODE_LEFTSHIFT] ||
         keymap[SCANCODE_RIGHTSHIFT] ||
         keymap[SCANCODE_LEFTCONTROL] ||
         keymap[SCANCODE_RIGHTCONTROL])
        b|=1;
      if(keymap[SCANCODE_Z]) b|=2;
      if(keymap[SCANCODE_X]) b|=4;
      if(keymap[SCANCODE_C]) b|=8;
      if(keymap[SCANCODE_V]) b|=16;
      break;
    case 1:	/* a,s,d,f,g */
      if(keymap[SCANCODE_A]) b|=1;
      if(keymap[SCANCODE_S]) b|=2;
      if(keymap[SCANCODE_D]) b|=4;
      if(keymap[SCANCODE_F]) b|=8;
      if(keymap[SCANCODE_G]) b|=16;
      break;
    case 2:	/* q,w,e,r,t */
      if(keymap[SCANCODE_Q]) b|=1;
      if(keymap[SCANCODE_W]) b|=2;
      if(keymap[SCANCODE_E]) b|=4;
      if(keymap[SCANCODE_R]) b|=8;
      if(keymap[SCANCODE_T]) b|=16;
      break;
    case 3:	/* 1,2,3,4,5 */
      if(keymap[SCANCODE_1]) b|=1;
      if(keymap[SCANCODE_2]) b|=2;
      if(keymap[SCANCODE_3]) b|=4;
      if(keymap[SCANCODE_4]) b|=8;
      if(keymap[SCANCODE_5]) b|=16;
      break;

    /* right-hand side */
    case 4:	/* 0,9,8,7,6 */
      if(keymap[SCANCODE_0]) b|=1;
      if(keymap[SCANCODE_9]) b|=2;
      if(keymap[SCANCODE_8]) b|=4;
      if(keymap[SCANCODE_7]) b|=8;
      if(keymap[SCANCODE_6]) b|=16;
      break;
    case 5:	/* p,o,i,u,y */
      if(keymap[SCANCODE_P]) b|=1;
      if(keymap[SCANCODE_O]) b|=2;
      if(keymap[SCANCODE_I]) b|=4;
      if(keymap[SCANCODE_U]) b|=8;
      if(keymap[SCANCODE_Y]) b|=16;
      break;
    case 6:	/* ent,l,k,j,h */
      if(keymap[SCANCODE_ENTER]) b|=1;
      if(keymap[SCANCODE_L]) b|=2;
      if(keymap[SCANCODE_K]) b|=4;
      if(keymap[SCANCODE_J]) b|=8;
      if(keymap[SCANCODE_H]) b|=16;
      break;
    case 7:	/* spc,dot,m,n,b */
      if(keymap[SCANCODE_SPACE]) b|=1;
      if(keymap[SCANCODE_PERIOD]) b|=2;
      if(keymap[SCANCODE_M]) b|=4;
      if(keymap[SCANCODE_N]) b|=8;
      if(keymap[SCANCODE_B]) b|=16;
      break;
    }
  
  keyports[y]=((b^31)|0xe0);	/* some things need top bits to be 1 */
  }
}



int main(int argc,char *argv[])
{
vga_init();

parseoptions(argc,argv);

initmem();
loadhelp();
zxpopen();

vga_setmode(G320x200x256);
vptr=vga_getgraphmem();
refresh_screen=1;
keyboard_init();
keyboard_translatekeys(DONT_CATCH_CTRLC);

#ifdef OSS_SOUND_SUPPORT
if(sound)
  sound_init();
#endif
startsigsandtimer();
mainloop();

/* not reached, but just in case */
exit_program();
return(0);	/* make -Wall happy */
}
