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
#include "sdl.h"
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
#define COMP_ALL ((COMP_RUNOPTS1 - 1) | COMP_RUNOPTS1)

/* Emulator variables I require access to */
extern int scrn_freq, invert_screen, zx80, memory_size;
extern int load_selector_state, refresh_screen, ignore_esc;
extern int sound, sixteenbit;
extern volatile int signal_int_flag;
extern char *zxpfilename;

/* Variables */
char workdir[256];

struct {
	int state;
	int xoffset;
	int yoffset;
} runtime_options0;

struct {
	int state;
	int xoffset;
	int yoffset;
} runtime_options1;

/* Emulator functions I require access to */
extern void exit_program(void);
extern void reset81(void);
extern void initmem(void);
#ifdef OSS_SOUND_SUPPORT
	extern void sound_ay_setvol(void);
#endif

/* Function prototypes */
void component_executive(void);
int get_active_component(void);
Uint32 emulator_timer (Uint32 interval, void *param);





