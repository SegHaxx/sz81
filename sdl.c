/* sz81 Copyright (C) 2007-2010 Thunor <thunorsif@hotmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <SDL/SDL.h>
#include <stdlib.h>
#include <string.h>
#include "sdl.h"

/* My defines */
/* Uncomment all of these to view the inner workings. Then monitor
 * the console output and press alt+r to cycle the resolutions.
#define SDL_DEBUG_VIDEO
#define SDL_DEBUG_TIMING
#define SDL_DEBUG_EVENTS
#define SDL_DEBUG
#define SDL_DEBUG_HOTSPOTS
#define SDL_DEBUG_FONTS */

#define TRUE 1
#define FALSE 0
#define UNDEFINED -1
#define MAX_CTRL_REMAPS 32
#define MAX_HOTSPOTS 64
#define MAX_FONTS 8

#define IMG_WM_ICON "sz81.bmp"
#define IMG_ZX80_KYBD "zx80kybd.bmp"
#define IMG_ZX81_KYBD "zx81kybd.bmp"
#define IMG_ZX80_FONT "zx80font.bmp"
#define IMG_ZX81_FONT "zx81font.bmp"
#define IMG_ZX82_FONT "zx82font.bmp"
#define IMG_SZ81_ICONS "sz81icons.bmp"

/* Icon bitmap offsets that must be multiplied by video.scale */
#define ICON_EXIT_X 0
#define ICON_EXIT_Y 0
#define ICON_RESET_X 16
#define ICON_RESET_Y 0
#define ICON_DONTHIDE_X 32
#define ICON_DONTHIDE_Y 0
#define ICON_AUTOHIDE_X 48
#define ICON_AUTOHIDE_Y 0
#define ICON_STICKYSHIFT_X 64
#define ICON_STICKYSHIFT_Y 0
#define ICON_TOGGLESHIFT_X 80
#define ICON_TOGGLESHIFT_Y 0
#define ICON_NOTINVERSE_X 96
#define ICON_NOTINVERSE_Y 0
#define ICON_INVERSE_X 112
#define ICON_INVERSE_Y 0
#define ICON_ALPHA_DN_X 128
#define ICON_ALPHA_DN_Y 0
#define ICON_ALPHA_UP_X 144
#define ICON_ALPHA_UP_Y 0

#define DEVICE_KEYBOARD 1
#define DEVICE_JOYSTICK 2

/* Hotspot group IDs */
#define HS_GRP_ROOT 0
#define HS_GRP_LOAD 1
#define HS_GRP_VKEYB 2
#define HS_GRP_CTB 3

/* Hotspot IDs (hotspots[] indexes which are also zorders) */
#define HS_ROOT_CANVAS 0
#define HS_LOAD_Q 1
#define HS_LOAD_A 2
#define HS_LOAD_ENTER 3
#define HS_LOAD_SPACE 4
#define HS_VKEYB_VKEYB 5
#define HS_VKEYB_1 6
#define HS_VKEYB_Q 16
#define HS_VKEYB_A 26
#define HS_VKEYB_NEWLINE 35
#define HS_VKEYB_SHIFT 36
#define HS_CTB_CTB 46
#define HS_CTB_EXIT 47
#define HS_CTB_RESET 48
#define HS_CTB_AUTOHIDE 49
#define HS_CTB_SHIFT_TYPE 50
#define HS_CTB_INVERSE 51
#define HS_CTB_ALPHA_DN 52
#define HS_CTB_ALPHA_UP 53

/* Hotspot properties */
#define HS_PROP_ONOFF 1
#define HS_PROP_STICKY 2
#define HS_PROP_TOGGLE 4
#define HS_PROP_STUCK 8
#define HS_PROP_VISIBLE 16
#define HS_PROP_SELECTED 32
#define HS_PROP_NAVIGABLE 64

/* Bitmap font IDs */
#define BMF_FONT_ZX80 0
#define BMF_FONT_ZX81 1
#define BMF_FONT_ZX82 2

/* GP2X button IDs */
#define GP2X_JOY_N 0x00
#define GP2X_JOY_NW 0x01
#define GP2X_JOY_W 0x02
#define GP2X_JOY_SW 0x03
#define GP2X_JOY_S 0x04
#define GP2X_JOY_SE 0x05
#define GP2X_JOY_E 0x06
#define GP2X_JOY_NE 0x07
#define GP2X_START 0x08
#define GP2X_SELECT 0x09
#define GP2X_LTRIG 0x0A
#define GP2X_RTRIG 0x0B
#define GP2X_BTN_A 0x0C
#define GP2X_BTN_B 0x0D
#define GP2X_BTN_X 0x0E
#define GP2X_BTN_Y 0x0F
#define GP2X_VOL_UP 0x10
#define GP2X_VOL_DN 0x11
#define GP2X_BTN_JOY 0x12


/* Emulator variables I need access to */
#ifdef OSS_SOUND_SUPPORT
extern int sound_hz;
#endif
extern int nosound_hz, scrn_freq, invert_screen, zx80;
extern int load_selector_state, refresh_screen, ignore_esc;
extern int sound;
extern volatile int signal_int_flag;

/* My variables */
SDL_Surface *wm_icon = NULL;
SDL_Joystick *joystick = NULL;

struct {
	SDL_Surface *screen;
	int fullscreen;		/* 0 or SDL_FULLSCREEN */
	int scale;			/* 1, 2 or 3 */
	int xres;
	int yres;
	int redraw;			/* TRUE to redraw entire screen */
} video;

struct {
	int xoffset;
	int yoffset;
	SDL_TimerID timer_id;
} emulator;

struct bmpfont {
	SDL_Surface *original;
	SDL_Surface *scaled[MAX_FONTS];
	Uint32 fg_colour[MAX_FONTS];
};
struct bmpfont zx80font, zx81font, zx82font;	

struct {
	SDL_Surface *original;
	SDL_Surface *scaled;
} sz81icons;

struct {
	SDL_Surface *original;
	SDL_Surface *scaled;
	int state;
	int xoffset;
	int yoffset;
	int alpha;
	int autohide;		/* TRUE to hide on newline */
	int toggle_shift;	/* TRUE for toggle, FALSE for sticky */
} vkeyb;

struct {
	SDL_Surface *scaled;
	int xoffset;
	int yoffset;
} control_bar;

struct {
	Uint32 colour_key;
	Uint32 bmf_fg_default;
	Uint32 emu_fg;
	Uint32 emu_bg;
	Uint32 hs_load_pressed;
	Uint32 hs_vkeyb_zx80_pressed;
	Uint32 hs_vkeyb_zx80_toggle_pressed;
	Uint32 hs_vkeyb_zx81_pressed;
	Uint32 hs_vkeyb_zx81_toggle_pressed;
	Uint32 hs_ctb_pressed;
} colours;

struct ctrlremap {
	int device;			/* The source device e.g. DEVICE_JOYSTICK */
	int id;				/* The source control id */
	int remap_id;		/* The main destination control id */
	int remap_mod_id;	/* An additional modifier destination control id */
};
struct ctrlremap ctrl_remaps[MAX_CTRL_REMAPS];

struct hotspot {
	int gid;			/* Group id for easy management */
	int flags;			/* An OR'd combination of HS_PROP_ properties */
	int hit_x, hit_y, hit_w, hit_h;	/* Hit box */
	int hl_x, hl_y, hl_w, hl_h;		/* Highlight box (if all UNDEFINED then use hitbox */
	int remap_id;		/* The main destination control id (could be UNDEFINED) */
};
struct hotspot hotspots[MAX_HOTSPOTS];

/* Emulator functions I need access to */
extern void exit_program(void);
extern void reset81(void);

/* My functions */
void hotspots_init(void);
void hotspots_resize(void);
void hotspots_update(void);
void hotspots_render(void);
int fonts_init(void);
int vkeyb_init(void);
int sz81icons_init(void);
int control_bar_init(void);
SDL_Surface *BMF_RenderText(int font, char *text, Uint32 fg_colour, Uint32 bg_colour);
Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void scale_surface(SDL_Surface *original, SDL_Surface *scaled);
int keysym_to_scancode(int reverse, int value);
void cycle_resolutions(void);
Uint32 emulator_timer (Uint32 interval, void *param);
void clean_up_before_exit(void);


/* DEVELOPMENT NOTES: main.c is for z81 and it uses SVGAlib. To get this
 * working under SDL I simply copied main.c to smain.c and substituted the
 * vga and vgakeyboard functions with my equivalents below. This means that
 * I don't need to touch the original code except where I need to patch in.
 * The result was an incredibly easy port, and now that I am in control of
 * the screen and keyboard I can grab/remap keys, add a GUI and a virtual
 * keyboard without interferring with the original source :) */


/* ------------------------------------------------------------------------*
 * ----------------- SVGAlib's vga function equivalents -------------------*
 * ------------------------------------------------------------------------*/

/***************************************************************************
 * VGA Initialise                                                          *
 ***************************************************************************/
/* Since this is the first function the emulator calls prior to parseoptions
 * (which could exit) many things are done here. Firstly some variables are
 * initialised with defaults, SDL is initialised and then a window manager
 * icon is loaded and a title set if the platform requires it .
 * 
 * On exit: returns 0 on success or -1 on failure */

int vga_init(void) {
	int count;
	#if defined (PLATFORM_GP2X)
	#elif defined (PLATFORM_ZAURUS)
	#else
		char filename[256];
	#endif
	
	/* Initialise some variables with defaults here */
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
	emulator.timer_id = NULL;
	zx80font.original = zx81font.original = zx82font.original = NULL;
	for (count = 0; count < MAX_FONTS; count++)
		zx80font.scaled[count] = zx81font.scaled[count] = 
			zx82font.scaled[count] = NULL;
	sz81icons.original = sz81icons.scaled = NULL;
	vkeyb.original = vkeyb.scaled = NULL;
	vkeyb.state = FALSE;
	vkeyb.alpha = SDL_ALPHA_OPAQUE;
	vkeyb.autohide = FALSE;
	vkeyb.toggle_shift = FALSE;
	control_bar.scaled = NULL;
	colours.colour_key = 0xff0080;
	colours.bmf_fg_default = 0xffffff;
	colours.emu_fg = 0x0;
	colours.emu_bg = 0xffffff;
	colours.hs_load_pressed = 0xffc000;
	colours.hs_vkeyb_zx80_pressed = 0xffff00;
	colours.hs_vkeyb_zx80_toggle_pressed = 0xff4000;
	colours.hs_vkeyb_zx81_pressed = 0xffc000;
	colours.hs_vkeyb_zx81_toggle_pressed = 0xff4000;
	colours.hs_ctb_pressed = 0xffc000;

	/* Initialise SDL */
	if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		fprintf(stderr, "%s: Cannot initialise SDL: %s", __func__,
			SDL_GetError());
		return -1;
	}

	atexit(clean_up_before_exit);

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
 * VGA Set Mode                                                            *
 ***************************************************************************/
/* On entry: mode = a video mode: G320x200x256 is the only mode supported
 *                  and this is overridden anyway :)
 *           mode = -1 to return the svgalib version in BCD
 *  On exit: if mode was -1 then returns svgalib version in BCD
 *           else returns 0 on success or -1 on failure */

