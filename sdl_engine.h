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
#include <stdlib.h>
#include <string.h>
#include "sdl.h"
#include "sdl_hotspots.h"
#include "sdl_input.h"
#include "sdl_resources.h"
#include "sdl_video.h"

/* Defines */
/* Uncomment all of these to view the inner workings. Then monitor
 * the console output and press alt+r to cycle the resolutions.
#define SDL_DEBUG_VIDEO
#define SDL_DEBUG_TIMING
#define SDL_DEBUG_EVENTS
#define SDL_DEBUG
#define SDL_DEBUG_HOTSPOTS
#define SDL_DEBUG_RCFILE
#define SDL_DEBUG_FONTS */

#define TRUE 1
#define FALSE 0
#define UNDEFINED -1

/* Component IDs */
#define COMP_EMU 1
#define COMP_LOAD 2
#define COMP_VKEYB 4
#define COMP_CTB 8
#define COMP_ALL (COMP_EMU | COMP_LOAD | COMP_VKEYB | COMP_CTB)

/* Emulator variables I require access to */
#ifdef OSS_SOUND_SUPPORT
extern int sound_hz;
#endif
extern int nosound_hz, scrn_freq, invert_screen, zx80;
extern int load_selector_state, refresh_screen, ignore_esc;
extern int sound;
extern volatile int signal_int_flag;

/* Variables */
struct {
	int xoffset;
	int yoffset;
	SDL_TimerID timer_id;
} emulator;

/* Emulator functions I require access to */
extern void exit_program(void);
extern void reset81(void);

/* Function prototypes */
void component_executive(void);
Uint32 emulator_timer (Uint32 interval, void *param);





