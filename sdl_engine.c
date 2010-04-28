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
 * SDL Initialise                                                          *
 ***************************************************************************/
/* Since this is the first function the emulator calls prior to parseoptions
 * (which could exit) many things are done here. Firstly some variables are
 * initialised with defaults, SDL is initialised and then a window manager
 * icon is loaded and a title set if the platform requires it.
 * 
 * On exit: returns TRUE on error
 *          else FALSE */

int sdl_init(void) {
	int count;
	#if defined (PLATFORM_GP2X)
	#elif defined (PLATFORM_ZAURUS)
	#else
		char filename[256];
	#endif
	
	/* Set-up a default video resolution depending on platform */
	#if defined(PLATFORM_GP2X)
		video.xres = 320; video.yres = 240; video.scale = 1;
		video.fullscreen = SDL_FULLSCREEN;
	#elif defined(PLATFORM_ZAURUS)
		video.xres = 640; video.yres = 480; video.scale = 2;
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
	wm_icon = NULL;

	/* Initialise everything to a default here that could possibly be
	 * overridden by a command line option or from an rcfile */
	joystick_dead_zone = JOYSTICK_DEAD_ZONE;
	sdl_key_repeat.delay = KEY_REPEAT_DELAY;
	sdl_key_repeat.interval = KEY_REPEAT_INTERVAL;
	#if defined(PLATFORM_GP2X)
		sdl_sound.volume = 30;
	#else
		sdl_sound.volume = 128;
	#endif
	colours.colour_key = 0xff0080;
	colours.bmf_fg_default = 0xffffff;
	colours.emu_fg = 0x202018;
	colours.emu_bg = 0xf0f0e8;
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
	colours.hs_options_selected = 0x00ff00;
	colours.hs_options_pressed = 0xffc000;
	current_input_id = UNDEFINED;
	sdl_com_line.show_input_id = FALSE;
	sdl_com_line.fullscreen = UNDEFINED;
	sdl_com_line.scale = UNDEFINED;
	sdl_com_line.xres = UNDEFINED;
	sdl_com_line.yres = UNDEFINED;

	/* Initialise other things that need to be done before sdl_video_setmode */
	sdl_sound.state = FALSE;
	emulator.state = TRUE;
	emulator.speed = 50;	/* 50Hz is the default */
	vkeyb.state = vkeyb.autohide = vkeyb.toggle_shift = FALSE;
	vkeyb.alpha = SDL_ALPHA_OPAQUE;
	runtime_options0.state = FALSE;
	runtime_options1.state = FALSE;
	ctrl_remapper.state = FALSE;
	joy_cfg.state = FALSE;
	rcfile.rewrite = FALSE;

	/* Initialise SDL */
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		fprintf(stderr, "%s: Cannot initialise SDL: %s", __func__,
			SDL_GetError());
		return TRUE;
	}

	atexit(clean_up_before_exit);

	fprintf(stdout, "PACKAGE_DATA_DIR is %s\n", PACKAGE_DATA_DIR);

	/* Set WM icon and title if required for platform before
	 * setting a video mode as per SDL docs instructions */
	#if defined (PLATFORM_GP2X)
	#elif defined (PLATFORM_ZAURUS)
	#else
		strcpy(filename, PACKAGE_DATA_DIR "/");
		strcat(filename, IMG_WM_ICON);

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

	return FALSE;
}

/***************************************************************************
 * Process the Command Line Options                                        *
 ***************************************************************************/
/* Eventually I'd like to call this instead of z81's parseoptions and move
 * all the runtime modifiable options to runtime options since I want to
 * present what's left graphically at start-up. For the moment, this is good */

void sdl_com_line_process(int argc, char *argv[]) {
	#ifdef SDL_DEBUG_COM_LINE
		int count;
	#endif

	/* Validate sz81's command line options */
	if (sdl_com_line.xres != UNDEFINED && sdl_com_line.xres < 240) {
		fprintf(stderr, "sz81: horizontal resolution must be >= 240.\n");
		exit(1);
	} else if (sdl_com_line.xres != UNDEFINED && sdl_com_line.yres == UNDEFINED) {
		fprintf(stderr, "sz81: missing vertical resolution.\n");
		exit(1);
	} else if (sdl_com_line.yres != UNDEFINED && sdl_com_line.yres < 240) {
		fprintf(stderr, "sz81: vertical resolution must be >= 240.\n");
		exit(1);
	} else if (sdl_com_line.yres != UNDEFINED && sdl_com_line.xres == UNDEFINED) {
		fprintf(stderr, "sz81: missing horizontal resolution.\n");
		exit(1);
	}

	/* Process sz81's command line options */
	#if defined(PLATFORM_GP2X)
	#elif defined(PLATFORM_ZAURUS)
	#else
		if (sdl_com_line.fullscreen != UNDEFINED) video.fullscreen = SDL_FULLSCREEN;
		if (sdl_com_line.xres != UNDEFINED) {
			/* Calculate the scale for the requested resolution */
			if (sdl_com_line.xres / 240 > sdl_com_line.yres / 240) {
				sdl_com_line.scale = sdl_com_line.yres / 240;
			} else {
				sdl_com_line.scale = sdl_com_line.xres / 240;
			}
			video.scale = sdl_com_line.scale;
			video.xres = sdl_com_line.xres;
			video.yres = sdl_com_line.yres;
		}
	#endif

	#ifdef SDL_DEBUG_COM_LINE
		printf("%s:\n", __func__);
		for (count = 0; count < argc; count++) {
			printf("  %i: %s\n", count, argv[count]);
		}
		printf("  sdl_com_line.show_input_id=%i\n", sdl_com_line.show_input_id);
		printf("  sdl_com_line.fullscreen=%i\n", sdl_com_line.fullscreen);
		printf("  sdl_com_line.scale=%i\n", sdl_com_line.scale);
		printf("  sdl_com_line.xres=%i\n", sdl_com_line.xres);
		printf("  sdl_com_line.yres=%i\n", sdl_com_line.yres);
	#endif
}