int vga_setmode(int mode) {
	int original_xres = video.xres;
	
	switch (mode) {
		case -1:
			return 0x1410;	/* Return svgalib version 1.4.1 in BCD */
			break;
		case TEXTMODE:
			return 0;
			break;
		case G320x200x256:
			/* Try the requested video resolution and if it's unavailable
			 * try the next one down and continue until one is accepted.
			 * Note: I first used SDL_VideoModeOK() but it didn't work on
			 * my Zaurus so it's probably best to use brute force */
			do {
				video.screen = SDL_SetVideoMode(video.xres, video.yres, 16,
					SDL_SWSURFACE | SDL_ANYFORMAT | video.fullscreen);
				if (video.screen == NULL) {
					fprintf(stderr, "%s: Cannot set video mode: %i x %i: %s\n",
						__func__, video.xres, video.yres, SDL_GetError());
					cycle_resolutions();
				}
			} while (video.screen == NULL && video.xres != original_xres);
			if (video.screen == NULL) exit(1);	/* Can't continue */
			break;
		default:
			return -1;
			break;
	}

	#ifdef SDL_DEBUG_VIDEO
		printf("%s: video.screen->format->BitsPerPixel=%i\n", __func__,
			video.screen->format->BitsPerPixel);
	#endif

	/* Hide the mouse pointer for certain devices */
	#if defined (PLATFORM_GP2X) || defined (PLATFORM_ZAURUS)
		SDL_ShowCursor(SDL_DISABLE);
	#endif
	
	/* Set-up the emulator's screen offset */
	if (video.xres <= 256 * video.scale) {
		emulator.xoffset = 0 - 32 * video.scale;
	} else {
		emulator.xoffset = (video.xres - 320 *  video.scale) / 2;
	}
	emulator.yoffset = (video.yres - 200 *  video.scale) / 2;

	#ifdef SDL_DEBUG_VIDEO
		printf("%s: emulator.xoffset=%i emulator.yoffset=%i\n", __func__,
			emulator.xoffset, emulator.yoffset);
	#endif

	/* Set-up the fonts */
	if (fonts_init()) exit(1);

	/* Set-up the sz81 icons */
	if (sz81icons_init()) exit(1);

	/* Set-up the virtual keyboard */
	if (vkeyb_init()) exit(1);

	/* Set-up the control bar */
	if (control_bar_init()) exit(1);

	/* Resize all hotspots for the current screen dimensions.
	 * The hotspots will not have yet been initialised the very first
	 * time here, but they will have been on subsequent visits and so a
	 * check needs to be performed which will skip it on the first visit */
	if (hotspots[HS_VKEYB_VKEYB].gid == HS_GRP_VKEYB) hotspots_resize();

	/* Prepare to redraw everything */
	video.redraw = TRUE;

	return 0;
}

/***************************************************************************
 * VGA Get Graph Mem                                                       *
 ***************************************************************************/
/* On exit: returns a pointer to the graphics memory */

unsigned char *vga_getgraphmem(void) {
	return vga_graphmemory;
}

/* ------------------------------------------------------------------------*
 * ------------- SVGAlib's vgakeyboard function equivalents ---------------*
 * ------------------------------------------------------------------------*/

/***************************************************************************
 * Keyboard Initialise                                                     *
 ***************************************************************************/
/* This initialises the keyboard buffer, sets up the control remappings,
 * initialises the hotspots and opens joystick 0 if available. At the moment
 * the control remapping is hardcoded but in the future it would be nice
 * (and with a joystick a necessity) to offer this as a runtime feature.
 * 
 * On exit: returns 0 on success or -1 on failure */

