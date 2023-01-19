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
 *
 *
 * common.h - prototypes etc. for common.c.
 */

/* And first, some display constants, as this seems the least
 * horrible place to put them.
 *
 * NB: *everything* horizontal here must be exactly divisible by 8.
 */

/* full internal image with overscan (but not hsync/vsync areas) */
#define ZX_VID_MARGIN		55
#define ZX_VID_HMARGIN		(8*8)
//#define ZX_VID_FULLWIDTH	(2*ZX_VID_HMARGIN+32*8)	/* sic */
//#define ZX_VID_FULLHEIGHT	(2*ZX_VID_MARGIN+192)
#define ZX_VID_FULLWIDTH 400
#define ZX_VID_FULLHEIGHT 300

/* ahem :-) */
#define FUDGE_FACTOR		(3*8)

/* X image */
#if 1
/* for testing QS Defender ;-) - I might want to add a
 * command-line option to show the whole overscan area
 * eventually...
 */
#define ZX_VID_X_XOFS		0
#define ZX_VID_X_YOFS		0
#define ZX_VID_X_WIDTH		ZX_VID_FULLWIDTH
#define ZX_VID_X_HEIGHT		ZX_VID_FULLHEIGHT
#else
#define ZX_VID_X_XOFS		(5*8-FUDGE_FACTOR)
#define ZX_VID_X_YOFS		35
#define ZX_VID_X_WIDTH		((32+3*2)*8)
#define ZX_VID_X_HEIGHT		(192+20*2)
#endif

/* svgalib image */
#define ZX_VID_VGA_XOFS		(4*8-FUDGE_FACTOR)
#define ZX_VID_VGA_YOFS		51
#define ZX_VID_VGA_WIDTH	((32+4*2)*8)
#define ZX_VID_VGA_HEIGHT	(192+4*2)


/* AY board types */
#define AY_TYPE_NONE		0
#define AY_TYPE_QUICKSILVA	1
#define AY_TYPE_ZONX		2


extern unsigned char mem[];
extern unsigned char *memptr[64];
extern int memattr[64];
extern unsigned char keyports[9];
extern unsigned long tstates, tsmax;
extern int borrow;
extern int help,sound,sound_vsync,sound_ay,sound_ay_type,vsync_visuals;
extern int invert_screen;

extern int interrupted;
extern int nmigen,hsyncgen,vsync;
extern int taguladisp;
extern int autoload;
extern int scrn_freq;
extern int fakedispx,fakedispy;

extern int refresh_screen;
extern int zx80;
extern int ignore_esc;

#ifndef SZ81	/* Added by Thunor */
extern void sighandler(int a);
extern void startsigsandtimer();
extern char *libdir(char *file);
extern void exit_program(void);
extern void loadhelp(void);
extern void save_p(int a);
extern void load_p(int a);
extern void reset81();
extern void parseoptions(int argc,char *argv[]);
#endif
extern void initmem();
extern void exitmem();
extern void zxpinit(void);
extern void zxpopen(void);
extern void zxpclose(void);
extern unsigned int in(int h,int l);
extern unsigned int out(int h,int l,int a);
extern void do_interrupt();
extern void update_kybd();
extern void do_interrupt();
extern void frame_pause(void);
#ifdef SZ81	/* Added by Thunor */
extern void common_reset(void);
#endif

extern int chromamode;
extern unsigned char bordercolour;

int printer_inout(int is_out,int val);
