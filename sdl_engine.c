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
#include "sdl_engine.h"

/* Defines */

/* Variables */

/* Function prototypes */
void clean_up_before_exit(void);


/***************************************************************************
 * VGA Initialise                                                          *
 ***************************************************************************/
/* Since this is the first function the emulator calls prior to parseoptions
 * (which could exit) many things are done here. Firstly some variables are
 * initialised with defaults, SDL is initialised and then a window manager
 * icon is loaded and a title set if the platform requires it.
 * 
 * This should be thought of now as sz81 and SDL_Init.
 * 
 * On exit: returns 0 on success or -1 on failure */

int vga_init(void) {
	int count;
	#if defined (PLATFORM_GP2X)
	#elif defined (PLATFORM_ZAURUS)
	#else
		char filename[256];
	#endif
	
	/* Set-up a default video resolution depending on platform.
	 * Eventually this will be configurable via the command line */
	#if defined(PLATFORM_GP2X)
		video.xres = 320; video.yres = 240; video.scale = 1;
		video.fullscreen = SDL_FULLSCREEN;
	#elif defined(PLATFORM_ZAURUS)
		video.xres = 240; video.yres = 320; video.scale = 1;
		video.fullscreen = SDL_FULLSCREEN;
	#else
		video.xres = 640; video.yres = 480; video.scale = 2;
		video.fullscreen = FALSE;
	#endif

	/* All SDL pointers are initialised to NULL here since we could
	 * exit prematurely and we don't want to cause seg faults by
	 * freeing nothing. As long as they are NULL everything is fine */
	emulator.timer_id = NULL;
	control_bar.scaled = NULL;
	vkeyb.original = vkeyb.scaled = NULL;
	sz81icons.original = sz81icons.scaled = NULL;
	zx80font.original = zx81font.original = zx82font.original = NULL;
	for (count = 0; count < MAX_FONTS; count++)
		zx80font.scaled[count] = zx81font.scaled[count] = 
			zx82font.scaled[count] = NULL;
	joystick = NULL;
	highlight = NULL;
	wm_icon = NULL;

	/* Initialise everything to a default here that could possibly be
	 * overridden by a command line option or from an rcfile */
	vkeyb.state = vkeyb.autohide = vkeyb.toggle_shift = FALSE;
	vkeyb.alpha = SDL_ALPHA_OPAQUE;

	joystick_dead_zone = JOYSTICK_DEAD_ZONE;
	
	colours.colour_key = 0xff0080;
	colours.bmf_fg_default = 0xffffff;
	colours.emu_fg = 0x0;
	colours.emu_bg = 0xffffff;
	colours.hs_load_selected = 0x00ff00;
	colours.hs_load_pressed = 0xffc000;
	colours.hs_vkeyb_zx80_selected = 0x00ff00;
	colours.hs_vkeyb_zx80_pressed = 0xffff00;
	colours.hs_vkeyb_zx80_toggle_pressed = 0xff4000;
	colours.hs_vkeyb_zx81_selected = 0x00ff00;
	colours.hs_vkeyb_zx81_pressed = 0xffc000;
	colours.hs_vkeyb_zx81_toggle_pressed = 0xff4000;
	colours.hs_ctb_selected = 0x00ff00;
	colours.hs_ctb_pressed = 0xffc000;

	/* Initialise SDL */
	if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		fprintf(stderr, "%s: Cannot initialise SDL: %s", __func__,
			SDL_GetError());
		return -1;
	}

	atexit(clean_up_before_exit);

	fprintf(stdout, "PACKAGE_DATA_DIR is %s\n", PACKAGE_DATA_DIR);

	/* Set WM icon and title if required for platform before
	 * setting a video mode as per SDL docs instructions */
	#if defined (PLATFORM_GP2X)
	#elif defined (PLATFORM_ZAURUS)
	#else
		strcpy (filename, PACKAGE_DATA_DIR "/");
		strcat (filename, IMG_WM_ICON);

		/* Load the bitmap */
		if ((wm_icon = SDL_LoadBMP(filename)) == NULL) {
			fprintf(stderr, "%s: Cannot load window manager icon %s: %s\n",
				__func__, filename, SDL_GetError ());
		} else {
			SDL_WM_SetIcon (wm_icon, NULL);
		}

		/* Set display window title */
		SDL_WM_SetCaption("sz81", "sz81");
	#endif

	return 0;
}