int keyboard_init(void) {
	int count, index;
	SDL_Event event;
	
	/* Erase the keyboard buffer */
	for (count = 0; count < MAX_SCANCODES; count++)
		keyboard_buffer[count] = KEY_NOTPRESSED;
	
	/* Undefine all the control remappings */
	for (count = 0; count < MAX_CTRL_REMAPS; count++) {
		ctrl_remaps[count].device = UNDEFINED;
		ctrl_remaps[count].remap_mod_id = UNDEFINED;
	}

	/* Set-up some input device to keyboard remappings */
	index = -1;
	ctrl_remaps[++index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_UP;
	ctrl_remaps[index].remap_id = SDLK_7;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_DOWN;
	ctrl_remaps[index].remap_id = SDLK_6;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_LEFT;
	ctrl_remaps[index].remap_id = SDLK_5;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_RIGHT;
	ctrl_remaps[index].remap_id = SDLK_8;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_BACKSPACE;
	ctrl_remaps[index].remap_id = SDLK_0;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_COMMA;
	ctrl_remaps[index].remap_id = SDLK_PERIOD;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_RSHIFT;
	ctrl_remaps[index].remap_id = SDLK_LSHIFT;

	#if defined(PLATFORM_GP2X)
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_JOY_N;
		ctrl_remaps[index].remap_id = SDLK_k;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_JOY_E;
		ctrl_remaps[index].remap_id = SDLK_l;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_JOY_S;
		ctrl_remaps[index].remap_id = SDLK_j;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_JOY_W;
		ctrl_remaps[index].remap_id = SDLK_h;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_START;
		ctrl_remaps[index].remap_id = SDLK_F10;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_SELECT;
		ctrl_remaps[index].remap_id = SDLK_F1;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_LTRIG;
		ctrl_remaps[index].remap_id = SDLK_q;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_RTRIG;
		ctrl_remaps[index].remap_id = SDLK_a;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_BTN_A;
		ctrl_remaps[index].remap_id = SDLK_1;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_BTN_X;
		ctrl_remaps[index].remap_id = SDLK_3;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_BTN_Y;
		ctrl_remaps[index].remap_id = SDLK_SPACE;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_BTN_B;
		ctrl_remaps[index].remap_id = SDLK_RETURN;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_VOL_DN;
		ctrl_remaps[index].remap_id = SDLK_2;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_VOL_UP;
		ctrl_remaps[index].remap_id = SDLK_4;
		ctrl_remaps[++index].device = DEVICE_JOYSTICK;
		ctrl_remaps[index].id = GP2X_BTN_JOY;
		ctrl_remaps[index].remap_id = SDLK_5;

	#elif defined(PLATFORM_ZAURUS)
	#endif
	
	/* Initialise the hotspots now */
	hotspots_init();

	/* Open joystick 0 if available */
	SDL_JoystickEventState(SDL_ENABLE);			
	joystick = SDL_JoystickOpen(0);
	if (joystick) {
		fprintf(stdout, "Joystick opened: 0:%s\n", SDL_JoystickName (0));
		/* Flush events as opening a joystick releases all the buttons */
		if (joystick) while (SDL_PollEvent (&event));
	}

	/* This function can be used to override certain emulator variables
	 * as following this all that's remaining is sound and timer
	 * intialisation before entering the emulator's main loop */

	/* Setting sound_hz = nosound_hz = 50*4 and scrn_freq = 2*4
	 * significantly speeds-up the emulator and can be useful when
	 * developing software using the emulator itself as it makes
	 * keyboard input much more responsive. This would be useful as a
	 * runtime reconfigurable option i.e. overclocking since BASIC
	 * programs are so much better :) */
	nosound_hz = 50;		/* z81 default (50Hz) */
	#ifdef OSS_SOUND_SUPPORT
		sound_hz = nosound_hz;
	#endif
	scrn_freq = 2;			/* z81 default (50Hz/2=25Hz) */
	
	if (!sound) {
		/* Create a 10ms timer if sound isn't enabled and managing timing */
		emulator.timer_id = SDL_AddTimer (10, emulator_timer, NULL);
		/*printf("Timing is being managed by an SDL timer\n");	temp temp */
	} else {
		/*printf("Timing is being managed using OSS sound\n");	temp temp */
	}
	
	return 0;
}

/***************************************************************************
 * Keyboard Close                                                          *
 ***************************************************************************/

void keyboard_close(void) {
}

/***************************************************************************
 * Keyboard Get State                                                      *
 ***************************************************************************/
/* On exit: returns a char pointer to a buffer holding the state of all keys */

char *keyboard_getstate(void) {
	return keyboard_buffer;
}

/***************************************************************************
 * Keyboard Update                                                         *
 ***************************************************************************/
/* I was experiencing an issue that was highlighted by pointer hotspot input
 * whereby the emulator or emulated machine wasn't processing the events
 * quickly enough and key presses were getting released before the pressed
 * state was recorded. I fixed this by adding a flag to skip every other
 * update which solves this problem beautifully.
 *
 * On exit: returns non-zero if there are keyboard events else 0 */

int keyboard_update(void) {
	static int skip_update = TRUE, last_hs_pressed = UNDEFINED;
	static int cursor_x = 0, cursor_y = 0;
	int eventfound = FALSE, count, found;
	int device, id, mod_id, state;
	SDLMod modstate;
	SDL_Event event, myevent;
	#ifdef SDL_DEBUG_TIMING
		static Uint32 lasttime = 0;
		static int Hz = 0;

		Hz++;
		if (SDL_GetTicks() - lasttime >= 1000) {
			printf("%s=%iHz\n", __func__, Hz);
			lasttime = SDL_GetTicks();
			Hz = 0;
		}
	#endif

	/* Process events every other call otherwise the emulator or the
	 * emulated machine has trouble keeping up with event creation */
	if (!(skip_update = !skip_update)) {

		while (SDL_PollEvent(&event)) {
			/* Get something we're interested in */
			device = id = mod_id = state = UNDEFINED;
			switch (event.type) {
				case SDL_KEYUP:
				case SDL_KEYDOWN:
					device = DEVICE_KEYBOARD;
					id = event.key.keysym.sym;
					state = event.key.state;
					break;
				case SDL_MOUSEMOTION:
					cursor_x = event.motion.x;
					cursor_y = event.motion.y;
					break;
				case SDL_JOYBUTTONUP:
				case SDL_JOYBUTTONDOWN:
					device = DEVICE_JOYSTICK;
					id = event.jbutton.button;
					state = event.jbutton.state;
					break;
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
					device = DEVICE_KEYBOARD;
					state = event.button.state;
					/* Convert a hotspot hit into an id */
					found = FALSE;
					if (state == SDL_PRESSED) {
						for (count = MAX_HOTSPOTS - 1; count >= 0; count--) {
							if (hotspots[count].gid != UNDEFINED &&
								hotspots[count].flags & HS_PROP_VISIBLE &&
								cursor_x >= hotspots[count].hit_x &&
								cursor_x < hotspots[count].hit_x + hotspots[count].hit_w &&
								cursor_y >= hotspots[count].hit_y &&
								cursor_y < hotspots[count].hit_y + hotspots[count].hit_h) {
								if (hotspots[count].remap_id != UNDEFINED) {
									/* Is the hotspot sticky or a toggle? */
									if (hotspots[count].flags & HS_PROP_STICKY ||
										hotspots[count].flags & HS_PROP_TOGGLE) {
										/* If it's currently stuck then release it */
										if (hotspots[count].flags & HS_PROP_STUCK) {
											hotspots[count].flags &= ~HS_PROP_STUCK;
											state = SDL_RELEASED;
										} else {
											hotspots[count].flags |= HS_PROP_STUCK;
										}
									}
									id = hotspots[count].remap_id;
									last_hs_pressed = count;
									found = TRUE;
								}
								break;
							}
						}
					} else {
						if (hotspots[last_hs_pressed].flags & HS_PROP_ONOFF) {
							/* If a virtual keyboard hotspot was last pressed
							 * then release stuck sticky hotspots */
							if (hotspots[last_hs_pressed].gid == HS_GRP_VKEYB) {
								for (count = MAX_HOTSPOTS - 1; count >= 0; count--) {
									if (hotspots[count].gid != UNDEFINED &&
										hotspots[count].flags & HS_PROP_STICKY &&
										hotspots[count].flags & HS_PROP_STUCK) {
										hotspots[count].flags &= ~HS_PROP_STUCK;
										myevent.type = SDL_KEYUP;
										myevent.key.keysym.sym = hotspots[count].remap_id;
										myevent.key.state = SDL_RELEASED;
										SDL_PushEvent(&myevent);
									}
								}
							}
							id = hotspots[last_hs_pressed].remap_id;
							last_hs_pressed = UNDEFINED;
							found = TRUE;
						}
					}
					if (!found) device = UNDEFINED;	/* Ignore id */
					break;
				case SDL_QUIT:
					exit_program();
					break;
			}

			/* Perform control remapping here. This could expand one event
			 * into two - id and mod_id - so this should be taken into account */
			if (device != UNDEFINED) {
				for (count = 0; count < MAX_CTRL_REMAPS; count++) {
					if (ctrl_remaps[count].device != UNDEFINED &&
						device == ctrl_remaps[count].device && id == ctrl_remaps[count].id) {
						id = ctrl_remaps[count].remap_id;
						mod_id = ctrl_remaps[count].remap_mod_id;
						break;
					}
				}
			}

			#ifdef SDL_DEBUG_EVENTS
				if (device != UNDEFINED) {
					printf("%s: device=%i: id=%i state=%i mod_id=%i\n",
						__func__, device, id, state, mod_id);
				}
			#endif

			/* Grab input for my own use but don't report it */
			if (device != UNDEFINED) {
				found = FALSE;
				modstate = SDL_GetModState();
				if (id == SDLK_r && (modstate & (KMOD_ALT | KMOD_MODE))) {
					/* Cycle screen resolutions on supported platforms */
					if (state == SDL_PRESSED) {
						cycle_resolutions();
						vga_setmode(G320x200x256);
					}
					found = TRUE;
				}
				if (found) device = UNDEFINED;	/* Ignore id and mod_id */
			}

			/* Intercept input for my own use but continue to report it */
			if (device != UNDEFINED) {
				if (id == SDLK_F1) {
					/* Toggle the virtual keyboard */
					if (state == SDL_PRESSED) {
						vkeyb.state = !vkeyb.state;
						if (!vkeyb.state) video.redraw = TRUE;
					}
				} else if (id == SDLK_F2) {
					if (state == SDL_PRESSED) {
						/* Reserved for future Save State */
					}
				} else if (id == SDLK_F3) {
					if (state == SDL_PRESSED) {
						/* Reserved for future Load State */
					}
				} else if (id == SDLK_F5) {
					if (state == SDL_PRESSED) {
						/* Reserved for future Options */
					}
				} else if (id == SDLK_F6) {
					if (vkeyb.state) {
						/* Toggle the vkeyb autohide state */
						if (state == SDL_PRESSED) {
							vkeyb.autohide = !vkeyb.autohide;
							control_bar_init();
							video.redraw = TRUE;
						}
					}
				} else if (id == SDLK_F7) {
					if (vkeyb.state) {
						/* Toggle the vkeyb shift type */
						if (state == SDL_PRESSED) {
							if (vkeyb.toggle_shift) {
								hotspots[HS_VKEYB_SHIFT].flags &= ~HS_PROP_TOGGLE;
								hotspots[HS_VKEYB_SHIFT].flags |= HS_PROP_STICKY;
							} else {
								hotspots[HS_VKEYB_SHIFT].flags &= ~HS_PROP_STICKY;
								hotspots[HS_VKEYB_SHIFT].flags |= HS_PROP_TOGGLE;
							}
							vkeyb.toggle_shift = !vkeyb.toggle_shift;
							control_bar_init();
							video.redraw = TRUE;
						}
					}
				} else if (id == SDLK_F8) {
					/* Toggle invert screen */
					if (state == SDL_PRESSED) {
						invert_screen = !invert_screen;
						refresh_screen = 1;
						control_bar_init();
						video.redraw = TRUE;
					}
				} else if (id == SDLK_HOME || id == SDLK_END) {
					if (vkeyb.state) {
						/* Adjust the vkeyb alpha */
						if (state == SDL_PRESSED) {
							if (id == SDLK_HOME && vkeyb.alpha == SDL_ALPHA_OPAQUE) {
								vkeyb.alpha -= 31;
							} else if (id == SDLK_HOME && vkeyb.alpha >= 64) {
								vkeyb.alpha -= 32;
							} else if (id == SDLK_END && vkeyb.alpha == 224) {
								vkeyb.alpha += 31;
							} else if (id == SDLK_END && vkeyb.alpha < 224) {
								vkeyb.alpha += 32;
							}
							if ((SDL_SetAlpha(vkeyb.scaled, SDL_SRCALPHA, vkeyb.alpha)) < 0) {
								fprintf(stderr, "%s: Cannot set surface alpha: %s\n", __func__,
									SDL_GetError());
								exit(1);
							}
							control_bar_init();
							video.redraw = TRUE;
						}
					}
				} else if (id == SDLK_F10) {
					/* Exit the emulator */
					if (state == SDL_RELEASED) {
						exit_program();
					}
				} else if (id == SDLK_F11) {
					#if !defined (PLATFORM_GP2X) && !defined (PLATFORM_ZAURUS)
						/* Toggle fullscreen on supported platforms */
						if (state == SDL_PRESSED) {
							video.fullscreen ^= SDL_FULLSCREEN;
							vga_setmode(G320x200x256);
						}
					#endif
				} else if (id == SDLK_F12) {
					/* Reset the emulator */
					if (state == SDL_PRESSED) {
						if (!ignore_esc) reset81();
					}
				}
			}
			
			/* Make sure that stuck sticky and toggle hotspots are not released
			 * by other means which would leave their stuck states intact but
			 * erase their pressed states within the keyboard buffer */
			if (device != UNDEFINED && state == SDL_RELEASED) {
				for (count = MAX_HOTSPOTS - 1; count >= 0; count--) {
					/* Since the necessary property checks would have been applied when
					 * the hotspot was stuck we can just check for stuck and the remap_id */
					if (hotspots[count].gid != UNDEFINED && hotspots[count].flags & HS_PROP_STUCK) {
						if (hotspots[count].remap_id == id) device = UNDEFINED;	/* Ignore id and mod_id */
						if (hotspots[count].remap_id == mod_id) mod_id = UNDEFINED;	/* Ignore mod_id */
					}
				}
			}
		
			/* Record what's left in the keyboard buffer */
			if (device != UNDEFINED) {
				eventfound = TRUE;
				if (state == SDL_PRESSED) {
					keyboard_buffer[keysym_to_scancode(FALSE, id)] = KEY_PRESSED;
					if (mod_id != UNDEFINED) keyboard_buffer
						[keysym_to_scancode(FALSE, mod_id)] = KEY_PRESSED;
				} else {
					keyboard_buffer[keysym_to_scancode(FALSE, id)] = KEY_NOTPRESSED;
					if (mod_id != UNDEFINED) keyboard_buffer
						[keysym_to_scancode(FALSE, mod_id)] = KEY_NOTPRESSED;
				}
			}

			/* Should the vkeyb be hidden on ENTER? */
			if (vkeyb.state && vkeyb.autohide && device != UNDEFINED &&
				id == SDLK_RETURN && state == SDL_RELEASED &&
				keyboard_buffer[keysym_to_scancode(FALSE, SDLK_LSHIFT)] == KEY_NOTPRESSED) {
				vkeyb.state = FALSE; video.redraw = TRUE;
			}

			/* If we've recorded something then quit now */
			if (eventfound) break;
		}


		/*for (count = 0; count < MAX_SCANCODES; count++) {
			if (keyboard_buffer[count] == KEY_PRESSED) {
				printf("%i is pressed\n", count);
			}
		} temp temp */

	}

	return eventfound;
}

/***************************************************************************
 * Keyboard Key Pressed                                                    *
 ***************************************************************************/
/* On exit: returns KEY_PRESSED if the key with scancode is pressed
 *          else KEY_NOTPRESSED */

int keyboard_keypressed(int scancode) {
	return keyboard_buffer[scancode];
}

/***************************************************************************
 * Keyboard Translate Keys                                                 *
 ***************************************************************************/

void keyboard_translatekeys(int mask) {
}

/* ------------------------------------------------------------------------*
 * --------------------------- My functions -------------------------------*
 * ------------------------------------------------------------------------*/

/***************************************************************************
 * Hotspots Initialise                                                     *
 ***************************************************************************/
/* This creates the hotspots and initialises their static properties */

void hotspots_init(void) {
	int count;

	/* Initialise all hotspots with some default values */
	for (count = 0; count < MAX_HOTSPOTS; count++) {
		hotspots[count].gid = UNDEFINED;
		hotspots[count].hit_x = hotspots[count].hit_y = UNDEFINED;
		hotspots[count].hit_w = hotspots[count].hit_h = UNDEFINED;
		hotspots[count].hl_x = hotspots[count].hl_y = UNDEFINED;
		hotspots[count].hl_w = hotspots[count].hl_h = UNDEFINED;
		hotspots[count].flags = 0;
		hotspots[count].flags |= HS_PROP_VISIBLE;
		hotspots[count].flags |= HS_PROP_NAVIGABLE;
		hotspots[count].flags |= HS_PROP_ONOFF;
		hotspots[count].remap_id = UNDEFINED;
	}
	
	/* Initialise root hotspots */
	hotspots[HS_ROOT_CANVAS].gid = HS_GRP_ROOT;
	hotspots[HS_ROOT_CANVAS].flags &= ~HS_PROP_NAVIGABLE;
	hotspots[HS_ROOT_CANVAS].remap_id = SDLK_F1;

	/* Initialise file selector hotspots */
	for (count = 0; count < 4; count++) {
		hotspots[HS_LOAD_Q + count].gid = HS_GRP_LOAD;
	}
	hotspots[HS_LOAD_Q].remap_id = SDLK_q;
	hotspots[HS_LOAD_Q].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_LOAD_A].remap_id = SDLK_a;
	hotspots[HS_LOAD_ENTER].remap_id = SDLK_RETURN;
	hotspots[HS_LOAD_SPACE].remap_id = SDLK_SPACE;

	/* Initialise virtual keyboard hotspots */
	hotspots[HS_VKEYB_VKEYB].gid = HS_GRP_VKEYB;
	hotspots[HS_VKEYB_VKEYB].flags &= ~HS_PROP_NAVIGABLE;
	for (count = 0; count < 10; count++) {
		hotspots[HS_VKEYB_1 + count].gid = HS_GRP_VKEYB;
		hotspots[HS_VKEYB_Q + count].gid = HS_GRP_VKEYB;
		hotspots[HS_VKEYB_A + count].gid = HS_GRP_VKEYB;
		hotspots[HS_VKEYB_SHIFT + count].gid = HS_GRP_VKEYB;
	}
	hotspots[HS_VKEYB_1].remap_id = SDLK_1;
	hotspots[HS_VKEYB_1].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_VKEYB_1 + 1].remap_id = SDLK_2;
	hotspots[HS_VKEYB_1 + 2].remap_id = SDLK_3;
	hotspots[HS_VKEYB_1 + 3].remap_id = SDLK_4;
	hotspots[HS_VKEYB_1 + 4].remap_id = SDLK_5;
	hotspots[HS_VKEYB_1 + 5].remap_id = SDLK_6;
	hotspots[HS_VKEYB_1 + 6].remap_id = SDLK_7;
	hotspots[HS_VKEYB_1 + 7].remap_id = SDLK_8;
	hotspots[HS_VKEYB_1 + 8].remap_id = SDLK_9;
	hotspots[HS_VKEYB_1 + 9].remap_id = SDLK_0;
	hotspots[HS_VKEYB_Q].remap_id = SDLK_q;
	hotspots[HS_VKEYB_Q + 1].remap_id = SDLK_w;
	hotspots[HS_VKEYB_Q + 2].remap_id = SDLK_e;
	hotspots[HS_VKEYB_Q + 3].remap_id = SDLK_r;
	hotspots[HS_VKEYB_Q + 4].remap_id = SDLK_t;
	hotspots[HS_VKEYB_Q + 5].remap_id = SDLK_y;
	hotspots[HS_VKEYB_Q + 6].remap_id = SDLK_u;
	hotspots[HS_VKEYB_Q + 7].remap_id = SDLK_i;
	hotspots[HS_VKEYB_Q + 8].remap_id = SDLK_o;
	hotspots[HS_VKEYB_Q + 9].remap_id = SDLK_p;
	hotspots[HS_VKEYB_A].remap_id = SDLK_a;
	hotspots[HS_VKEYB_A + 1].remap_id = SDLK_s;
	hotspots[HS_VKEYB_A + 2].remap_id = SDLK_d;
	hotspots[HS_VKEYB_A + 3].remap_id = SDLK_f;
	hotspots[HS_VKEYB_A + 4].remap_id = SDLK_g;
	hotspots[HS_VKEYB_A + 5].remap_id = SDLK_h;
	hotspots[HS_VKEYB_A + 6].remap_id = SDLK_j;
	hotspots[HS_VKEYB_A + 7].remap_id = SDLK_k;
	hotspots[HS_VKEYB_A + 8].remap_id = SDLK_l;
	hotspots[HS_VKEYB_A + 9].remap_id = SDLK_RETURN;
	hotspots[HS_VKEYB_SHIFT].flags &= ~HS_PROP_ONOFF;
	if (vkeyb.toggle_shift) {
		hotspots[HS_VKEYB_SHIFT].flags |= HS_PROP_TOGGLE;
	} else {
		hotspots[HS_VKEYB_SHIFT].flags |= HS_PROP_STICKY;
	}
	hotspots[HS_VKEYB_SHIFT].remap_id = SDLK_LSHIFT;
	hotspots[HS_VKEYB_SHIFT + 1].remap_id = SDLK_z;
	hotspots[HS_VKEYB_SHIFT + 2].remap_id = SDLK_x;
	hotspots[HS_VKEYB_SHIFT + 3].remap_id = SDLK_c;
	hotspots[HS_VKEYB_SHIFT + 4].remap_id = SDLK_v;
	hotspots[HS_VKEYB_SHIFT + 5].remap_id = SDLK_b;
	hotspots[HS_VKEYB_SHIFT + 6].remap_id = SDLK_n;
	hotspots[HS_VKEYB_SHIFT + 7].remap_id = SDLK_m;
	hotspots[HS_VKEYB_SHIFT + 8].remap_id = SDLK_PERIOD;
	hotspots[HS_VKEYB_SHIFT + 9].remap_id = SDLK_SPACE;

	/* Initialise control bar hotspots */
	for (count = 0; count < 8; count++) {
		hotspots[HS_CTB_CTB + count].gid = HS_GRP_CTB;
	}
	hotspots[HS_CTB_EXIT].remap_id = SDLK_F10;
	hotspots[HS_CTB_RESET].remap_id = SDLK_F12;
	hotspots[HS_CTB_AUTOHIDE].remap_id = SDLK_F6;
	hotspots[HS_CTB_SHIFT_TYPE].remap_id = SDLK_F7;
	hotspots[HS_CTB_INVERSE].remap_id = SDLK_F8;
	hotspots[HS_CTB_ALPHA_DN].remap_id = SDLK_HOME;
	hotspots[HS_CTB_ALPHA_UP].remap_id = SDLK_END;

	/* Resize all hotspots for the current screen dimensions */
	hotspots_resize();

	/* Update all hotspots to reflect current component states */
	hotspots_update();
}

