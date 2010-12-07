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

#define Z80_quit  1
#define Z80_NMI   2
#define Z80_reset 3
#define Z80_load  4
#define Z80_save  5
#define Z80_log   6

extern int hsize,vsize;
extern int interrupted;
extern unsigned char scrnbmp[],scrnbmp_old[];
extern unsigned long tstates,tsmax,frames;
extern int ay_reg;

extern void vsync_raise(void);
extern void vsync_lower(void);
extern void mainloop();
#ifdef SZ81	/* Added by Thunor */
extern void z80_init(void);
#endif

#define fetch(x) (memptr[(unsigned short)(x)>>10][(x)&1023])
#define fetch2(x) ((fetch((x)+1)<<8)|fetch(x))

/* due to timing constraints, we presume that only one-byte stores
 * are used by programs for memory-mapped AY addons.
 *
 * Also presume memory-mapped ones don't intercept, just connect
 * (if they don't read, that would be fine).
 */
#ifdef OSS_SOUND_SUPPORT
#define AY_STORE_CHECK(x,y) \
          if(sound_ay) {\
             if(sound_ay_type==AY_TYPE_QUICKSILVA) {\
                switch(x){\
                   case 0x7fff:\
                      ay_reg=((y)&15); break;\
                   case 0x7ffe:\
                      sound_ay_write(ay_reg,(y));\
                }\
             }\
          }
#else
#define AY_STORE_CHECK(x,y)  /* nothing */
#endif

#define store(x,y) do {\
          unsigned short off=(x)&1023;\
          unsigned char page=(unsigned short)(x)>>10;\
          int attr=memattr[page];\
          AY_STORE_CHECK(x,y) \
          if(attr){\
             memptr[page][off]=(y);\
             }\
           } while(0)

#define store2b(x,hi,lo) do {\
          unsigned short off=(x)&1023;\
          unsigned char page=(unsigned short)(x)>>10;\
          int attr=memattr[page];\
          if(attr) { \
             memptr[page][off]=(lo);\
             memptr[page][off+1]=(hi);\
             }\
          } while(0)

#define store2(x,y) store2b(x,(y)>>8,(y)&255)

#ifdef __GNUC__
static void inline storefunc(unsigned short ad,unsigned char b){
   store(ad,b);
}
#undef store
#define store(x,y) storefunc(x,y)

static void inline store2func(unsigned short ad,unsigned char b1,unsigned char b2){
   store2b(ad,b1,b2);
}
#undef store2b
#define store2b(x,hi,lo) store2func(x,hi,lo)
#endif

#define bc ((b<<8)|c)
#define de ((d<<8)|e)
#define hl ((h<<8)|l)