/***************************************************************************
 * Component Executive                                                     *
 ***************************************************************************/
/* This function monitors program component state changes and if something
 * needs to be done as a result of a state change then it is managed from
 * here to keep things organised and to cut down on code duplication */

void component_executive(void) {
	static int active_components = COMP_EMU;
	int found = FALSE;
	
	/* If the load selector's state has changed then do something */
	if ((active_components & COMP_LOAD) != (load_selector_state * COMP_LOAD)) {
		/* Hide the vkeyb if the file selector is active */
		if (load_selector_state && vkeyb.state) vkeyb.state = FALSE;
		found = TRUE;
	}

	/* If the vkeyb's state has changed then do something */
	if ((active_components & COMP_VKEYB) != (vkeyb.state * COMP_VKEYB)) {
		keyboard_buffer_reset();
		key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
		if (!vkeyb.state) video.redraw = TRUE;
		found = TRUE;
	}

	/* Update hotspot states if a state has changed */
	if (found) hotspots_update();

	/* Maintain a copy of current program component states  */
	active_components = COMP_EMU;
	if (load_selector_state) {
		active_components |= COMP_LOAD;
	} else {
		active_components &= ~COMP_LOAD;
	}
	if (vkeyb.state) {
		active_components |= COMP_VKEYB;
	} else {
		active_components &= ~COMP_VKEYB;
	}
}

/***************************************************************************
 * Timer Initialise                                                        *
 ***************************************************************************/
/* I've still yet to convert sound to use the SDL API. Currently if OSS sound
 * is enabled then it's used for timing, otherwise an SDL timer is used */

void sdl_timer_init(void) {

	if (!sound) {
		/* Create a 10ms timer if sound isn't enabled and managing timing */
		emulator.timer_id = SDL_AddTimer (10, emulator_timer, NULL);
		/*printf("Timing is being managed by an SDL timer\n");	temp temp */
	} else {
		/*printf("Timing is being managed using OSS sound\n");	temp temp */
	}
}

/***************************************************************************
 * Emulator Timer                                                          *
 ***************************************************************************/

Uint32 emulator_timer (Uint32 interval, void *param) {
	static int intervals = 0;

	intervals++;
	if (intervals >= 100 / nosound_hz) {
		signal_int_flag = TRUE;
		intervals = 0;
	}
	
	return interval;
}

/***************************************************************************
 * Clean Up Before Exit                                                    *
 ***************************************************************************/

void clean_up_before_exit(void) {
	int count;

	if (emulator.timer_id) SDL_RemoveTimer (emulator.timer_id);

	if (control_bar.scaled) SDL_FreeSurface(control_bar.scaled);
	if (vkeyb.original) SDL_FreeSurface(vkeyb.original);
	if (vkeyb.scaled) SDL_FreeSurface(vkeyb.scaled);
	if (sz81icons.original) SDL_FreeSurface(sz81icons.original);
	if (sz81icons.scaled) SDL_FreeSurface(sz81icons.scaled);
	if (zx80font.original) SDL_FreeSurface(zx80font.original);
	if (zx81font.original) SDL_FreeSurface(zx81font.original);
	if (zx82font.original) SDL_FreeSurface(zx82font.original);
	for (count = 0; count < MAX_FONTS; count++) {
		if (zx80font.scaled[count]) SDL_FreeSurface(zx80font.scaled[count]);
		if (zx81font.scaled[count]) SDL_FreeSurface(zx81font.scaled[count]);
		if (zx82font.scaled[count]) SDL_FreeSurface(zx82font.scaled[count]);
	}

	#ifdef PLATFORM_GP2X
		/* if (joystick) SDL_JoystickClose(joystick); This will segfault
		 * using the GPH SDK */
	#else
		if (joystick) SDL_JoystickClose(joystick);
	#endif

	if (highlight) SDL_FreeSurface(highlight);
	if (wm_icon) SDL_FreeSurface(wm_icon);

	SDL_Quit();
}