/***************************************************************************
 * Hotspots Resize                                                         *
 ***************************************************************************/
/* This repositions and resizes all the hotspots for the current dimensions
 * of the screen. It's done once at the start and again when the screen is
 * resized and it requires that hotspots-init has been called at least once */

void hotspots_resize(void) {
	int count;

	/* Resize root hotspots */
	hotspots[HS_ROOT_CANVAS].hit_x = 0;
	hotspots[HS_ROOT_CANVAS].hit_y = 0;
	hotspots[HS_ROOT_CANVAS].hit_w = video.screen->w;
	hotspots[HS_ROOT_CANVAS].hit_h = video.screen->h;

	/* Resize the ZX81's file selector hotspots */
	if (!zx80) {
		hotspots[HS_LOAD_Q].hit_x = emulator.xoffset + 36 * video.scale;
		hotspots[HS_LOAD_Q].hit_y = emulator.yoffset + 176 * video.scale;
		hotspots[HS_LOAD_Q].hit_w = 40 * video.scale;
		hotspots[HS_LOAD_Q].hit_h = 16 * video.scale;
		hotspots[HS_LOAD_A].hit_x = emulator.xoffset + 76 * video.scale;
		hotspots[HS_LOAD_A].hit_y = emulator.yoffset + 176 * video.scale;
		hotspots[HS_LOAD_A].hit_w = 40 * video.scale;
		hotspots[HS_LOAD_A].hit_h = 16 * video.scale;
		hotspots[HS_LOAD_ENTER].hit_x = emulator.xoffset + 116 * video.scale;
		hotspots[HS_LOAD_ENTER].hit_y = emulator.yoffset + 176 * video.scale;
		hotspots[HS_LOAD_ENTER].hit_w = 88 * video.scale;
		hotspots[HS_LOAD_ENTER].hit_h = 16 * video.scale;
		hotspots[HS_LOAD_SPACE].hit_x = emulator.xoffset + 204 * video.scale;
		hotspots[HS_LOAD_SPACE].hit_y = emulator.yoffset + 176 * video.scale;
		hotspots[HS_LOAD_SPACE].hit_w = 80 * video.scale;
		hotspots[HS_LOAD_SPACE].hit_h = 16 * video.scale;
		/* Set-up hl_x/y/w/h */
		for (count = HS_LOAD_Q; count <= HS_LOAD_ENTER; count++) {
			hotspots[count].hl_x = hotspots[count].hit_x;
			hotspots[count].hl_y = hotspots[count].hit_y;
			hotspots[count].hl_w = hotspots[count].hit_w + video.scale;
			hotspots[count].hl_h = hotspots[count].hit_h;
		}
	}

	/* Resize the virtual keyboard hotspots */
	hotspots[HS_VKEYB_VKEYB].hit_x = vkeyb.xoffset;
	hotspots[HS_VKEYB_VKEYB].hit_y = vkeyb.yoffset;
	hotspots[HS_VKEYB_VKEYB].hit_w = vkeyb.scaled->w;
	hotspots[HS_VKEYB_VKEYB].hit_h = vkeyb.scaled->h;
	if (zx80) {
		for (count = 0; count < 40; count++) {
			hotspots[HS_VKEYB_1 + count].hit_x = vkeyb.xoffset + 9 *
				video.scale + (count % 10) * 24 * video.scale;
			hotspots[HS_VKEYB_1 + count].hit_y = vkeyb.yoffset + 33 *
				video.scale + (count / 10) * 26 * video.scale;
			hotspots[HS_VKEYB_1 + count].hit_w = 22 * video.scale;
			hotspots[HS_VKEYB_1 + count].hit_h = 16 * video.scale;
		}
	} else {
		for (count = 0; count < 40; count++) {
			hotspots[HS_VKEYB_1 + count].hit_x = vkeyb.xoffset + 9 *
				video.scale + (count % 10) * 24 * video.scale;
			hotspots[HS_VKEYB_1 + count].hit_y = vkeyb.yoffset + 9 *
				video.scale + (count / 10) * 32 * video.scale;
			hotspots[HS_VKEYB_1 + count].hit_w = 22 * video.scale;
			hotspots[HS_VKEYB_1 + count].hit_h = 16 * video.scale;
		}
	}

	/* Resize the control bar hotspots */
	hotspots[HS_CTB_CTB].hit_x = control_bar.xoffset;
	hotspots[HS_CTB_CTB].hit_y = control_bar.yoffset;
	hotspots[HS_CTB_CTB].hit_w = control_bar.scaled->w;
	hotspots[HS_CTB_CTB].hit_h = control_bar.scaled->h;
	/* Set-up hit_x */
	hotspots[HS_CTB_EXIT].hit_x = control_bar.xoffset + video.scale;
	hotspots[HS_CTB_RESET].hit_x = control_bar.xoffset + video.scale + 17 * 2 * video.scale;
	hotspots[HS_CTB_AUTOHIDE].hit_x = control_bar.xoffset + video.scale + 17 * 4 * video.scale;
	hotspots[HS_CTB_SHIFT_TYPE].hit_x = control_bar.xoffset + video.scale + 17 * 5 * video.scale;
	hotspots[HS_CTB_INVERSE].hit_x = control_bar.xoffset + video.scale + 17 * 6 * video.scale;
	hotspots[HS_CTB_ALPHA_DN].hit_x = control_bar.xoffset + video.scale + 17 * 7 * video.scale;
	hotspots[HS_CTB_ALPHA_UP].hit_x = control_bar.xoffset + video.scale + 17 * 8 * video.scale;
	/* Set-up hit_y/w/h */
	for (count = HS_CTB_EXIT; count <= HS_CTB_ALPHA_UP; count++) {
		hotspots[count].hit_y = control_bar.yoffset + video.scale;
		hotspots[count].hit_w = hotspots[count].hit_h = 16 * video.scale;
	}
	/* Set-up hl_x/y/w/h */
	for (count = HS_CTB_EXIT; count <= HS_CTB_ALPHA_UP; count++) {
		hotspots[count].hl_x = hotspots[count].hit_x + video.scale;
		hotspots[count].hl_y = hotspots[count].hit_y + video.scale;
		hotspots[count].hl_w = hotspots[count].hit_w - 2 * video.scale;
		hotspots[count].hl_h = hotspots[count].hit_h - 2 * video.scale;
	}

}

/***************************************************************************
 * Hotspots Update                                                         *
 ***************************************************************************/
/* This sets hotspots [in]visible depending on the states of the components
 * that affect them. It's called from within the renderer since then we will
 * know what the emulator has done since keyboard-update */

void hotspots_update(void) {
	int count;
	
	/* Update the ZX81's file selector hotspots */
	if (load_selector_state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_LOAD) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_LOAD) hotspots[count].flags &= ~HS_PROP_VISIBLE;
	}

	/* Hide the vkeyb if the file selector is active */
	if (load_selector_state && vkeyb.state) {
		vkeyb.state = FALSE; video.redraw = TRUE;
	}

	/* Update the virtual keyboard hotspots */
	if (vkeyb.state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_VKEYB) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_VKEYB) hotspots[count].flags &= ~HS_PROP_VISIBLE;
	}

	/* Update the control bar hotspots */
	if (vkeyb.state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_CTB) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_CTB) hotspots[count].flags &= ~HS_PROP_VISIBLE;
	}

}

/***************************************************************************
 * Hotspots Render                                                         *
 ***************************************************************************/
/* This renders visible hotspots over everything else and is called towards
 * the end of sdl-video-refresh. What the hotspot will look like is decided
 * here and depends especially on the gid and occassionally on the flags. */

