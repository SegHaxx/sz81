/* sz81 Copyright (C) 2007-2011 Thunor <thunorsif@hotmail.com>
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

/* Includes */
#include <sys/stat.h>
#include <ctype.h>	/* Needed for toupper() only */
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "sdl.h"
#include "sdl_hotspots.h"
#include "sdl_input.h"
#include "sdl_loadsave.h"
#include "sdl_resources.h"
#include "sdl_sound.h"
#include "sdl_video.h"
#ifdef __amigaos4__
	#include "amiga.h" /* Amiga-specifics */
#endif

/* Defines */
/* Uncomment all of these to view the inner workings. Then monitor
 * the console output and press alt+r to cycle the resolutions.
#define SDL_DEBUG_VIDEO
#define SDL_DEBUG_TIMING
#define SDL_DEBUG_EVENTS
#define SDL_DEBUG_HOTSPOTS
#define SDL_DEBUG_RCFILE
#define SDL_DEBUG_FONTS
#define SDL_DEBUG_SOUND
#define SDL_DEBUG_JOYSTICK
#define SDL_DEBUG_COM_LINE
*/

#define TRUE 1
#define FALSE 0
#define UNDEFINED -1

/* Component IDs */
#define COMP_EMU 1
#define COMP_LOAD 2
#define COMP_VKEYB 4
#define COMP_CTB 8
#define COMP_RUNOPTS0 16
#define COMP_RUNOPTS1 32
#define COMP_RUNOPTS2 64
#define COMP_RUNOPTS3 128
#define COMP_RUNOPTS_ALL (COMP_RUNOPTS0 | COMP_RUNOPTS1 | COMP_RUNOPTS2 | COMP_RUNOPTS3)
#define COMP_LDFILE 256
#define COMP_SSTATE 512
#define COMP_DIALOG 1024
#define COMP_ALL ((COMP_DIALOG - 1) | COMP_DIALOG)

/* TODO: Emulator variables I require access to */
/* Variables from the top of z80.c */
extern unsigned long tstates;
extern unsigned long frames;
extern int vsy;
extern unsigned long linestart;
extern int vsync_toggle, vsync_lasttoggle;
/* Variables liberated from the top of mainloop */
extern unsigned long nextlinetime, linegap, lastvsyncpend;
extern unsigned char intsample;
extern unsigned char op;
extern int ulacharline;
extern int nmipend, intpend, vsyncpend, vsynclen;
extern int hsyncskip;
extern int framewait;
/* Variables from the top of common.c */
extern unsigned char mem[];
extern int sound, sound_vsync;
extern int sound_ay, sound_ay_type;
extern int signal_int_flag;
extern int interrupted;
extern int nmigen, hsyncgen, vsync;
extern char *zxpfilename;
extern int load_selector_state;
extern int refresh_screen;
/* Variables from the top of sound.c */
extern int sound_stereo, sound_stereo_acb, sound_ay_unreal;

/* Variables */
extern char startdir[256];

typedef struct runtimeoptions {
	int state;
	int xoffset;
	int yoffset;
	char **text;
} runtimeoptions_;

extern runtimeoptions_ runtime_options[MAX_RUNTIME_OPTIONS];

/* Emulator functions I require access to */
extern void initmem(void);
extern void frame_pause(void);
extern void do_interrupt(void);
#ifdef OSS_SOUND_SUPPORT
	extern void sound_ay_setvol(void);
	extern void sound_framesiz_init(void);
        extern void sound_end(void);
#endif

/* Function prototypes */
int get_active_component(void);
int runtime_options_which(void);
void emulator_reset(void);
int emulator_hold(int *condition);
void emulator_exit(void);
Uint32 emulator_timer (Uint32 interval, void *param);