/***************************************************************************
 * Component Executive                                                     *
 ***************************************************************************/
/* This function monitors program component state changes and if something
 * needs to be done as a result of a state change then it is managed from
 * here to keep things organised and to cut down on code duplication */

void component_executive(void) {
	static int active_components = COMP_EMU;
	static int ctrl_remapper_state = FALSE;
	int found = FALSE;
	
	/* Monitor emulator's state */
	if ((active_components & COMP_EMU) != (emulator.state * COMP_EMU)) {
		if (!emulator.state && load_selector_state) load_selector_state = FALSE;
		found = TRUE;
	}

	/* Monitor load selector's state */
	if ((active_components & COMP_LOAD) != (load_selector_state * COMP_LOAD)) {
		if (load_selector_state && vkeyb.state) vkeyb.state = FALSE;
		found = TRUE;
	}

	/* Monitor runtime options' state */
	if (((active_components & COMP_RUNOPTS0) != (runtime_options0.state * COMP_RUNOPTS0)) ||
		((active_components & COMP_RUNOPTS1) != (runtime_options1.state * COMP_RUNOPTS1))) {
		if ((runtime_options0.state || runtime_options1.state) && vkeyb.state) vkeyb.state = FALSE;
		/* Update the joycfg text */
		set_joy_cfg_text(0);
		video.redraw = TRUE;
		found = TRUE;
	}

	/* Monitor virtual keyboard's state */
	if ((active_components & COMP_VKEYB) != (vkeyb.state * COMP_VKEYB)) {
		if (!vkeyb.state) video.redraw = TRUE;
		found = TRUE;
	}

	if (found) {
		keyboard_buffer_reset();
		key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
		/* Update hotspot states */
		hotspots_update();
	}

	/* Monitor control remapper's state */ 
	if (ctrl_remapper_state != ctrl_remapper.state) {
		if (ctrl_remapper.state) ctrl_remapper.interval = 0;
	}
	/* Manage control remapper's interval decrementing and resetting from here */
	if (ctrl_remapper.state && --ctrl_remapper.interval <= 0)
		ctrl_remapper.interval = ctrl_remapper.master_interval;

	/* Maintain a copy of current program component states  */
	ctrl_remapper_state = ctrl_remapper.state;
	active_components = 0;
	if (emulator.state) {
		active_components |= COMP_EMU;
	} else {
		active_components &= ~COMP_EMU;
	}
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
	if (runtime_options0.state) {
		active_components |= COMP_RUNOPTS0;
	} else {
		active_components &= ~COMP_RUNOPTS0;
	}
	if (runtime_options1.state) {
		active_components |= COMP_RUNOPTS1;
	} else {
		active_components &= ~COMP_RUNOPTS1;
	}
}

/***************************************************************************
 * Get Active Component                                                    *
 ***************************************************************************/
/* This returns the main currently active component */

int get_active_component(void) {
	int retval;
	
	if (runtime_options0.state) {
		retval = COMP_RUNOPTS0;
	} else if (runtime_options1.state) {
		retval = COMP_RUNOPTS1;
	} else if (vkeyb.state) {
		retval = COMP_VKEYB;
	} else if (load_selector_state) {
		retval = COMP_LOAD;
	} else {
		retval = COMP_EMU;
	}
	
	return retval;
}

/***************************************************************************
 * Timer Initialise                                                        *
 ***************************************************************************/

void sdl_timer_init(void) {
	/* Create a 10ms timer */
	emulator.timer_id = SDL_AddTimer (10, emulator_timer, NULL);
}

/***************************************************************************
 * Emulator Timer                                                          *
 ***************************************************************************/

Uint32 emulator_timer (Uint32 interval, void *param) {
	static int intervals = 0;

	intervals++;
	if (intervals >= 100 / emulator.speed) {
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

	sdl_sound_end();

	if (emulator.timer_id) SDL_RemoveTimer (emulator.timer_id);

	if (rcfile.rewrite) rcfile_write();

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
		#ifdef TOOLCHAIN_OPEN2X
			if (joystick) SDL_JoystickClose(joystick);
		#endif
	#else
		if (joystick) SDL_JoystickClose(joystick);
	#endif

	if (wm_icon) SDL_FreeSurface(wm_icon);

	SDL_Quit();
}