void hotspots_render(void) {
	int hl_x, hl_y, hl_w, hl_h;
	SDL_Surface *highlight;
	int count, surround;
	int scancode, alpha;
	SDL_Rect dstrect;
	Uint32 colour;

	/* Scan through the keyboard buffer and highlight any hotspots that
	 * are currently visible that have ids that are recorded as pressed */
	for (scancode = 0; scancode < MAX_SCANCODES; scancode++) {
	#ifndef SDL_DEBUG_HOTSPOTS
		if (keyboard_buffer[scancode] == KEY_PRESSED) {
	#endif
			for (count = 0; count < MAX_HOTSPOTS; count++) {
				if (hotspots[count].gid != UNDEFINED &&
					hotspots[count].gid != HS_GRP_ROOT &&
					hotspots[count].flags & HS_PROP_VISIBLE &&
					keysym_to_scancode(TRUE, scancode) == hotspots[count].remap_id) {
					/* Set the size of the hotspot's highlight.
					 * If hl_x/y/w/h are all UNDEFINED then use hit_x/y/w/h instead */
					if (hotspots[count].hl_x == UNDEFINED && hotspots[count].hl_y == UNDEFINED &&
						hotspots[count].hl_w == UNDEFINED && hotspots[count].hl_h == UNDEFINED) {
						hl_x = hotspots[count].hit_x; hl_y = hotspots[count].hit_y;
						hl_w = hotspots[count].hit_w; hl_h = hotspots[count].hit_h;
					} else {
						hl_x = hotspots[count].hl_x; hl_y = hotspots[count].hl_y;
						hl_w = hotspots[count].hl_w; hl_h = hotspots[count].hl_h;
					}
					/* Choose a suitable colour */
					if (hotspots[count].gid == HS_GRP_LOAD) {
						colour = colours.hs_load_pressed;
					} else if (hotspots[count].gid == HS_GRP_VKEYB) {
						if (hotspots[count].flags & HS_PROP_TOGGLE &&
							hotspots[count].flags & HS_PROP_STUCK) {
							if (zx80) {
								colour = colours.hs_vkeyb_zx80_toggle_pressed;
							} else {
								colour = colours.hs_vkeyb_zx81_toggle_pressed;
							}
						} else {
							if (zx80) {
								colour = colours.hs_vkeyb_zx80_pressed;
							} else {
								colour = colours.hs_vkeyb_zx81_pressed;
							}
						}
					} else if (hotspots[count].gid == HS_GRP_CTB) {
						colour = colours.hs_ctb_pressed;
					} else {
						colour = 0xff00ff;
					}
					#ifdef SDL_DEBUG_HOTSPOTS	/* Ooh, pretty Christmas Lights Edition(tm) ;) */
						colour = (rand() % 256) << 16 | (rand() % 256) << 8 | rand() % 256;
					#endif
					/* Create an RGB surface to accommodate the highlight */
					highlight = SDL_CreateRGBSurface(SDL_SWSURFACE, hl_w, hl_h,
						video.screen->format->BitsPerPixel,
						video.screen->format->Rmask, video.screen->format->Gmask,
						video.screen->format->Bmask, video.screen->format->Amask);
					/* Fill the highlight */
					if (SDL_FillRect(highlight, NULL, SDL_MapRGB(video.screen->format,
						colour >> 16 & 0xff, colour >> 8 & 0xff, colour & 0xff)) < 0) {
						fprintf(stderr, "%s: FillRect error: %s\n", __func__,
							SDL_GetError ());
						exit(1);
					}
					/* Choose a suitable alpha */
					if (hotspots[count].gid == HS_GRP_VKEYB) {
						alpha = (vkeyb.alpha + 1) / 2;
					} else {
						alpha = 128;
					}
					/* Set the alpha for the entire surface */
					if (SDL_SetAlpha(highlight, SDL_SRCALPHA, alpha) < 0) {
						fprintf(stderr, "%s: Cannot set surface alpha: %s\n", __func__,
							SDL_GetError());
						exit(1);
					}
					/* Blit it to the screen */
					dstrect.x = hl_x; dstrect.y = hl_y; dstrect.w = hl_w; dstrect.h = hl_h;
					if (SDL_BlitSurface (highlight, NULL, video.screen, &dstrect) < 0) {
						fprintf(stderr, "%s: BlitSurface error: %s\n", __func__,
							SDL_GetError ());
						exit(1);
					}
					SDL_FreeSurface(highlight);

					/* For the load selector, add interior left and right
					 * solid bars using the current background colour */
					if (hotspots[count].gid == HS_GRP_LOAD) {
						if (!invert_screen) {
							colour = colours.emu_bg;
						} else {
							colour = colours.emu_fg;
						}
						#ifdef SDL_DEBUG_HOTSPOTS	/* Ooh, pretty Christmas Lights Edition(tm) ;) */
							colour = (rand() % 256) << 16 | (rand() % 256) << 8 | rand() % 256;
						#endif
						for (surround = 0; surround < 2; surround++) {
							if (surround == 0) {
								dstrect.x = hl_x; dstrect.y = hl_y;
								dstrect.w = video.scale; dstrect.h = hl_h;
							} else {
								dstrect.x = hl_x + hl_w - video.scale; dstrect.y = hl_y;
								dstrect.w = video.scale; dstrect.h = hl_h;
							}
							if (SDL_FillRect(video.screen, &dstrect, SDL_MapRGB(video.screen->format,
								colour >> 16 & 0xff, colour >> 8 & 0xff, colour & 0xff)) < 0) {
								fprintf(stderr, "%s: FillRect error: %s\n", __func__,
									SDL_GetError ());
								exit(1);
							}
						}
					}
				}
			}
	#ifndef SDL_DEBUG_HOTSPOTS
		}
	#endif
	}
}

/***************************************************************************
 * Fonts Initialise                                                        *
 ***************************************************************************/
/* This can be called multiple times and it is if the resolution is switched.
 * 
 * On exit: returns TRUE on error
 *          else FALSE */

int fonts_init(void) {
	SDL_Surface *unconverted;
	struct bmpfont *ptrfont;
	char filename[256];
	int count;

	/* Free all existing scaled surfaces first */
	for (count = 0; count < MAX_FONTS; count++) {
		if (zx80font.scaled[count]) SDL_FreeSurface(zx80font.scaled[count]);
		if (zx81font.scaled[count]) SDL_FreeSurface(zx81font.scaled[count]);
		if (zx82font.scaled[count]) SDL_FreeSurface(zx82font.scaled[count]);
		zx80font.scaled[count] = NULL;
		zx81font.scaled[count] = NULL;
		zx82font.scaled[count] = NULL;
	}

	for (count = 0; count < 3; count++) {
		/* Load the original scale font images once */
		if (count == BMF_FONT_ZX80) {
			ptrfont = &zx80font;
		} else if (count == BMF_FONT_ZX81) {
			ptrfont = &zx81font;
		} else {
			ptrfont = &zx82font;
		}
		if (ptrfont->original == NULL) {
			strcpy (filename, PACKAGE_DATA_DIR "/");
			if (count == BMF_FONT_ZX80) {
				strcat (filename, IMG_ZX80_FONT);
			} else if (count == BMF_FONT_ZX81) {
				strcat (filename, IMG_ZX81_FONT);
			} else {
				strcat (filename, IMG_ZX82_FONT);
			}
			/* Load the bitmap */
			if ((unconverted = SDL_LoadBMP(filename)) == NULL) {
				fprintf(stderr, "%s: Cannot load font image %s: %s\n", __func__,
					filename, SDL_GetError ());
				return TRUE;
			}
			/* Convert the font surface to the same pixel
			 * format as the screen for fast blitting */
			ptrfont->original = SDL_DisplayFormat (unconverted);
			SDL_FreeSurface(unconverted);	/* Free the redundant surface */
			if (ptrfont->original == NULL) {
				fprintf(stderr, "%s: Cannot create surface: %s\n", __func__,
					SDL_GetError ());
				return TRUE;
			}
			/* Set the transparent colour */
			if (SDL_SetColorKey (ptrfont->original, SDL_SRCCOLORKEY,
				SDL_MapRGB(video.screen->format, colours.colour_key >> 16 & 0xff,
				colours.colour_key >> 8 & 0xff, colours.colour_key & 0xff)) < 0) {
				fprintf(stderr, "%s: Cannot set surface colour key: %s\n", __func__,
					SDL_GetError ());
				return TRUE;
			}
		}

		/* Create an RGB surface to accommodate the scaled font */
		ptrfont->scaled[0] = SDL_CreateRGBSurface(SDL_SWSURFACE,
			ptrfont->original->w * video.scale, ptrfont->original->h * video.scale,
			video.screen->format->BitsPerPixel,
			video.screen->format->Rmask, video.screen->format->Gmask,
			video.screen->format->Bmask, video.screen->format->Amask);
		if (ptrfont->scaled[0] == NULL) {
			fprintf(stderr, "%s: Cannot create RGB surface: %s\n", __func__,
				SDL_GetError ());
			return TRUE;
		}
		/* Set the transparent colour */
		if (SDL_SetColorKey (ptrfont->scaled[0], SDL_SRCCOLORKEY,
			SDL_MapRGB(video.screen->format, colours.colour_key >> 16 & 0xff,
			colours.colour_key >> 8 & 0xff, colours.colour_key & 0xff)) < 0) {
			fprintf(stderr, "%s: Cannot set surface colour key: %s\n", __func__,
				SDL_GetError ());
			return TRUE;
		}

		/* Now scale the font into the new surface */
		scale_surface(ptrfont->original, ptrfont->scaled[0]);

		/* scaled[0] fg_colour for each font is the same as the original */
		ptrfont->fg_colour[0] = colours.bmf_fg_default;
	}

	#ifdef SDL_DEBUG_FONTS
		printf("zx80font.original=%p\n", zx80font.original);
		for (count = 0; count < MAX_FONTS; count++) {
			printf("  zx80font.scaled[%i]=%p\n", count, zx80font.scaled[count]);
		}
		printf("zx81font.original=%p\n", zx81font.original);
		for (count = 0; count < MAX_FONTS; count++) {
			printf("  zx81font.scaled[%i]=%p\n", count, zx81font.scaled[count]);
		}
		printf("zx82font.original=%p\n", zx82font.original);
		for (count = 0; count < MAX_FONTS; count++) {
			printf("  zx82font.scaled[%i]=%p\n", count, zx82font.scaled[count]);
		}
	#endif

	return FALSE;
}

/***************************************************************************
 * Virtual Keyboard Initialise                                             *
 ***************************************************************************/
/* This can be called multiple times and it is if the resolution is switched.
 * 
 * On exit: returns TRUE on error
 *          else FALSE */

int vkeyb_init(void) {
	SDL_Surface *unconverted;
	char filename[256];

	/* Free any existing scaled surface first */
	if (vkeyb.scaled) SDL_FreeSurface(vkeyb.scaled);

	/* Load the appropriate original scale keyboard image once */
	if (vkeyb.original == NULL) {
		strcpy (filename, PACKAGE_DATA_DIR "/");
		if (zx80) {
			strcat (filename, IMG_ZX80_KYBD);
		} else {
			strcat (filename, IMG_ZX81_KYBD);
		}
		/* Load the bitmap */
		if ((unconverted = SDL_LoadBMP(filename)) == NULL) {
			fprintf(stderr, "%s: Cannot load keyboard image %s: %s\n", __func__,
				filename, SDL_GetError ());
			return TRUE;
		}
		/* Convert the keyboard surface to the same pixel
		 * format as the screen for fast blitting */
		vkeyb.original = SDL_DisplayFormat (unconverted);
		SDL_FreeSurface(unconverted);	/* Free the redundant surface */
		if (vkeyb.original == NULL) {
			fprintf(stderr, "%s: Cannot create surface: %s\n", __func__,
				SDL_GetError ());
			return TRUE;
		}
	}
	
	/* Create an RGB surface to accommodate the scaled keyboard */
	vkeyb.scaled = SDL_CreateRGBSurface(SDL_SWSURFACE, vkeyb.original->w * video.scale,
		vkeyb.original->h * video.scale, video.screen->format->BitsPerPixel,
		video.screen->format->Rmask, video.screen->format->Gmask,
		video.screen->format->Bmask, video.screen->format->Amask);
	if (vkeyb.scaled == NULL) {
		fprintf(stderr, "%s: Cannot create RGB surface: %s\n", __func__,
			SDL_GetError ());
		return TRUE;
	}

	/* Now scale the keyboard into the new surface */
	scale_surface(vkeyb.original, vkeyb.scaled);

	/* Apply some alpha to the entire surface if required */
	if (vkeyb.alpha < SDL_ALPHA_OPAQUE) {
		if ((SDL_SetAlpha(vkeyb.scaled, SDL_SRCALPHA, vkeyb.alpha)) < 0) {
			fprintf(stderr, "%s: Cannot set surface alpha: %s\n", __func__, 
				SDL_GetError());
			return TRUE;
		}
	}

	/* Set-up the virtual keyboard's screen offset */
	vkeyb.xoffset = (video.xres - vkeyb.scaled->w) / 2;
	vkeyb.yoffset = (video.yres - vkeyb.scaled->h) / 2;

	return FALSE;
}

