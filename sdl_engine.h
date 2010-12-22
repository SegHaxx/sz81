/* sz81 Copyright (C) 2007-2010 Thunor <thunorsif@hotmail.com>
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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sdl.h"
#include "sdl_dialogs.h"
#include "sdl_hotspots.h"
#include "sdl_input.h"
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
#define COMP_ALL ((COMP_RUNOPTS3 - 1) | COMP_RUNOPTS3)

/* Machine models */
#define MODEL_ZX81 0
#define MODEL_ZX80 1

/* Emulator variables I require access to */
extern int refresh_screen;
extern int load_selector_state;
extern int ignore_esc;	/* Only used within the load selector so it can eventually go temp temp */
extern int interrupted;
extern volatile int signal_int_flag;
extern char *zxpfilename;
extern int sound, sound_vsync;
extern int sound_ay, sound_ay_type;
extern int sound_stereo, sound_stereo_acb;

/* Variables */
char startdir[256];

struct runtimeoptions {
	int state;
	int xoffset;
	int yoffset;
	char **text;
};
struct runtimeoptions runtime_options[MAX_RUNTIME_OPTIONS];

/* Emulator functions I require access to */
extern void reset81(void);
extern void initmem(void);
#ifdef OSS_SOUND_SUPPORT
	extern void sound_ay_setvol(void);
	extern void sound_framesiz_init(void);
#endif

/* Function prototypes */
int get_active_component(void);
int runtime_options_which(void);
Uint32 emulator_timer (Uint32 interval, void *param);