/***************************************************************************
 * sz81 Icons Initialise                                                   *
 ***************************************************************************/
/* This can be called multiple times and it is if the resolution is switched.
 * 
 * On exit: returns TRUE on error
 *          else FALSE */

int sz81icons_init(void) {
	SDL_Surface *unconverted;
	char filename[256];

	/* Free any existing scaled surface first */
	if (sz81icons.scaled) SDL_FreeSurface(sz81icons.scaled);

	/* Load the original scale icons once */
	if (sz81icons.original == NULL) {
		strcpy (filename, PACKAGE_DATA_DIR "/");
		strcat (filename, IMG_SZ81_ICONS);
		/* Load the bitmap */
		if ((unconverted = SDL_LoadBMP(filename)) == NULL) {
			fprintf(stderr, "%s: Cannot load icons image %s: %s\n", __func__,
				filename, SDL_GetError ());
			return TRUE;
		}
		/* Convert the icons surface to the same pixel
		 * format as the screen for fast blitting */
		sz81icons.original = SDL_DisplayFormat (unconverted);
		SDL_FreeSurface(unconverted);	/* Free the redundant surface */
		if (sz81icons.original == NULL) {
			fprintf(stderr, "%s: Cannot create surface: %s\n", __func__,
				SDL_GetError ());
			return TRUE;
		}
	}
	
	/* Create an RGB surface to accommodate the scaled icons */
	sz81icons.scaled = SDL_CreateRGBSurface(SDL_SWSURFACE, sz81icons.original->w * video.scale,
		sz81icons.original->h * video.scale, video.screen->format->BitsPerPixel,
		video.screen->format->Rmask, video.screen->format->Gmask,
		video.screen->format->Bmask, video.screen->format->Amask);
	if (sz81icons.scaled == NULL) {
		fprintf(stderr, "%s: Cannot create RGB surface: %s\n", __func__,
			SDL_GetError ());
		return TRUE;
	}

	/* Now scale the icons into the new surface */
	scale_surface(sz81icons.original, sz81icons.scaled);

	return FALSE;
}

/***************************************************************************
 * Control Bar Initialise                                                  *
 ***************************************************************************/
/* This can be called multiple times and it is if the resolution is switched.
 * 
 * On exit: returns TRUE on error
 *          else FALSE */

int control_bar_init(void) {
	SDL_Rect srcrect, dstrect;
	Uint32 colour;
	int count;

	/* Free any existing scaled surface first */
	if (control_bar.scaled) SDL_FreeSurface(control_bar.scaled);

 	/* Create an RGB surface to accommodate a scaled control bar */
	control_bar.scaled = SDL_CreateRGBSurface(SDL_SWSURFACE,
		256 * video.scale, 18 * video.scale, video.screen->format->BitsPerPixel,
		video.screen->format->Rmask, video.screen->format->Gmask,
		video.screen->format->Bmask, video.screen->format->Amask);
	if (control_bar.scaled == NULL) {
		fprintf(stderr, "%s: Cannot create RGB surface: %s\n", __func__,
			SDL_GetError ());
		return TRUE;
	}
	/* Set the transparent colour */
	if (SDL_SetColorKey (control_bar.scaled, SDL_SRCCOLORKEY,
		SDL_MapRGB(video.screen->format, colours.colour_key >> 16 & 0xff,
		colours.colour_key >> 8 & 0xff, colours.colour_key & 0xff)) < 0) {
		fprintf(stderr, "%s: Cannot set surface colour key: %s\n", __func__, SDL_GetError ());
		return TRUE;
	}
	/* Fill it with the colour key */
	if (SDL_FillRect(control_bar.scaled, NULL, SDL_MapRGB(video.screen->format,
		colours.colour_key >> 16 & 0xff, colours.colour_key >> 8 & 0xff,
		colours.colour_key & 0xff)) < 0) {
		fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
		return TRUE;
	}
	/* Create outlines for the icons */
	colour = colours.emu_fg;
	dstrect.x = dstrect.y = 0; dstrect.w = dstrect.h = 18 * video.scale;
	for (count = 0; count < 3; count++) {
		/* 17 and not 18 is used below because the outlines overlap */
		if (count > 0) dstrect.x += 17 * 2 * video.scale;
		if (count == 2) dstrect.w = 17 * 5 * video.scale + video.scale;
		if (SDL_FillRect(control_bar.scaled, &dstrect, SDL_MapRGB(video.screen->format,
			colour >> 16 & 0xff, colour >> 8 & 0xff, colour & 0xff)) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			return TRUE;
		}
	}
	/* Blit the icons */
	srcrect.w = srcrect.h = dstrect.w = dstrect.h = 16 * video.scale;
	dstrect.y = video.scale;
	/* Exit */
	srcrect.x = ICON_EXIT_X * video.scale; srcrect.y = ICON_EXIT_Y * video.scale;
	dstrect.x = video.scale;
	if (SDL_BlitSurface (sz81icons.scaled, &srcrect, control_bar.scaled, &dstrect) < 0) {
		fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
		return TRUE;
	}
	/* Reset */
	srcrect.x = ICON_RESET_X * video.scale; srcrect.y = ICON_RESET_Y * video.scale;
	dstrect.x = video.scale + 17 * 2 * video.scale; 
	if (SDL_BlitSurface (sz81icons.scaled, &srcrect, control_bar.scaled, &dstrect) < 0) {
		fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
		return TRUE;
	}
	/* Autohide */
	if (vkeyb.autohide) {
		srcrect.x = ICON_AUTOHIDE_X * video.scale; srcrect.y = ICON_AUTOHIDE_Y * video.scale;
	} else {
		srcrect.x = ICON_DONTHIDE_X * video.scale; srcrect.y = ICON_DONTHIDE_Y * video.scale;
	}
	dstrect.x = video.scale + 17 * 4 * video.scale; 
	if (SDL_BlitSurface (sz81icons.scaled, &srcrect, control_bar.scaled, &dstrect) < 0) {
		fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
		return TRUE;
	}
	/* Shift type */
	if (vkeyb.toggle_shift) {
		srcrect.x = ICON_TOGGLESHIFT_X * video.scale; srcrect.y = ICON_TOGGLESHIFT_Y * video.scale;
	} else {
		srcrect.x = ICON_STICKYSHIFT_X * video.scale; srcrect.y = ICON_STICKYSHIFT_Y * video.scale;
	}
	dstrect.x = video.scale + 17 * 5 * video.scale; 
	if (SDL_BlitSurface (sz81icons.scaled, &srcrect, control_bar.scaled, &dstrect) < 0) {
		fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
		return TRUE;
	}
	/* Inverse screen */
	if (invert_screen) {
		srcrect.x = ICON_INVERSE_X * video.scale; srcrect.y = ICON_INVERSE_Y * video.scale;
	} else {
		srcrect.x = ICON_NOTINVERSE_X * video.scale; srcrect.y = ICON_NOTINVERSE_Y * video.scale;
	}
	dstrect.x = video.scale + 17 * 6 * video.scale; 
	if (SDL_BlitSurface (sz81icons.scaled, &srcrect, control_bar.scaled, &dstrect) < 0) {
		fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
		return TRUE;
	}
	/* Alpha down */
	srcrect.x = ICON_ALPHA_DN_X * video.scale; srcrect.y = ICON_ALPHA_DN_Y * video.scale;
	dstrect.x = video.scale + 17 * 7 * video.scale; 
	if (SDL_BlitSurface (sz81icons.scaled, &srcrect, control_bar.scaled, &dstrect) < 0) {
		fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
		return TRUE;
	}
	/* Alpha up */
	srcrect.x = ICON_ALPHA_UP_X * video.scale; srcrect.y = ICON_ALPHA_UP_Y * video.scale;
	dstrect.x = video.scale + 17 * 8 * video.scale; 
	if (SDL_BlitSurface (sz81icons.scaled, &srcrect, control_bar.scaled, &dstrect) < 0) {
		fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
		return TRUE;
	}

	/* Set-up the control bar's screen offset */
	control_bar.xoffset = (video.xres - control_bar.scaled->w) / 2;
	if (control_bar.xoffset < 0) control_bar.xoffset = 0;
	control_bar.yoffset = emulator.yoffset - 19 * video.scale;
	if (control_bar.yoffset < 0) control_bar.yoffset = 0;

	return FALSE;
}

/***************************************************************************
 * Render Bitmap Font Text                                                 *
 ***************************************************************************/
/* On entry: font should be BMF_FONT_ZX80, BMF_FONT_ZX81 or BMF_FONT_ZX82
 *           text points to the ASCII text to render
 *           fg_colour is the text colour
 *           bg_colour is the background colour (if you require it to be
 *               transparent then use colours.colour_key and it will be set)
 *  On exit: returns a pointer to a surface on success (YOU must free it)
 *           else NULL on error */

SDL_Surface *BMF_RenderText(int font, char *text, Uint32 fg_colour, Uint32 bg_colour) {
	SDL_Surface *rendered_text = NULL;
	struct bmpfont *ptrfont;
	int count, index, found, x, y;
	SDL_Rect srcrect, dstrect;

	/* Set the font we are going to use */
	if (font == BMF_FONT_ZX80) {
		ptrfont = &zx80font;
	} else if (font == BMF_FONT_ZX81) {
		ptrfont = &zx81font;
	} else {
		ptrfont = &zx82font;
	}

	/* Does a font of fg_colour already exist? */
	found = FALSE;
	for (index = 0; index < MAX_FONTS; index++) {
		if (ptrfont->scaled[index] && ptrfont->fg_colour[index] == fg_colour) {
			found = TRUE;
			break;
		} else if (ptrfont->scaled[index] == NULL) {
			break;
		}
	}	
	if (index == MAX_FONTS) {
		/* Not found and no free slot remaining */
		fprintf(stderr, "%s: Cannot create font: all slots utilised\n", __func__);
		return NULL;	
	} else if (!found) {
		/* Didn't find one so we'll have to create it */
		/* Make a duplicate of font.scaled[0] the easy way */
		ptrfont->scaled[index] = SDL_DisplayFormat(ptrfont->scaled[0]);
		if (ptrfont->scaled[index] == NULL) {
			fprintf(stderr, "%s: Cannot create surface: %s\n", __func__, SDL_GetError ());
			return NULL;
		}
		/* Fill the font with the requested fg_colour */
		if (SDL_MUSTLOCK(ptrfont->scaled[index])) SDL_LockSurface(ptrfont->scaled[index]);
		for (y = 0; y < ptrfont->scaled[index]->h; y++) {
			for (x = 0; x < ptrfont->scaled[index]->w; x++) {
				if (get_pixel(ptrfont->scaled[index], x, y) == SDL_MapRGB(video.screen->format,
					colours.bmf_fg_default >> 16 & 0xff, colours.bmf_fg_default >> 8 & 0xff,
					colours.bmf_fg_default & 0xff)) {
					set_pixel(ptrfont->scaled[index], x, y, SDL_MapRGB(video.screen->format,
					fg_colour >> 16 & 0xff, fg_colour >> 8 & 0xff, fg_colour & 0xff));
				}
			}
		}
		if (SDL_MUSTLOCK(ptrfont->scaled[index])) SDL_UnlockSurface(ptrfont->scaled[index]);
	}

	/* Create an RGB surface to accommodate the rendered text */
	rendered_text = SDL_CreateRGBSurface(SDL_SWSURFACE,
		strlen(text) * 8 * video.scale, 8 * video.scale, video.screen->format->BitsPerPixel,
		video.screen->format->Rmask, video.screen->format->Gmask,
		video.screen->format->Bmask, video.screen->format->Amask);
	if (rendered_text == NULL) {
		fprintf(stderr, "%s: Cannot create RGB surface: %s\n", __func__, SDL_GetError ());
		return NULL;
	}
	/* Set the transparent colour */
	if (SDL_SetColorKey (rendered_text, SDL_SRCCOLORKEY,
		SDL_MapRGB(video.screen->format, colours.colour_key >> 16 & 0xff,
		colours.colour_key >> 8 & 0xff, colours.colour_key & 0xff)) < 0) {
		fprintf(stderr, "%s: Cannot set surface colour key: %s\n", __func__, SDL_GetError ());
		return NULL;
	}
	/* Fill it with the bg_colour */
	if (SDL_FillRect(rendered_text, NULL, SDL_MapRGB(video.screen->format,
		bg_colour >> 16 & 0xff, bg_colour >> 8 & 0xff, bg_colour & 0xff)) < 0) {
		fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
		return NULL;
	}
	/* Write the text to the new surface */
	for (count = 0; count < strlen(text); count++) {
		srcrect.x = (text[count] - 32) * 8 * video.scale; srcrect.y = 0;
		srcrect.w = srcrect.h = 8 * video.scale;
		dstrect.x = count * 8 * video.scale; dstrect.y = 0;
		dstrect.w = dstrect.h = 8 * video.scale;
		if (SDL_BlitSurface (ptrfont->scaled[index], &srcrect, rendered_text, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
		return NULL;
		}
	}

	return rendered_text;
}

/***************************************************************************
 * Get Pixel                                                               *
 ***************************************************************************/
/* This function is based upon getpixel() from the SDL documentation.
 * If surfaces must be locked/unlocked then the caller is responsible for this
 * 
 * On exit: returns a value which can be converted to RGB using SDL_GetRGB() */

Uint32 get_pixel(SDL_Surface *surface, int x, int y) {
	Uint32 retpixel;
	int bpp;
	Uint8 *offset;

	bpp = surface->format->BytesPerPixel;
	offset = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	if (bpp == 1) {
		retpixel = *offset;
	} else if (bpp == 2) {
		retpixel = *(Uint16*)offset;
	} else if (bpp == 3) {
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			retpixel = offset[0] << 16 | offset[1] << 8 | offset[2];
		} else {
			retpixel = offset[0] | offset[1] << 8 | offset[2] << 16;
		}
	} else if (bpp == 4) {
		retpixel = *(Uint32*)offset;
	} else {
		retpixel = 0;
	}

	return retpixel;
}

/***************************************************************************
 * Set Pixel                                                               *
 ***************************************************************************/
/* This function is based upon putpixel() from the SDL documentation.
 * If surfaces must be locked/unlocked then the caller is responsible for this.
 * 
 * The pixel being put should be mapped to the surface format first using SDL_MapRGB() */

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
	int bpp;
	Uint8 *offset;

	bpp = surface->format->BytesPerPixel;
	offset = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
		case 1:
			*offset = pixel;
			break;
		case 2:
			*(Uint16 *)offset = pixel;
			break;
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				offset[0] = (pixel >> 16) & 0xff;
				offset[1] = (pixel >> 8) & 0xff;
				offset[2] = pixel & 0xff;
			} else {
				offset[0] = pixel & 0xff;
				offset[1] = (pixel >> 8) & 0xff;
				offset[2] = (pixel >> 16) & 0xff;
			}
			break;
		case 4:
			*(Uint32 *)offset = pixel;
			break;
	}
}

/***************************************************************************
 * Scale Surface                                                           *
 ***************************************************************************/
/* This scales up an original surface into the scaled surface by the
 * current factor of video.scale (scaling by pixel replication) */

void scale_surface(SDL_Surface *original, SDL_Surface *scaled) {
	Uint32 *src_pixels_32, *dst_pixels_32, colour;
	Uint16 *src_pixels_16, *dst_pixels_16;
	int srcx, srcy, desx, desy;

	if (SDL_MUSTLOCK(original)) SDL_LockSurface(original);
	if (SDL_MUSTLOCK(scaled)) SDL_LockSurface(scaled);

	src_pixels_16 = original->pixels; dst_pixels_16 = scaled->pixels;
	src_pixels_32 = original->pixels; dst_pixels_32 = scaled->pixels;

	for (srcy = desy = 0; srcy < original->h; srcy++) {
		for (srcx = desx = 0; srcx < original->w; srcx++) {
			if (video.screen->format->BitsPerPixel == 16) {
				/* Get source pixel */
				colour = src_pixels_16[srcy * original->w + srcx];

				/* Write the destination pixel[s] */
				dst_pixels_16[desy * scaled->w + desx] = colour;
				if (video.scale > 1) {
					/* x2 scaling */
					dst_pixels_16[desy * scaled->w + desx + 1] = colour;
					dst_pixels_16[(desy + 1) * scaled->w + desx] = colour;
					dst_pixels_16[(desy + 1) * scaled->w + desx + 1] = colour;
					if (video.scale > 2) {
						/* x3 scaling */
						dst_pixels_16[desy * scaled->w + desx + 2] = colour;
						dst_pixels_16[(desy + 1) * scaled->w + desx + 2] = colour;
						dst_pixels_16[(desy + 2) * scaled->w + desx] = colour;
						dst_pixels_16[(desy + 2) * scaled->w + desx + 1] = colour;
						dst_pixels_16[(desy + 2) * scaled->w + desx + 2] = colour;
					}
				}
			} else if (video.screen->format->BitsPerPixel == 32) {
				/* Get source pixel */
				colour = src_pixels_32[srcy * original->w + srcx];

				/* Write the destination pixel[s] */
				dst_pixels_32[desy * scaled->w + desx] = colour;
				if (video.scale > 1) {
					/* x2 scaling */
					dst_pixels_32[desy * scaled->w + desx + 1] = colour;
					dst_pixels_32[(desy + 1) * scaled->w + desx] = colour;
					dst_pixels_32[(desy + 1) * scaled->w + desx + 1] = colour;
					if (video.scale > 2) {
						/* x3 scaling */
						dst_pixels_32[desy * scaled->w + desx + 2] = colour;
						dst_pixels_32[(desy + 1) * scaled->w + desx + 2] = colour;
						dst_pixels_32[(desy + 2) * scaled->w + desx] = colour;
						dst_pixels_32[(desy + 2) * scaled->w + desx + 1] = colour;
						dst_pixels_32[(desy + 2) * scaled->w + desx + 2] = colour;
					}
				}
			}
			desx += video.scale;
		}
		desy += video.scale;
	}

	if (SDL_MUSTLOCK(original)) SDL_LockSurface(original);
	if (SDL_MUSTLOCK(scaled)) SDL_LockSurface(scaled);
}

/***************************************************************************
 * Keysym to Scancode                                                      *
 ***************************************************************************/
/* Translates SDL keysyms to SVGAlib scancodes and the reverse */

int keysym_to_scancode(int reverse, int value) {
	
	if (!reverse) {
		switch (value) {
			case SDLK_ESCAPE: return SCANCODE_ESCAPE;
			case SDLK_F1: return SCANCODE_F1;
			case SDLK_F2: return SCANCODE_F2;
			case SDLK_F3: return SCANCODE_F3;
			case SDLK_F4: return SCANCODE_F4;
			case SDLK_F5: return SCANCODE_F5;
			case SDLK_F6: return SCANCODE_F6;
			case SDLK_F7: return SCANCODE_F7;
			case SDLK_F8: return SCANCODE_F8;
			case SDLK_F9: return SCANCODE_F9;
			case SDLK_F10: return SCANCODE_F10;
			case SDLK_F11: return SCANCODE_F11;
			case SDLK_F12: return SCANCODE_F12;
			case SDLK_HOME: return SCANCODE_HOME;
			case SDLK_END: return SCANCODE_END;
			case SDLK_LSHIFT: return SCANCODE_LEFTSHIFT;
			case SDLK_RSHIFT: return SCANCODE_RIGHTSHIFT;
			case SDLK_LCTRL: return SCANCODE_LEFTCONTROL;
			case SDLK_RCTRL: return SCANCODE_RIGHTCONTROL;
			case SDLK_z: return SCANCODE_Z;
			case SDLK_x: return SCANCODE_X;
			case SDLK_c: return SCANCODE_C;
			case SDLK_v: return SCANCODE_V;
			case SDLK_a: return SCANCODE_A;
			case SDLK_s: return SCANCODE_S;
			case SDLK_d: return SCANCODE_D;
			case SDLK_f: return SCANCODE_F;
			case SDLK_g: return SCANCODE_G;
			case SDLK_q: return SCANCODE_Q;
			case SDLK_w: return SCANCODE_W;
			case SDLK_e: return SCANCODE_E;
			case SDLK_r: return SCANCODE_R;
			case SDLK_t: return SCANCODE_T;
			case SDLK_1: return SCANCODE_1;
			case SDLK_2: return SCANCODE_2;
			case SDLK_3: return SCANCODE_3;
			case SDLK_4: return SCANCODE_4;
			case SDLK_5: return SCANCODE_5;
			case SDLK_0: return SCANCODE_0;
			case SDLK_9: return SCANCODE_9;
			case SDLK_8: return SCANCODE_8;
			case SDLK_7: return SCANCODE_7;
			case SDLK_6: return SCANCODE_6;
			case SDLK_p: return SCANCODE_P;
			case SDLK_o: return SCANCODE_O;
			case SDLK_i: return SCANCODE_I;
			case SDLK_u: return SCANCODE_U;
			case SDLK_y: return SCANCODE_Y;
			case SDLK_RETURN: return SCANCODE_ENTER;
			case SDLK_l: return SCANCODE_L;
			case SDLK_k: return SCANCODE_K;
			case SDLK_j: return SCANCODE_J;
			case SDLK_h: return SCANCODE_H;
			case SDLK_SPACE: return SCANCODE_SPACE;
			case SDLK_PERIOD: return SCANCODE_PERIOD;
			case SDLK_m: return SCANCODE_M;
			case SDLK_n: return SCANCODE_N;
			case SDLK_b: return SCANCODE_B;
			default: return 0;
		}
	} else {
		switch (value) {
			case SCANCODE_ESCAPE: return SDLK_ESCAPE;
			case SCANCODE_F1: return SDLK_F1;
			case SCANCODE_F2: return SDLK_F2;
			case SCANCODE_F3: return SDLK_F3;
			case SCANCODE_F4: return SDLK_F4;
			case SCANCODE_F5: return SDLK_F5;
			case SCANCODE_F6: return SDLK_F6;
			case SCANCODE_F7: return SDLK_F7;
			case SCANCODE_F8: return SDLK_F8;
			case SCANCODE_F9: return SDLK_F9;
			case SCANCODE_F10: return SDLK_F10;
			case SCANCODE_F11: return SDLK_F11;
			case SCANCODE_F12: return SDLK_F12;
			case SCANCODE_HOME: return SDLK_HOME;
			case SCANCODE_END: return SDLK_END;
			case SCANCODE_LEFTSHIFT: return SDLK_LSHIFT;
			case SCANCODE_RIGHTSHIFT: return SDLK_RSHIFT;
			case SCANCODE_LEFTCONTROL: return SDLK_LCTRL;
			case SCANCODE_RIGHTCONTROL: return SDLK_RCTRL;
			case SCANCODE_Z: return SDLK_z;
			case SCANCODE_X: return SDLK_x;
			case SCANCODE_C: return SDLK_c;
			case SCANCODE_V: return SDLK_v;
			case SCANCODE_A: return SDLK_a;
			case SCANCODE_S: return SDLK_s;
			case SCANCODE_D: return SDLK_d;
			case SCANCODE_F: return SDLK_f;
			case SCANCODE_G: return SDLK_g;
			case SCANCODE_Q: return SDLK_q;
			case SCANCODE_W: return SDLK_w;
			case SCANCODE_E: return SDLK_e;
			case SCANCODE_R: return SDLK_r;
			case SCANCODE_T: return SDLK_t;
			case SCANCODE_1: return SDLK_1;
			case SCANCODE_2: return SDLK_2;
			case SCANCODE_3: return SDLK_3;
			case SCANCODE_4: return SDLK_4;
			case SCANCODE_5: return SDLK_5;
			case SCANCODE_0: return SDLK_0;
			case SCANCODE_9: return SDLK_9;
			case SCANCODE_8: return SDLK_8;
			case SCANCODE_7: return SDLK_7;
			case SCANCODE_6: return SDLK_6;
			case SCANCODE_P: return SDLK_p;
			case SCANCODE_O: return SDLK_o;
			case SCANCODE_I: return SDLK_i;
			case SCANCODE_U: return SDLK_u;
			case SCANCODE_Y: return SDLK_y;
			case SCANCODE_ENTER: return SDLK_RETURN;
			case SCANCODE_L: return SDLK_l;
			case SCANCODE_K: return SDLK_k;
			case SCANCODE_J: return SDLK_j;
			case SCANCODE_H: return SDLK_h;
			case SCANCODE_SPACE: return SDLK_SPACE;
			case SCANCODE_PERIOD: return SDLK_PERIOD;
			case SCANCODE_M: return SDLK_m;
			case SCANCODE_N: return SDLK_n;
			case SCANCODE_B: return SDLK_b;
			default: return 0;
		}
	}
}

/***************************************************************************
 * Video Refresh                                                           *
 ***************************************************************************/
/* Updates the SDL screen surface.
 * 
 * What it actually does is convert the data from the 8 bit 320x200 VGA
 * graphics memory to the n bit SDL screen surface, scale if required and
 * centre the emulator within the dimensions of the screen */

void sdl_video_refresh(void) {
	Uint32 colour, fg_colour, bg_colour;
	int srcx, srcy, desx, desy, srcw;
	Uint32 *screen_pixels_32;
	Uint16 *screen_pixels_16;
	SDL_Rect dstrect;
	#ifdef SDL_DEBUG_FONTS
		struct bmpfont *ptrfont;
		int count, fontcount;
	#endif
	#ifdef SDL_DEBUG_TIMING
		static Uint32 lasttime = 0;
		static int Hz = 0;

		Hz++;
		if (SDL_GetTicks() - lasttime >= 1000) {
			printf("%s=%iHz\n", __func__, Hz);
			lasttime = SDL_GetTicks();
			Hz = 0;
		}
	#endif

	/* Update hotspot states now */
	hotspots_update();

	/* Prepare the colours we shall be using */
	fg_colour = SDL_MapRGB(video.screen->format, colours.emu_fg >> 16 & 0xff,
		colours.emu_fg >> 8 & 0xff, colours.emu_fg & 0xff);
	bg_colour = SDL_MapRGB(video.screen->format, colours.emu_bg >> 16 & 0xff,
		colours.emu_bg >> 8 & 0xff, colours.emu_bg & 0xff);

	/* Should everything be redrawn? */
	if (video.redraw) {
		video.redraw = FALSE;
		/* Wipe the entire screen surface */
		#ifdef SDL_DEBUG_VIDEO
			colour = SDL_MapRGB(video.screen->format, 0x0, 0x80, 0xc0);
		#else
			if (!invert_screen) {
				colour = bg_colour;
			} else {
				colour = fg_colour;
			}
		#endif
		if (SDL_FillRect(video.screen, NULL, colour) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
	}

	if (SDL_MUSTLOCK(video.screen)) SDL_LockSurface(video.screen);

	screen_pixels_16 = video.screen->pixels;
	screen_pixels_32 = video.screen->pixels;

	/* Set-up destination y coordinates */
	desy = emulator.yoffset;

	for (srcy = 0; srcy < 200; srcy++) {
		
		/* [Re]set-up x coordinates and src width */
		if (video.xres < 320 * video.scale) {
			srcx = abs(emulator.xoffset / video.scale);
			if (zx80 && video.xres < 256 * video.scale)
				srcx += 8 * 2;	/* The emulator shifts it right 2 chars! */
			srcw = video.xres / video.scale + srcx; desx = 0;
		} else {
			srcx = 0;
			srcw = 320; desx = emulator.xoffset;
		}
		
		for (;srcx < srcw; srcx++) {
			/* Get 8 bit source pixel */
			if (vga_graphmemory[srcy * 320 + srcx] == 0) {
				colour = fg_colour;
			} else {
				colour = bg_colour;
			}

			if (video.screen->format->BitsPerPixel == 16) {
				/* Write the destination pixel[s] */
				screen_pixels_16[desy * video.xres + desx] = colour;
				if (video.scale > 1) {
					/* x2 scaling */
					screen_pixels_16[desy * video.xres + desx + 1] = colour;
					screen_pixels_16[(desy + 1) * video.xres + desx] = colour;
					screen_pixels_16[(desy + 1) * video.xres + desx + 1] = colour;
					if (video.scale > 2) {
						/* x3 scaling */
						screen_pixels_16[desy * video.xres + desx + 2] = colour;
						screen_pixels_16[(desy + 1) * video.xres + desx + 2] = colour;
						screen_pixels_16[(desy + 2) * video.xres + desx] = colour;
						screen_pixels_16[(desy + 2) * video.xres + desx + 1] = colour;
						screen_pixels_16[(desy + 2) * video.xres + desx + 2] = colour;
					}
				}
			} else if (video.screen->format->BitsPerPixel == 32) {
				/* Write the destination pixel[s] */
				screen_pixels_32[desy * video.xres + desx] = colour;
				if (video.scale > 1) {
					/* x2 scaling */
					screen_pixels_32[desy * video.xres + desx + 1] = colour;
					screen_pixels_32[(desy + 1) * video.xres + desx] = colour;
					screen_pixels_32[(desy + 1) * video.xres + desx + 1] = colour;
					if (video.scale > 2) {
						/* x3 scaling */
						screen_pixels_32[desy * video.xres + desx + 2] = colour;
						screen_pixels_32[(desy + 1) * video.xres + desx + 2] = colour;
						screen_pixels_32[(desy + 2) * video.xres + desx] = colour;
						screen_pixels_32[(desy + 2) * video.xres + desx + 1] = colour;
						screen_pixels_32[(desy + 2) * video.xres + desx + 2] = colour;
					}
				}
			}
			desx += video.scale;
		}
		desy += video.scale;
	}

	if (SDL_MUSTLOCK(video.screen)) SDL_UnlockSurface(video.screen);
	
	/* Do we need to overlay the virtual keyboard? */
	if (vkeyb.state) {
		dstrect.x = vkeyb.xoffset; dstrect.y = vkeyb.yoffset;
		dstrect.w = vkeyb.scaled->w; dstrect.h =  vkeyb.scaled->h;
		if (SDL_BlitSurface (vkeyb.scaled, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__,
				SDL_GetError ());
			exit(1);
		}
		/* Show the control bar too */
		dstrect.x = control_bar.xoffset; dstrect.y = control_bar.yoffset;
		dstrect.w = control_bar.scaled->w; dstrect.h =  control_bar.scaled->h;
		if (SDL_BlitSurface (control_bar.scaled, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
	}

	/* Render visible hotspots now */
	hotspots_render();

	#ifdef SDL_DEBUG_FONTS
		/* Define SDL_DEBUG_VIDEO too and press alt+r a few
		 * times to see all of the fonts original and scaled */
		dstrect.x = dstrect.y = 0;
		for (fontcount = 0; fontcount < 3; fontcount++) {
			if (fontcount == BMF_FONT_ZX80) {
				ptrfont = &zx80font;
			} else if (fontcount == BMF_FONT_ZX81) {
				ptrfont = &zx81font;
			} else {
				ptrfont = &zx82font;
			}
			dstrect.w = ptrfont->original->w; dstrect.h = ptrfont->original->h;
			if (SDL_BlitSurface (ptrfont->original, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__,
					SDL_GetError ());
				exit(1);
			}
			dstrect.y += dstrect.h;
			for (count = 0; count < MAX_FONTS; count++) {
				if (ptrfont->scaled[count]) {
					dstrect.w = ptrfont->scaled[count]->w; dstrect.h = ptrfont->scaled[count]->h;
					if (SDL_BlitSurface (ptrfont->scaled[count], NULL, video.screen, &dstrect) < 0) {
						fprintf(stderr, "%s: BlitSurface error: %s\n", __func__,
							SDL_GetError ());
						exit(1);
					}
					dstrect.y += dstrect.h;
				}
			}
		}
	#endif

	SDL_Flip(video.screen);
}

/***************************************************************************
 * Cycle Resolutions                                                       *
 ***************************************************************************/
/* Cycle to the next resolution */

void cycle_resolutions(void) {

	#if defined(SDL_DEBUG_VIDEO)
		/* 1024x600 | 960x720 | 800x480 | 640x480 | 480x640 | 320x240 | 240x320 */
		if (video.xres == 1024) {
			video.xres = 960; video.yres = 720; video.scale = 3;
		} else if (video.xres == 960) {
			video.xres = 800; video.yres = 480; video.scale = 2;
		} else if (video.xres == 800) {
			video.xres = 640; video.yres = 480; video.scale = 2;
		} else if (video.xres == 640) {
			video.xres = 480; video.yres = 640; video.scale = 2;
		} else if (video.xres == 480) {
			video.xres = 320; video.yres = 240; video.scale = 1;
		} else if (video.xres == 320) {
			video.xres = 240; video.yres = 320; video.scale = 1;
		} else {
			video.xres = 1024; video.yres = 600; video.scale = 2;
		}
	#elif defined(PLATFORM_GP2X)
	#elif defined(PLATFORM_ZAURUS)
	#else
		/* 960x720 | 640x480 | 320x240 */
		if (video.xres == 960) {
			video.xres = 640; video.yres = 480; video.scale = 2;
		} else if (video.xres == 640) {
			video.xres = 320; video.yres = 240; video.scale = 1;
		} else {
			video.xres = 960; video.yres = 720; video.scale = 3;
		}
	#endif
}

/***************************************************************************
 * Emulator Timer                                                          *
 ***************************************************************************/
/* I've still yet to convert sound to use the SDL API. Currently if OSS sound
 * is enabled then it's used for timing, otherwise this SDL timer is used */

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

	if (wm_icon) SDL_FreeSurface(wm_icon);

	SDL_Quit();
}





