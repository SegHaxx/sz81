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
#define MAX_KEYSYMS (138 + 6)

/* SVGAlib stuff that remains */
#define KEY_NOTPRESSED 0
#define KEY_PRESSED 1

#define JOYDEADZONE (32767 * joystick_dead_zone / 100)

/* Cursor (virtual) device control IDs */
#define CURSOR_REMAP 32762
#define CURSOR_N 32763
#define CURSOR_S 32764
#define CURSOR_W 32765
#define CURSOR_E 32766
#define CURSOR_HIT 32767

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

/* Variables */
int device, id, mod_id, state;
int last_runopt_comp = COMP_RUNOPT0;
SDL_Event event;

char *keysyms[] = {
	"SDLK_UNKNOWN", "SDLK_FIRST", "SDLK_BACKSPACE", "SDLK_TAB", "SDLK_CLEAR", 
	"SDLK_RETURN", "SDLK_PAUSE", "SDLK_ESCAPE", "SDLK_SPACE", "SDLK_EXCLAIM", 
	"SDLK_QUOTEDBL", "SDLK_HASH", "SDLK_DOLLAR", "SDLK_AMPERSAND", "SDLK_QUOTE",
	"SDLK_LEFTPAREN", "SDLK_RIGHTPAREN", "SDLK_ASTERISK", "SDLK_PLUS", "SDLK_COMMA",
	"SDLK_MINUS", "SDLK_PERIOD", "SDLK_SLASH", "SDLK_0", "SDLK_1", 
	"SDLK_2", "SDLK_3", "SDLK_4", "SDLK_5", "SDLK_6", 
	"SDLK_7", "SDLK_8", "SDLK_9", "SDLK_COLON", "SDLK_SEMICOLON", 
	"SDLK_LESS", "SDLK_EQUALS", "SDLK_GREATER", "SDLK_QUESTION", "SDLK_AT", 
	"SDLK_LEFTBRACKET", "SDLK_BACKSLASH", "SDLK_RIGHTBRACKET", "SDLK_CARET", "SDLK_UNDERSCORE", 
	"SDLK_BACKQUOTE", "SDLK_a", "SDLK_b", "SDLK_c", "SDLK_d", 
	"SDLK_e", "SDLK_f", "SDLK_g", "SDLK_h", "SDLK_i", 
	"SDLK_j", "SDLK_k", "SDLK_l", "SDLK_m", "SDLK_n", 
	"SDLK_o", "SDLK_p", "SDLK_q", "SDLK_r", "SDLK_s", 
	"SDLK_t", "SDLK_u", "SDLK_v", "SDLK_w", "SDLK_x", 
	"SDLK_y", "SDLK_z", "SDLK_DELETE", "SDLK_KP0", "SDLK_KP1", 
	"SDLK_KP2", "SDLK_KP3", "SDLK_KP4", "SDLK_KP5", "SDLK_KP6", 
	"SDLK_KP7", "SDLK_KP8", "SDLK_KP9", "SDLK_KP_PERIOD", "SDLK_KP_DIVIDE", 
	"SDLK_KP_MULTIPLY", "SDLK_KP_MINUS", "SDLK_KP_PLUS", "SDLK_KP_ENTER", "SDLK_KP_EQUALS", 
	"SDLK_UP", "SDLK_DOWN", "SDLK_RIGHT", "SDLK_LEFT", "SDLK_INSERT", 
	"SDLK_HOME", "SDLK_END", "SDLK_PAGEUP", "SDLK_PAGEDOWN", "SDLK_F1", 
	"SDLK_F2", "SDLK_F3", "SDLK_F4", "SDLK_F5", "SDLK_F6", 
	"SDLK_F7", "SDLK_F8", "SDLK_F9", "SDLK_F10", "SDLK_F11", 
	"SDLK_F12", "SDLK_F13", "SDLK_F14", "SDLK_F15", "SDLK_NUMLOCK", 
	"SDLK_CAPSLOCK", "SDLK_SCROLLOCK", "SDLK_RSHIFT", "SDLK_LSHIFT", "SDLK_RCTRL", 
	"SDLK_LCTRL", "SDLK_RALT", "SDLK_LALT", "SDLK_RMETA", "SDLK_LMETA", 
	"SDLK_LSUPER", "SDLK_RSUPER", "SDLK_MODE", "SDLK_COMPOSE", "SDLK_HELP", 
	"SDLK_PRINT", "SDLK_SYSREQ", "SDLK_BREAK", "SDLK_MENU", "SDLK_POWER", 
	"SDLK_EURO", "SDLK_UNDO", "SDLK_LAST", "CURSOR_REMAP", "CURSOR_N", 
	"CURSOR_S", "CURSOR_W", "CURSOR_E", "CURSOR_HIT"};
int keycodes[] = {
	SDLK_UNKNOWN, SDLK_FIRST, SDLK_BACKSPACE, SDLK_TAB, SDLK_CLEAR, 
	SDLK_RETURN, SDLK_PAUSE, SDLK_ESCAPE, SDLK_SPACE, SDLK_EXCLAIM, 
	SDLK_QUOTEDBL, SDLK_HASH, SDLK_DOLLAR, SDLK_AMPERSAND, SDLK_QUOTE,
	SDLK_LEFTPAREN, SDLK_RIGHTPAREN, SDLK_ASTERISK, SDLK_PLUS, SDLK_COMMA,
	SDLK_MINUS, SDLK_PERIOD, SDLK_SLASH, SDLK_0, SDLK_1, 
	SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, 
	SDLK_7, SDLK_8, SDLK_9, SDLK_COLON, SDLK_SEMICOLON, 
	SDLK_LESS, SDLK_EQUALS, SDLK_GREATER, SDLK_QUESTION, SDLK_AT, 
	SDLK_LEFTBRACKET, SDLK_BACKSLASH, SDLK_RIGHTBRACKET, SDLK_CARET, SDLK_UNDERSCORE, 
	SDLK_BACKQUOTE, SDLK_a, SDLK_b, SDLK_c, SDLK_d, 
	SDLK_e, SDLK_f, SDLK_g, SDLK_h, SDLK_i, 
	SDLK_j, SDLK_k, SDLK_l, SDLK_m, SDLK_n, 
	SDLK_o, SDLK_p, SDLK_q, SDLK_r, SDLK_s, 
	SDLK_t, SDLK_u, SDLK_v, SDLK_w, SDLK_x, 
	SDLK_y, SDLK_z, SDLK_DELETE, SDLK_KP0, SDLK_KP1, 
	SDLK_KP2, SDLK_KP3, SDLK_KP4, SDLK_KP5, SDLK_KP6, 
	SDLK_KP7, SDLK_KP8, SDLK_KP9, SDLK_KP_PERIOD, SDLK_KP_DIVIDE, 
	SDLK_KP_MULTIPLY, SDLK_KP_MINUS, SDLK_KP_PLUS, SDLK_KP_ENTER, SDLK_KP_EQUALS, 
	SDLK_UP, SDLK_DOWN, SDLK_RIGHT, SDLK_LEFT, SDLK_INSERT, 
	SDLK_HOME, SDLK_END, SDLK_PAGEUP, SDLK_PAGEDOWN, SDLK_F1, 
	SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5, SDLK_F6, 
	SDLK_F7, SDLK_F8, SDLK_F9, SDLK_F10, SDLK_F11, 
	SDLK_F12, SDLK_F13, SDLK_F14, SDLK_F15, SDLK_NUMLOCK, 
	SDLK_CAPSLOCK, SDLK_SCROLLOCK, SDLK_RSHIFT, SDLK_LSHIFT, SDLK_RCTRL, 
	SDLK_LCTRL, SDLK_RALT, SDLK_LALT, SDLK_RMETA, SDLK_LMETA, 
	SDLK_LSUPER, SDLK_RSUPER, SDLK_MODE, SDLK_COMPOSE, SDLK_HELP, 
	SDLK_PRINT, SDLK_SYSREQ, SDLK_BREAK, SDLK_MENU, SDLK_POWER, 
	SDLK_EURO, SDLK_UNDO, SDLK_LAST, CURSOR_REMAP, CURSOR_N, 
	CURSOR_S, CURSOR_W, CURSOR_E, CURSOR_HIT};

/* Function prototypes */
void manage_all_input(void);
void manage_vkeyb_input(void);
void manage_runopt_input(void);
Uint32 adjust_colour_component(Uint32 rgb, Uint32 mask, int amount, int granulate);


/***************************************************************************
 * Keyboard Initialise                                                     *
 ***************************************************************************/
/* This initialises the keyboard buffer, opens joystick 0 if available,
 * sets up the control remappings and initialises the hotspots.
 * It's only called once.
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
		ctrl_remaps[count].components = UNDEFINED;
		ctrl_remaps[count].protected = UNDEFINED;
		ctrl_remaps[count].device = UNDEFINED;
		ctrl_remaps[count].id = UNDEFINED;
		ctrl_remaps[count].remap_device = UNDEFINED;
		ctrl_remaps[count].remap_id = UNDEFINED;
		ctrl_remaps[count].remap_mod_id = UNDEFINED;
	}

	/* Open joystick 0 if available */
	SDL_JoystickEventState(SDL_ENABLE);			
	joystick = SDL_JoystickOpen(0);
	if (joystick) {
		fprintf(stdout, "Joystick opened: 0:%s\n", SDL_JoystickName(0));
		/* Flush events as opening a joystick releases all the buttons */
		while (SDL_PollEvent (&event));
	}

	/* Set some default controls for platform */
	index = -1;
	/* Keyboard to keyboard */
	/* Universally active */
	ctrl_remaps[++index].components = COMP_ALL;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_RSHIFT;
	ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
	ctrl_remaps[index].remap_id = SDLK_LSHIFT;

	/* Active within the emulator and vkeyb/ctb only */
	ctrl_remaps[++index].components = COMP_EMU | COMP_VKEYB;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_UP;
	ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
	ctrl_remaps[index].remap_id = SDLK_7;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].components = COMP_EMU | COMP_VKEYB;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_DOWN;
	ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
	ctrl_remaps[index].remap_id = SDLK_6;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].components = COMP_EMU | COMP_VKEYB;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_LEFT;
	ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
	ctrl_remaps[index].remap_id = SDLK_5;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].components = COMP_EMU | COMP_VKEYB;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_RIGHT;
	ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
	ctrl_remaps[index].remap_id = SDLK_8;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].components = COMP_EMU | COMP_VKEYB;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_BACKSPACE;
	ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
	ctrl_remaps[index].remap_id = SDLK_0;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].components = COMP_EMU | COMP_VKEYB;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_COMMA;
	ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
	ctrl_remaps[index].remap_id = SDLK_PERIOD;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	/* Active within the runopt only */
	ctrl_remaps[++index].components = COMP_RUNOPT0 | COMP_RUNOPT1;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_UP;
	ctrl_remaps[index].remap_device = DEVICE_CURSOR;
	ctrl_remaps[index].remap_id = CURSOR_N;

	ctrl_remaps[++index].components = COMP_RUNOPT0 | COMP_RUNOPT1;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_DOWN;
	ctrl_remaps[index].remap_device = DEVICE_CURSOR;
	ctrl_remaps[index].remap_id = CURSOR_S;

	ctrl_remaps[++index].components = COMP_RUNOPT0 | COMP_RUNOPT1;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_LEFT;
	ctrl_remaps[index].remap_device = DEVICE_CURSOR;
	ctrl_remaps[index].remap_id = CURSOR_W;

	ctrl_remaps[++index].components = COMP_RUNOPT0 | COMP_RUNOPT1;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_RIGHT;
	ctrl_remaps[index].remap_device = DEVICE_CURSOR;
	ctrl_remaps[index].remap_id = CURSOR_E;

	ctrl_remaps[++index].components = COMP_RUNOPT0 | COMP_RUNOPT1;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_RETURN;
	ctrl_remaps[index].remap_device = DEVICE_CURSOR;
	ctrl_remaps[index].remap_id = CURSOR_HIT;

	/* Joystick to some other device.
	 * Platform specific joysticks can be hardcoded with some defaults
	 * here but otherwise their configurations should be entirely read
	 * in from the rcfile since their button/axis layouts are unknown */
	if (joystick) {
		#ifdef PLATFORM_GP2X
			/* Universally active */
			ctrl_remaps[++index].components = COMP_ALL;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_START;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_F1;

			ctrl_remaps[++index].components = COMP_ALL;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_SELECT;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_REMAP;

			/* Active within the emulator only */
			ctrl_remaps[++index].components = COMP_EMU;
			ctrl_remaps[index].protected = FALSE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_N;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_q;

			ctrl_remaps[++index].components = COMP_EMU;
			ctrl_remaps[index].protected = FALSE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_S;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_a;

			ctrl_remaps[++index].components = COMP_EMU;
			ctrl_remaps[index].protected = FALSE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_W;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_o;

			ctrl_remaps[++index].components = COMP_EMU;
			ctrl_remaps[index].protected = FALSE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_E;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_p;

			ctrl_remaps[++index].components = COMP_EMU;
			ctrl_remaps[index].protected = FALSE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_Y;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_0;
			ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

			ctrl_remaps[++index].components = COMP_EMU;
			ctrl_remaps[index].protected = FALSE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_X;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_SPACE;

			ctrl_remaps[++index].components = COMP_EMU;
			ctrl_remaps[index].protected = FALSE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_A;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_RETURN;

			ctrl_remaps[++index].components = COMP_EMU;
			ctrl_remaps[index].protected = FALSE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_B;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_RETURN;

			ctrl_remaps[++index].components = COMP_EMU;
			ctrl_remaps[index].protected = FALSE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_LTRIG;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_LSHIFT;

			/* Active within the load selector only */
			ctrl_remaps[++index].components = COMP_LOAD;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_N;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_q;

			ctrl_remaps[++index].components = COMP_LOAD;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_S;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_a;

			ctrl_remaps[++index].components = COMP_LOAD;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_W;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_W;

			ctrl_remaps[++index].components = COMP_LOAD;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_E;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_E;

			ctrl_remaps[++index].components = COMP_LOAD;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_X;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_SPACE;

			ctrl_remaps[++index].components = COMP_LOAD;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_A;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_HIT;

			ctrl_remaps[++index].components = COMP_LOAD;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_B;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_RETURN;

			/* Active within the virtual keyboard only */
			ctrl_remaps[++index].components = COMP_VKEYB;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_N;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_N;

			ctrl_remaps[++index].components = COMP_VKEYB;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_S;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_S;

			ctrl_remaps[++index].components = COMP_VKEYB;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_W;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_W;

			ctrl_remaps[++index].components = COMP_VKEYB;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_E;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_E;

			ctrl_remaps[++index].components = COMP_VKEYB;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_Y;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_0;
			ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

			ctrl_remaps[++index].components = COMP_VKEYB;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_X;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_SPACE;

			ctrl_remaps[++index].components = COMP_VKEYB;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_A;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_HIT;

			ctrl_remaps[++index].components = COMP_VKEYB;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_B;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_RETURN;

			ctrl_remaps[++index].components = COMP_VKEYB;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_LTRIG;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_LSHIFT;
		#else
			if (strcmp(SDL_JoystickName(0),
				"Microsoft SideWinder Game Pad Pro USB version 1.0") == 0) {
				/* This is my own PC joystick which I've been using for testing
				 * and I'll leave the configuration here as somebody else might
				 * use the same joystick (probably not) and it also serves as a
				 * useful configuration example */
				/* Universally active */
				ctrl_remaps[++index].components = COMP_ALL;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 2;	/* C */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_F1;

				ctrl_remaps[++index].components = COMP_ALL;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 5;	/* Z */
				ctrl_remaps[index].remap_device = DEVICE_CURSOR;
				ctrl_remaps[index].remap_id = CURSOR_REMAP;

				/* Active within the emulator only */
				ctrl_remaps[++index].components = COMP_EMU;
				ctrl_remaps[index].protected = FALSE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 12;	/* Up */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_q;

				ctrl_remaps[++index].components = COMP_EMU;
				ctrl_remaps[index].protected = FALSE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 13;	/* Down */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_a;

				ctrl_remaps[++index].components = COMP_EMU;
				ctrl_remaps[index].protected = FALSE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 10;	/* Left */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_o;

				ctrl_remaps[++index].components = COMP_EMU;
				ctrl_remaps[index].protected = FALSE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 11;	/* Right */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_p;

				ctrl_remaps[++index].components = COMP_EMU;
				ctrl_remaps[index].protected = FALSE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 4;	/* Y */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_0;
				ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

				ctrl_remaps[++index].components = COMP_EMU;
				ctrl_remaps[index].protected = FALSE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 0;	/* A */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_SPACE;

				ctrl_remaps[++index].components = COMP_EMU;
				ctrl_remaps[index].protected = FALSE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 3;	/* X */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_RETURN;

				ctrl_remaps[++index].components = COMP_EMU;
				ctrl_remaps[index].protected = FALSE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 1;	/* B */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_RETURN;

				ctrl_remaps[++index].components = COMP_EMU;
				ctrl_remaps[index].protected = FALSE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 6;	/* LTrig */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_LSHIFT;

				/* Active within the load selector only */
				ctrl_remaps[++index].components = COMP_LOAD;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 12;	/* Up */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_q;

				ctrl_remaps[++index].components = COMP_LOAD;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 13;	/* Down */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_a;

				ctrl_remaps[++index].components = COMP_LOAD;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 10;	/* Left */
				ctrl_remaps[index].remap_device = DEVICE_CURSOR;
				ctrl_remaps[index].remap_id = CURSOR_W;

				ctrl_remaps[++index].components = COMP_LOAD;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 11;	/* Right */
				ctrl_remaps[index].remap_device = DEVICE_CURSOR;
				ctrl_remaps[index].remap_id = CURSOR_E;

				ctrl_remaps[++index].components = COMP_LOAD;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 0;	/* A */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_SPACE;

				ctrl_remaps[++index].components = COMP_LOAD;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 3;	/* X */
				ctrl_remaps[index].remap_device = DEVICE_CURSOR;
				ctrl_remaps[index].remap_id = CURSOR_HIT;

				ctrl_remaps[++index].components = COMP_LOAD;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 1;	/* B */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_RETURN;

				/* Active within the virtual keyboard only */
				ctrl_remaps[++index].components = COMP_VKEYB;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 12;	/* Up */
				ctrl_remaps[index].remap_device = DEVICE_CURSOR;
				ctrl_remaps[index].remap_id = CURSOR_N;

				ctrl_remaps[++index].components = COMP_VKEYB;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 13;	/* Down */
				ctrl_remaps[index].remap_device = DEVICE_CURSOR;
				ctrl_remaps[index].remap_id = CURSOR_S;

				ctrl_remaps[++index].components = COMP_VKEYB;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 10;	/* Left */
				ctrl_remaps[index].remap_device = DEVICE_CURSOR;
				ctrl_remaps[index].remap_id = CURSOR_W;

				ctrl_remaps[++index].components = COMP_VKEYB;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 11;	/* Right */
				ctrl_remaps[index].remap_device = DEVICE_CURSOR;
				ctrl_remaps[index].remap_id = CURSOR_E;

				ctrl_remaps[++index].components = COMP_VKEYB;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 4;	/* Y */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_0;
				ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

				ctrl_remaps[++index].components = COMP_VKEYB;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 0;	/* A */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_SPACE;

				ctrl_remaps[++index].components = COMP_VKEYB;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 3;	/* X */
				ctrl_remaps[index].remap_device = DEVICE_CURSOR;
				ctrl_remaps[index].remap_id = CURSOR_HIT;

				ctrl_remaps[++index].components = COMP_VKEYB;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 1;	/* B */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_RETURN;

				ctrl_remaps[++index].components = COMP_VKEYB;
				ctrl_remaps[index].protected = TRUE;
				ctrl_remaps[index].device = DEVICE_JOYSTICK;
				ctrl_remaps[index].id = 6;	/* LTrig */
				ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				ctrl_remaps[index].remap_id = SDLK_LSHIFT;
			}
		#endif
	}
	#ifdef SDL_DEBUG_EVENTS
		printf("%s: ctrl_remaps index=%i\n", __func__, index);
	#endif

	/* Initialise the hotspots now */
	hotspots_init();

	/* Initialise the control remapper */
	ctrl_remapper.state = FALSE;
	ctrl_remapper.master_interval = CTRL_REMAPPER_INTERVAL / (1000 / (emulator.speed / scrn_freq));
	ctrl_remapper.interval = 0;

	return 0;
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
	static int skip_update = TRUE, last_hs_pressed[2];
	static int axisstates[MAX_JOY_AXES * 2], init = TRUE;
	int hs_load_selected, hs_vkeyb_ctb_selected;
	int eventfound = FALSE, count, found;
	int axis_end = 0;
	SDL_Event virtualevent;
	SDLMod modstate;
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

	/* Do some first time initialisation */
	if (init) {
		init = FALSE;
		last_hs_pressed[0] = last_hs_pressed[1] = UNDEFINED;
		for (count = 0; count < MAX_JOY_AXES; count++)
			axisstates[count * 2] = axisstates[count * 2 + 1] = SDL_RELEASED;
		/* Flush events (may have been done earlier too when opening a joystick */
		while (SDL_PollEvent (&event));
		event.type = SDL_NOEVENT;	/* Important */
	}

	/* Process events every other call otherwise the emulator or the
	 * emulated machine has trouble keeping up with event creation */
	if (!(skip_update = !skip_update)) {

		/* If there's something to repeat then maybe do it now. As this
		 * is done inside the skip-update the default granularity is
		 * 40ms: 1000ms / (50Hz / 2) but this can change if the emulation
		 * speed is altered e.g. 1000ms / (80Hz / 2) = 25ms */
		key_repeat_manager(KRM_FUNC_TICK, NULL, 0);

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
					break;
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
					/* This code is also visited by joystick button events
					 * masquerading as mouse button events. To enable them
					 * to coexit peacefully, the joy buttons start at 128 */
					if (event.button.button % 128 == SDL_BUTTON_LEFT) {
						device = DEVICE_KEYBOARD;
						state = event.button.state;
						/* Convert a hotspot hit into an id */
						found = FALSE;
						if (state == SDL_PRESSED) {
							for (count = MAX_HOTSPOTS - 1; count >= 0; count--) {
								if (hotspots[count].gid != UNDEFINED &&
									hotspots[count].flags & HS_PROP_VISIBLE &&
									event.button.x >= hotspots[count].hit_x &&
									event.button.x < hotspots[count].hit_x + hotspots[count].hit_w &&
									event.button.y >= hotspots[count].hit_y &&
									event.button.y < hotspots[count].hit_y + hotspots[count].hit_h) {
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
										last_hs_pressed[event.button.button / 128] = count;
										found = TRUE;
									}
									break;
								}
							}
						} else {
							if (hotspots[last_hs_pressed[event.button.button / 128]].flags &
								HS_PROP_ONOFF) {
								/* If a virtual keyboard hotspot was last pressed
								 * then release stuck sticky hotspots */
								if (hotspots[last_hs_pressed[event.button.button / 128]].gid ==
									HS_GRP_VKEYB) {
									for (count = MAX_HOTSPOTS - 1; count >= 0; count--) {
										if (hotspots[count].gid != UNDEFINED &&
											hotspots[count].flags & HS_PROP_STICKY &&
											hotspots[count].flags & HS_PROP_STUCK) {
											hotspots[count].flags &= ~HS_PROP_STUCK;
											virtualevent.type = SDL_KEYUP;
											virtualevent.key.keysym.sym = hotspots[count].remap_id;
											virtualevent.key.state = SDL_RELEASED;
											SDL_PushEvent(&virtualevent);
										}
									}
								}
								id = hotspots[last_hs_pressed[event.button.button / 128]].remap_id;
								last_hs_pressed[event.button.button / 128] = UNDEFINED;
								found = TRUE;
							}
						}
						if (!found) device = UNDEFINED;	/* Ignore id */
					}
					break;
				case SDL_JOYBUTTONUP:
				case SDL_JOYBUTTONDOWN:
					device = DEVICE_JOYSTICK;
					id = event.jbutton.button;
					state = event.jbutton.state;
					break;
				case SDL_JOYAXISMOTION:
					/* The following joy axis code I wrote for the GNU Robbo project :-
					 * 
					 * The axis motion will be reported as a value from -32768 to +32767
					 * with 0 being the theoretical middle point. Some analogue joysticks
					 * are actually digital and offer limited analogue functionality by
					 * firing off a few values throughout this range (these joysticks will
					 * tend to rest at 0) whilst a truly analogue joystick will fire lots
					 * of different values with the middle firing anything but 0. The
					 * JOYDEADZONE offers us a reasonable user configurable middle region.
					 * 
					 * To simulate digital on/off button functionality from an analogue
					 * axis we need to record previous axis motion activity so that we can
					 * see if we've already generated an SDL_JOYBUTTONDOWN/UP event. This
					 * activity is stored within the axisstates array as pairs of values
					 * representing both ends of an axis, with each value belonging to a
					 * virtual button. The first integer records the up/down state of the
					 * negative end of an axis with the positive end stored in the other.
					 * 
					 * Following are the situations that this code will account for :-
					 *   SDL_JOYBUTTONDOWN - axis middle to any axis end
					 *   SDL_JOYBUTTONDOWN - one axis end to another axis end with the
					 *   opposite end generating SDL_JOYBUTTONUP
					 *   SDL_JOYBUTTONUP - any axis end to the middle */
					if (abs(event.jaxis.value) >= JOYDEADZONE) {
						/* Identify which end of the axis we are operating upon:
						 * 0 for -32768 to JOYDEADZONE, 1 for JOYDEADZONE to +32767 */
						axis_end = 0;
						if (event.jaxis.value >= JOYDEADZONE) axis_end = 1;

						if (axisstates[event.jaxis.axis * 2 + axis_end] == SDL_RELEASED) {
							/* Generate a virtual joystick button event */
							virtualevent.jbutton.button = event.jaxis.axis * 2 +
								axis_end + SDL_JoystickNumButtons (joystick);
							virtualevent.type = SDL_JOYBUTTONDOWN;
							virtualevent.jbutton.state = SDL_PRESSED;
							SDL_PushEvent(&virtualevent);

							/* Record its SDL_PRESSED state within axisstates */
							axisstates[event.jaxis.axis * 2 + axis_end] = SDL_PRESSED;
						}

						/* Do we need to release the opposite end of the axis? */
						if (axisstates[event.jaxis.axis * 2 + (1 - axis_end)] == SDL_PRESSED) {
							/* Generate a virtual joystick button event */
							virtualevent.jbutton.button = event.jaxis.axis * 2 +
								(1 - axis_end) + SDL_JoystickNumButtons (joystick);
							virtualevent.type = SDL_JOYBUTTONUP;
							virtualevent.jbutton.state = SDL_RELEASED;
							SDL_PushEvent(&virtualevent);

							/* Record its SDL_RELEASED state within axisstates */
							axisstates[event.jaxis.axis * 2 + (1 - axis_end)] = SDL_RELEASED;
						}
					} else {
						/* Do we need to release either end of the axis? */
						for (count = 0; count < 2; count++) {
							if (axisstates[event.jaxis.axis * 2 + count] == SDL_PRESSED) {
								/* Generate a virtual joystick button event */
								virtualevent.jbutton.button = event.jaxis.axis * 2 +
									count + SDL_JoystickNumButtons (joystick);
								virtualevent.type = SDL_JOYBUTTONUP;
								virtualevent.jbutton.state = SDL_RELEASED;
								SDL_PushEvent(&virtualevent);

								/* Record its SDL_RELEASED state within axisstates */
								axisstates[event.jaxis.axis * 2 + count] = SDL_RELEASED;
							}
						}
					}
					break;
				case SDL_QUIT:
					exit_program();
					break;
				default:
					break;
			}

			/* If the user has passed the -d option then record the
			 * currently pressed id for later displaying on-screen */
			if (sdl_cl_show_input_id && device != UNDEFINED) {
				if (state == SDL_PRESSED) {
					current_input_id = id;
				} else {
					current_input_id = UNDEFINED;
					video.redraw = TRUE;
				}
			}

			#ifdef SDL_DEBUG_EVENTS
				if (device != UNDEFINED) {
					printf("%s: device=%i: id=%i state=%i mod_id=%i\n",
						__func__, device, id, state, mod_id);
				}
			#endif

			if (!ctrl_remapper.state) {
				/* Remap controls from any input device to any input device including a
				 * virtual cursor device. So whatever was found above we can now convert
				 * into anything we like, including expanding one event into two -
				 * id and mod_id. Controls can also be assigned to one or more components
				 * so for example when the load selector is active, joy up and down are
				 * "q" and "a", but at other times they can be something else */
				if (device != UNDEFINED) {
					for (count = 0; count < MAX_CTRL_REMAPS; count++) {
						if (ctrl_remaps[count].device != UNDEFINED &&
							get_active_component() & ctrl_remaps[count].components &&
							device == ctrl_remaps[count].device && id == ctrl_remaps[count].id) {
							device = ctrl_remaps[count].remap_device;
							id = ctrl_remaps[count].remap_id;
							mod_id = ctrl_remaps[count].remap_mod_id;
							break;
						}
					}
				}

				/* Manage virtual cursor device input here. This is how it works :-
				 * A real hardware joystick event occurs which will go through the control
				 * remapper above and reach here as either a keyboard event or a cursor event.
				 * Keyboard events will just continue on down through this function, possibly
				 * getting grabbed (ooh matron) or intercepted and stored in the keyboard
				 * buffer for the emulator to pick up. Cursor events though need to be dealt
				 * with now as either they are moving a selection box around a GUI component
				 * or the user has hit a hotspot which will result in the hit being remapped
				 * again to a mouse button. This system maintains a physical hardware input 
				 * layer at the bottom, a dynamic virtual input layer in the middle with the
				 * target actions at the top */
				if (device == DEVICE_CURSOR) {
					/* Locate currently selected hotspot for group LOAD visible or not */
					for (count = 0; count < MAX_HOTSPOTS; count++)
						if (hotspots[count].gid == HS_GRP_LOAD &&
							hotspots[count].flags & HS_PROP_SELECTED) break;
					hs_load_selected = count;
					/* Locate currently selected hotspot for group VKEYB + CTB visible or not */
					for (count = 0; count < MAX_HOTSPOTS; count++)
						if ((hotspots[count].gid == HS_GRP_VKEYB || hotspots[count].gid == HS_GRP_CTB) &&
							hotspots[count].flags & HS_PROP_SELECTED) break;
					hs_vkeyb_ctb_selected = count;
					/* Process the events */
					if (state == SDL_PRESSED) {
						if (id == CURSOR_HIT) {
							/* Remap the virtual cursor event to a mouse button event
							 * which will hit a hotspot and generate a keyboard event.
							 * I'm adding 128 to the mouse button index to make them
							 * uniquely identifiable in the hotspot code above */
							virtualevent.type = SDL_MOUSEBUTTONDOWN;
							virtualevent.button.button = 128 + SDL_BUTTON_LEFT;
							virtualevent.button.state = SDL_PRESSED;
							if (load_selector_state) {
								virtualevent.button.x = hotspots[hs_load_selected].hit_x;
								virtualevent.button.y = hotspots[hs_load_selected].hit_y;
								SDL_PushEvent(&virtualevent);
							} else if (vkeyb.state) {
								virtualevent.button.x = hotspots[hs_vkeyb_ctb_selected].hit_x;
								virtualevent.button.y = hotspots[hs_vkeyb_ctb_selected].hit_y;
								SDL_PushEvent(&virtualevent);
							}
						} else if (id == CURSOR_REMAP) {
							/* Initiate joystick control remapping if the virtual keyboard is
							 * active (this could be extended to include other components later) */
							if (get_active_component() == COMP_VKEYB) {
								ctrl_remapper.state = TRUE;
							}
						} else if (id == CURSOR_N) {
							if (load_selector_state) {
							} else if (vkeyb.state) {
								/* Move the selector up */
								key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_VKEYB * CURSOR_N);
								hotspots[hs_vkeyb_ctb_selected].flags &= ~HS_PROP_SELECTED;
								if (hs_vkeyb_ctb_selected >= HS_CTB_EXIT && 
									hs_vkeyb_ctb_selected <= HS_CTB_ALPHA_UP) {
									hotspots[HS_VKEYB_SHIFT + hs_vkeyb_ctb_selected -
										HS_CTB_EXIT].flags |= HS_PROP_SELECTED;
								} else if (hs_vkeyb_ctb_selected == HS_CTB_RUNOPTS) {
									hotspots[HS_VKEYB_SHIFT + 9].flags |= HS_PROP_SELECTED;
								} else if (hs_vkeyb_ctb_selected >= HS_VKEYB_1 &&
									hs_vkeyb_ctb_selected <= HS_VKEYB_1 + 6) {
									hotspots[HS_CTB_EXIT + hs_vkeyb_ctb_selected -
										HS_VKEYB_1].flags |= HS_PROP_SELECTED;
								} else if (hs_vkeyb_ctb_selected >= HS_VKEYB_1 + 7 &&
									hs_vkeyb_ctb_selected <= HS_VKEYB_1 + 8) {
									hotspots[HS_VKEYB_SHIFT + hs_vkeyb_ctb_selected -
										HS_VKEYB_1].flags |= HS_PROP_SELECTED;
								} else if (hs_vkeyb_ctb_selected == HS_VKEYB_1 + 9) {
									hotspots[HS_CTB_RUNOPTS].flags |= HS_PROP_SELECTED;
								} else if (hs_vkeyb_ctb_selected >= HS_VKEYB_Q) {
									hotspots[hs_vkeyb_ctb_selected - 10].flags |= HS_PROP_SELECTED;
								}
							}
						} else if (id == CURSOR_S) {
							if (load_selector_state) {
							} else if (vkeyb.state) {
								/* Move the selector down */
								key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_VKEYB * CURSOR_S);
								hotspots[hs_vkeyb_ctb_selected].flags &= ~HS_PROP_SELECTED;
								if (hs_vkeyb_ctb_selected >= HS_VKEYB_SHIFT &&
									hs_vkeyb_ctb_selected <= HS_VKEYB_SHIFT + 6) {
									hotspots[HS_CTB_EXIT + hs_vkeyb_ctb_selected -
										HS_VKEYB_SHIFT].flags |= HS_PROP_SELECTED;
								} else if (hs_vkeyb_ctb_selected >= HS_VKEYB_SHIFT + 7 &&
									hs_vkeyb_ctb_selected <= HS_VKEYB_SHIFT + 8) {
									hotspots[HS_VKEYB_1 + hs_vkeyb_ctb_selected -
										HS_VKEYB_SHIFT].flags |= HS_PROP_SELECTED;
								} else if (hs_vkeyb_ctb_selected == HS_VKEYB_SHIFT + 9) {
									hotspots[HS_CTB_RUNOPTS].flags |= HS_PROP_SELECTED;
								} else if (hs_vkeyb_ctb_selected >= HS_CTB_EXIT && 
									hs_vkeyb_ctb_selected <= HS_CTB_ALPHA_UP) {
									hotspots[HS_VKEYB_1 + hs_vkeyb_ctb_selected -
										HS_CTB_EXIT].flags |= HS_PROP_SELECTED;
								} else if (hs_vkeyb_ctb_selected == HS_CTB_RUNOPTS) {
									hotspots[HS_VKEYB_1 + 9].flags |= HS_PROP_SELECTED;
								} else if (hs_vkeyb_ctb_selected <= HS_VKEYB_A + 9) {
									hotspots[hs_vkeyb_ctb_selected + 10].flags |= HS_PROP_SELECTED;
								}
							}
						} else if (id == CURSOR_W) {
							/* Move the selector left */
							if (load_selector_state) {
								key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_LOAD * CURSOR_W);
								hotspots[hs_load_selected].flags &= ~HS_PROP_SELECTED;
								if (--hs_load_selected < HS_LOAD_Q) hs_load_selected = HS_LOAD_SPACE;
								hotspots[hs_load_selected].flags |= HS_PROP_SELECTED;
							} else if (vkeyb.state) {
								key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_VKEYB * CURSOR_W);
								hotspots[hs_vkeyb_ctb_selected].flags &= ~HS_PROP_SELECTED;
								if (hotspots[hs_vkeyb_ctb_selected].gid == HS_GRP_CTB) {
									if (--hs_vkeyb_ctb_selected < HS_CTB_EXIT)
										hs_vkeyb_ctb_selected = HS_CTB_RUNOPTS;
									hotspots[hs_vkeyb_ctb_selected].flags |= HS_PROP_SELECTED;
								} else {
									if (hs_vkeyb_ctb_selected == HS_VKEYB_1 ||
										hs_vkeyb_ctb_selected == HS_VKEYB_Q ||
										hs_vkeyb_ctb_selected == HS_VKEYB_A ||
										hs_vkeyb_ctb_selected == HS_VKEYB_SHIFT) {
										hotspots[hs_vkeyb_ctb_selected + 9].flags |= HS_PROP_SELECTED;
									} else {
										hotspots[--hs_vkeyb_ctb_selected].flags |= HS_PROP_SELECTED;
									}
								}
							}
						} else if (id == CURSOR_E) {
							/* Move the selector right */
							if (load_selector_state) {
								key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_LOAD * CURSOR_E);
								hotspots[hs_load_selected].flags &= ~HS_PROP_SELECTED;
								if (++hs_load_selected > HS_LOAD_SPACE) hs_load_selected = HS_LOAD_Q;
								hotspots[hs_load_selected].flags |= HS_PROP_SELECTED;
							} else if (vkeyb.state) {
								key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_VKEYB * CURSOR_E);
								hotspots[hs_vkeyb_ctb_selected].flags &= ~HS_PROP_SELECTED;
								if (hotspots[hs_vkeyb_ctb_selected].gid == HS_GRP_CTB) {
									if (++hs_vkeyb_ctb_selected > HS_CTB_RUNOPTS)
										hs_vkeyb_ctb_selected = HS_CTB_EXIT;
									hotspots[hs_vkeyb_ctb_selected].flags |= HS_PROP_SELECTED;
								} else {
									if (hs_vkeyb_ctb_selected == HS_VKEYB_1 + 9 ||
										hs_vkeyb_ctb_selected == HS_VKEYB_Q + 9 ||
										hs_vkeyb_ctb_selected == HS_VKEYB_A + 9 ||
										hs_vkeyb_ctb_selected == HS_VKEYB_SHIFT + 9) {
										hotspots[hs_vkeyb_ctb_selected - 9].flags |= HS_PROP_SELECTED;
									} else {
										hotspots[++hs_vkeyb_ctb_selected].flags |= HS_PROP_SELECTED;
									}
								}
							}
						}
					} else {	/* SDL_RELEASED */
						if (id == CURSOR_HIT) {
							/* Release a previous press (it's not important where) */
							virtualevent.type = SDL_MOUSEBUTTONUP;
							virtualevent.button.button = 128 + SDL_BUTTON_LEFT;
							virtualevent.button.state = SDL_RELEASED;
							virtualevent.button.x = 0;
							virtualevent.button.y = 0;
							SDL_PushEvent(&virtualevent);
						} else if (id == CURSOR_N) {
						} else if (id == CURSOR_S) {
						} else if (id == CURSOR_W) {
						} else if (id == CURSOR_E) {
						}
						key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
					}
					device = UNDEFINED;	/* Erase it - it'll be ignored below anyway */
				}

				/* Manage sz81 input */
				if (device == DEVICE_KEYBOARD) {
					found = FALSE;
					modstate = SDL_GetModState();
					if (id == SDLK_r && (modstate & (KMOD_ALT | KMOD_MODE))) {
						/* Cycle screen resolutions on supported platforms */
						if (state == SDL_PRESSED) {
							cycle_resolutions();
							sdl_video_setmode();
						}
						found = TRUE;
					}
					if (found) device = UNDEFINED;	/* Ignore id and mod_id */
				}

				/* Manage COMP_RUNOPT0 and COMP_RUNOPT1 input */
				if (get_active_component() == COMP_RUNOPT0 ||
					get_active_component() == COMP_RUNOPT1) manage_runopt_input();
				
				/* Manage COMP_VKEYB and COMP_CTB input */
				if (get_active_component() == COMP_VKEYB ||
					get_active_component() == COMP_CTB) manage_vkeyb_input();

				/* Manage COMP_ALL input */
				manage_all_input();

				/* Make sure that stuck sticky and toggle hotspots are not released
				 * by real hardware which would leave their stuck states intact but
				 * erase their pressed states within the keyboard buffer */
				if (device == DEVICE_KEYBOARD && state == SDL_RELEASED) {
					for (count = MAX_HOTSPOTS - 1; count >= 0; count--) {
						/* Since the necessary property checks would have been applied when
						 * the hotspot was stuck we can just check for stuck and the remap_id */
						if (hotspots[count].gid != UNDEFINED && hotspots[count].flags & HS_PROP_STUCK) {
							if (hotspots[count].remap_id == id) device = UNDEFINED;	/* Ignore id and mod_id */
							if (hotspots[count].remap_id == mod_id) mod_id = UNDEFINED;	/* Ignore mod_id */
						}
					}
				}
			
				/* Record the real/virtual/remapped event within the keyboard buffer */
				if (device == DEVICE_KEYBOARD) {
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
				if (device == DEVICE_KEYBOARD && id == SDLK_RETURN &&
					state == SDL_RELEASED && vkeyb.state && vkeyb.autohide &&
					keyboard_buffer[keysym_to_scancode(FALSE, SDLK_LSHIFT)] == KEY_NOTPRESSED) {
					vkeyb.state = FALSE;
				}

				/* If we've recorded something then quit now */
				if (eventfound) break;
				
			} else {
				/* A new control remapping is currently being recorded */
				if (device != UNDEFINED) {
					if (state == SDL_PRESSED) {
						ctrl_remapper.state = FALSE;
						/* Locate currently selected hotspot for group VKEYB + CTB */
						for (count = 0; count < MAX_HOTSPOTS; count++)
							if ((hotspots[count].gid == HS_GRP_VKEYB || hotspots[count].gid == HS_GRP_CTB) &&
								hotspots[count].flags & HS_PROP_SELECTED) break;
						hs_vkeyb_ctb_selected = count;
						found = FALSE;
						for (count = 0; count < MAX_CTRL_REMAPS; count++) {
							if (ctrl_remaps[count].device == device && 
								ctrl_remaps[count].id == id &&
								ctrl_remaps[count].remap_device == DEVICE_CURSOR &&
								ctrl_remaps[count].remap_id == CURSOR_REMAP) {
								/* The user cancelled with CURSOR_REMAP */
								break;
							} else if (ctrl_remaps[count].components & COMP_EMU &&
								ctrl_remaps[count].device == device && 
								ctrl_remaps[count].id == id) {
								/* Overwrite existing if not protected */
								if (ctrl_remaps[count].protected == FALSE) found = TRUE;
								break;
							} else if (ctrl_remaps[count].device == UNDEFINED) {
								/* Insert new */
								ctrl_remaps[count].components = COMP_ALL;
								ctrl_remaps[count].protected = FALSE;
								ctrl_remaps[count].device = device;
								ctrl_remaps[count].id = id;
								found = TRUE;
								break;
							}
						}
						if (found) {
							/* Record it */
							ctrl_remaps[count].remap_device = DEVICE_KEYBOARD;
							ctrl_remaps[count].remap_id = hotspots[hs_vkeyb_ctb_selected].remap_id;
							ctrl_remaps[count].remap_mod_id = UNDEFINED;
							if (keyboard_buffer[keysym_to_scancode(FALSE, SDLK_LSHIFT)] ==
								KEY_PRESSED) ctrl_remaps[count].remap_mod_id = SDLK_LSHIFT;
							rcfile.rewrite = TRUE;
						}
					}
				}
			}
		}
	}

	return eventfound;
}

/***************************************************************************
 * Manage All Input                                                        *
 ***************************************************************************/

void manage_all_input(void) {
	static int last_vkeyb_state = FALSE;

	/* Note that I'm currently ignoring modifier states */
	if (device == DEVICE_KEYBOARD) {
		if (id == SDLK_F1) {
			/* Toggle the virtual keyboard */
			if (state == SDL_PRESSED) {
				if (!load_selector_state && 
					!runtime_options0.state && !runtime_options1.state) {
					vkeyb.state = !vkeyb.state;
				}
			}
		} else if (id == SDLK_F2) {
			/* Reserved for future Save State */
			if (state == SDL_PRESSED) {
			}
		} else if (id == SDLK_F3) {
			/* Reserved for future Load State */
			if (state == SDL_PRESSED) {
			}
		} else if (id == SDLK_F4) {
			/* Reserved for future Emulation Speed reduce */
			if (state == SDL_PRESSED) {
			}
		} else if (id == SDLK_F5) {
			/* Reserved for future Emulation Speed increase */
			if (state == SDL_PRESSED) {
			}
		} else if (id == SDLK_F8) {
			/* Toggle invert screen */
			if (state == SDL_PRESSED) {
				invert_screen = !invert_screen;
				refresh_screen = 1;
				control_bar_init();
				video.redraw = TRUE;
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
					sdl_video_setmode();
				}
			#endif
		} else if (id == SDLK_F12) {
			/* Reset the emulator */
			if (state == SDL_PRESSED) {
				if (!load_selector_state &&
					!runtime_options0.state && !runtime_options1.state) {
					if (!ignore_esc) reset81();
				}
			}
		} else if (id == SDLK_ESCAPE) {
			/* Toggle runtime options */
			if (state == SDL_PRESSED) {
				if (!load_selector_state) {
					if (!runtime_options0.state && !runtime_options1.state) {
						if (last_runopt_comp == COMP_RUNOPT0) {
							runtime_options0.state = TRUE;
						} else {
							runtime_options1.state = TRUE;
						}
						emulator.state = FALSE;
						last_vkeyb_state = vkeyb.state;	/* Preserve vkeyb state */
					} else {
						runtime_options0.state = runtime_options1.state = FALSE;
						emulator.state = TRUE;
						vkeyb.state = last_vkeyb_state;	/* Restore vkeyb state */
					}
				}
			}
		} else if (id == SDLK_MINUS || id == SDLK_EQUALS) {
			/* Adjust the volume */
			if (state == SDL_PRESSED) {
				key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPT1 * id);
				if (id == SDLK_MINUS) {
					if (sdl_sound.volume > 0) {
						sdl_sound.volume -= 2;
						#ifdef OSS_SOUND_SUPPORT
							sound_ay_setvol();
						#endif
					}
				} else {
					if (sdl_sound.volume < 128) {
						sdl_sound.volume += 2;
						#ifdef OSS_SOUND_SUPPORT
							sound_ay_setvol();
						#endif
					}
				}
			} else {
				key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
			}
		}
	}
}

/***************************************************************************
 * Manage Virtual Keyboard (and Control Bar) Input                         *
 ***************************************************************************/

void manage_vkeyb_input(void) {
	/* Note that I'm currently ignoring modifier states */
	if (device == DEVICE_KEYBOARD) {
		if (id == SDLK_F6) {
			/* Toggle the vkeyb autohide state */
			if (state == SDL_PRESSED) {
				vkeyb.autohide = !vkeyb.autohide;
				control_bar_init();
				video.redraw = TRUE;
			}
		} else if (id == SDLK_F7) {
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
		} else if (id == SDLK_HOME || id == SDLK_END) {
			/* Adjust the vkeyb alpha */
			if (state == SDL_PRESSED) {
				key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_CTB * id);
				if (id == SDLK_HOME && vkeyb.alpha == SDL_ALPHA_OPAQUE) {
					vkeyb.alpha -= 15;
				} else if (id == SDLK_HOME && vkeyb.alpha >= 16) {
					vkeyb.alpha -= 16;
				} else if (id == SDLK_END && vkeyb.alpha == 240) {
					vkeyb.alpha += 15;
				} else if (id == SDLK_END && vkeyb.alpha < 240) {
					vkeyb.alpha += 16;
				}
				if ((SDL_SetAlpha(vkeyb.scaled, SDL_SRCALPHA, vkeyb.alpha)) < 0) {
					fprintf(stderr, "%s: Cannot set surface alpha: %s\n", __func__,
						SDL_GetError());
					exit(1);
				}
				video.redraw = TRUE;
			} else {
				key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
			}
		}
	}
}

/***************************************************************************
 * Manage Runtime Options Input                                            *
 ***************************************************************************/

void manage_runopt_input(void) {
	int amount;
	
	/* Note that I'm currently ignoring modifier states */
	if (device == DEVICE_KEYBOARD) {
		if (id == SDLK_F2) {
			/* Save */
			if (state == SDL_PRESSED) {
			}
		} else if (id == SDLK_PAGEUP || id == SDLK_PAGEDOWN) {
			/* < Back and Next > */
			if (state == SDL_PRESSED) {
				if (runtime_options0.state) {
					if (id == SDLK_PAGEDOWN) {
						runtime_options0.state = FALSE;
						runtime_options1.state = TRUE;
						last_runopt_comp = COMP_RUNOPT1;
					}
				} else if (runtime_options1.state) {
					if (id == SDLK_PAGEUP) {
						runtime_options1.state = FALSE;
						runtime_options0.state = TRUE;
						last_runopt_comp = COMP_RUNOPT0;
					}
				}
			}
		} else if (id == SDLK_HOME || id == SDLK_END) {
			if (runtime_options0.state) {
				/* Key repeat delay < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPT1 * id);
					if (id == SDLK_HOME) {
						if (sdl_key_repeat.delay > 40) sdl_key_repeat.delay -= 10;
					} else {
						if (sdl_key_repeat.delay < 520) sdl_key_repeat.delay += 10;
					}
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_INSERT || id == SDLK_DELETE) {
			if (runtime_options0.state) {
				/* Key repeat interval < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPT1 * id);
					if (id == SDLK_INSERT) {
						if (sdl_key_repeat.interval > 40) sdl_key_repeat.interval -= 10;
					} else {
						if (sdl_key_repeat.interval < 520) sdl_key_repeat.interval += 10;
					}
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_d || id == SDLK_u) {
			if (runtime_options1.state) {
				/* Joy Dead Zone < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPT0 * id);
					if (id == SDLK_d) {
						if (joystick_dead_zone > 2) joystick_dead_zone -= 2;
					} else {
						if (joystick_dead_zone < 98) joystick_dead_zone += 2;
					}
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			} else if (runtime_options0.state) {
				/* Foreground colour red < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPT1 * id);
					amount = 8; if (id == SDLK_d) amount = -8;
					colours.emu_fg = adjust_colour_component(colours.emu_fg, 0xff0000, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_0 || id == SDLK_1) {
			if (runtime_options0.state) {
				/* Foreground colour green < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPT1 * id);
					amount = 8; if (id == SDLK_0) amount = -8;
					colours.emu_fg = adjust_colour_component(colours.emu_fg, 0x00ff00, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_2 || id == SDLK_3) {
			if (runtime_options0.state) {
				/* Foreground colour blue < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPT1 * id);
					amount = 8; if (id == SDLK_2) amount = -8;
					colours.emu_fg = adjust_colour_component(colours.emu_fg, 0x0000ff, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_4 || id == SDLK_5) {
			if (runtime_options0.state) {
				/* Background colour red < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPT1 * id);
					amount = 8; if (id == SDLK_4) amount = -8;
					colours.emu_bg = adjust_colour_component(colours.emu_bg, 0xff0000, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_6 || id == SDLK_7) {
			if (runtime_options0.state) {
				/* Background colour green < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPT1 * id);
					amount = 8; if (id == SDLK_6) amount = -8;
					colours.emu_bg = adjust_colour_component(colours.emu_bg, 0x00ff00, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_8 || id == SDLK_9) {
			if (runtime_options0.state) {
				/* Background colour blue < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPT1 * id);
					amount = 8; if (id == SDLK_8) amount = -8;
					colours.emu_bg = adjust_colour_component(colours.emu_bg, 0x0000ff, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		}
	}
}

/***************************************************************************
 * Key Repeat Manager                                                      *
 ***************************************************************************/
/* Here's how to use this simple and very effective key repeating function :-
 * 
 * If an event has triggered something that you'd like to repeat then call
 * key_repeat_manager(KRM_FUNC_REPEAT, &event, eventid) and it will repeat
 * after a period of time. Additionally you will need to call
 * key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0) in your event release code
 * to stop it repeating else you'll cause a catastrophic chain reaction :s
 * 
 * The eventid is a unique id that can be used to detect when a new event is
 * being repeated and the user hasn't released the previous one so that the
 * interval can be reset to the initial delay.
 * 
 * At the top of your event managing function call
 * key_repeat_manager(KRM_FUNC_TICK, NULL, 0) to provide this function with
 * a heartbeat.
 * 
 * That's it. Focus on the action that you want to repeat and repeat the
 * event that triggered it. Then the input device becomes irrelevant.
 * 
 * On entry: funcid = KRM_FUNC_TICK to provide key repeat functionality
 *                    from your event manager with event being NULL and
 *                    eventid being 0
 *           funcid = KRM_FUNC_REPEAT to set a key repeat with
 *                    event pointing to the event to repeat and
 *                    eventid = something unique
 *           funcid = KRM_FUNC_RELEASE to clear any current key repeat
 *                    with event being NULL and
 *                    eventid being 0
 */

void key_repeat_manager(int funcid, SDL_Event *event, int eventid) {
	static int interval = 0, last_eventid = 0, init = TRUE;
	static SDL_Event repeatevent;
		
	if (init) {
		init = FALSE;
		key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
	}

	if (funcid == KRM_FUNC_RELEASE) {
		repeatevent.type = SDL_NOEVENT;
		last_eventid = eventid;
		/* Reset to the initial delay (* 2 as this is currently running at half the emulation Hz) */
		interval = sdl_key_repeat.delay / (1000 / emulator.speed * 2);
		/* When interval is 1 there can be some odd behaviour, presumably
		 * because the event queue is being flooded with presses and so
		 * I'm limiting it to 2 which stops this happening. At the default
		 * emulation speed of 50Hz and with this running at half that, 2
		 * equates to 80ms */
		if (interval < 2) interval = 2;
	} else if (funcid == KRM_FUNC_TICK) {
		if (repeatevent.type != SDL_NOEVENT) {
			if (--interval <= 0) {
				/* Reset the interval (* 2 as this is currently running at half the emulator Hz) */
				interval = sdl_key_repeat.interval / (1000 / emulator.speed * 2);
				if (interval < 2) interval = 2;	/* See note above about this */
				SDL_PushEvent(&repeatevent);
			}
		}
	} else if (funcid == KRM_FUNC_REPEAT) {
		if (last_eventid != eventid)
			key_repeat_manager(KRM_FUNC_RELEASE, NULL, eventid);
		repeatevent = *event;
	}
}

/***************************************************************************
 * Keyboard Buffer Reset                                                   *
 ***************************************************************************/
/* This unpresses any pressed controls within the keyboard buffer except
 * SHIFT. It's used when changing the states of program components to make
 * sure that any controls that the user still has pressed don't remain
 * unreleased which can happen if the controls are component specific */

void keyboard_buffer_reset(void) {
	int scancode;
	
	for (scancode = 0; scancode < MAX_SCANCODES; scancode++) {
		if (scancode != SCANCODE_LEFTSHIFT) keyboard_buffer[scancode] = KEY_NOTPRESSED;
	}
}

/***************************************************************************
 * Keysym to Scancode                                                      *
 ***************************************************************************/
/* Translates SDL keysyms to SVGAlib scancodes and the reverse */

int keysym_to_scancode(int reverse, int value) {
	
	if (!reverse) {
		switch (value) {
			case SDLK_ESCAPE: return SCANCODE_ESCAPE;
			case SDLK_MINUS: return SCANCODE_MINUS;
			case SDLK_EQUALS: return SCANCODE_EQUAL;
			case SDLK_LEFTBRACKET: return SCANCODE_BRACKET_LEFT;
			case SDLK_RIGHTBRACKET: return SCANCODE_BRACKET_RIGHT;
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
			case SDLK_PAGEUP: return SCANCODE_PAGEUP;
			case SDLK_END: return SCANCODE_END;
			case SDLK_PAGEDOWN: return SCANCODE_PAGEDOWN;
			case SDLK_INSERT: return SCANCODE_INSERT;
			case SDLK_DELETE: return SCANCODE_REMOVE;
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
			case SCANCODE_MINUS: return SDLK_MINUS;
			case SCANCODE_EQUAL: return SDLK_EQUALS;
			case SCANCODE_BRACKET_LEFT: return SDLK_LEFTBRACKET;
			case SCANCODE_BRACKET_RIGHT: return SDLK_RIGHTBRACKET;
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
			case SCANCODE_PAGEUP: return SDLK_PAGEUP;
			case SCANCODE_END: return SDLK_END;
			case SCANCODE_PAGEDOWN: return SDLK_PAGEDOWN;
			case SCANCODE_INSERT: return SDLK_INSERT;
			case SCANCODE_REMOVE: return SDLK_DELETE;
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
 * Keysym to Keycode                                                       *
 ***************************************************************************/
/* This converts a symbolic key name into its equivalent keycode.
 * Its primary use is for reading configuration items from the rcfile and it
 * handles both DEVICE_CURSOR and SDL keysyms.
 * 
 * On exit: returns UNDEFINED if keysym not found */

int keysym_to_keycode(char *keysym) {
	int count, keycode, found = FALSE;
	
	if (strlen(keysym) > 0) {	/* Could be a NULL string */
		/* Firstly check that it isn't already a keycode within a string */
		if (sscanf(keysym, "%i", &keycode) == 1) return keycode;

		/* Now look for a match within keysyms[] */
		for (count = 0; count < MAX_KEYSYMS; count++) {
			if (strcmp(keysyms[count], keysym) == 0) {
				found = TRUE;
				break;
			}
		}
		if (found) return keycodes[count];
	}
	
	return UNDEFINED;
}

/***************************************************************************
 * Keycode to Keysym                                                       *
 ***************************************************************************/
/* This converts a keycode into its equivalent symbolic key name.
 * Its primary use is for writing configuration items to the rcfile and it
 * handles both DEVICE_CURSOR and SDL keycodes.
 * 
 * On exit: returns a pointer to "" if keycode not found */

char *keycode_to_keysym(int keycode) {
	int count, found = FALSE;
	static char nullstring[1] = "";

	/* Look for a match within keycodes[] */
	for (count = 0; count < MAX_KEYSYMS; count++) {
		if (keycodes[count] == keycode) {
			found = TRUE;
			break;
		}
	}
	if (found) return keysyms[count];

	return nullstring;
}

/***************************************************************************
 * Adjust Colour Component                                                 *
 ***************************************************************************/
/* This will add or subtract an amount within the bounds of any one of the
 * red, green and blue colour components of an RGB value. If required it will
 * also granulate the result by rounding in a suitable direction */

Uint32 adjust_colour_component(Uint32 rgb, Uint32 mask, int amount, int granulate) {
	Uint32 red = rgb >> 16 & 0xff, green = rgb >> 8 & 0xff, blue = rgb & 0xff;
	Uint32 *comp, count;
	
	comp = &red;
	if (mask == 0x00ff00) comp = &green;
	if (mask == 0x0000ff) comp = &blue;

	for (count = 0; count < 2; count++) {
		if (amount >= 0) {
			if (*comp <= 0xff - amount) {
				*comp += amount;
			} else {
				*comp = 0xff;
			}
		} else if (amount < 0) {
			if (*comp >= abs(amount)) {
				*comp -= abs(amount);
			} else {
				*comp = 0;
			}
		}
		if (*comp % abs(amount) && count < 1 && granulate) {
			if (*comp % abs(amount) > abs(amount) / 2) {
				amount = abs(amount) - *comp % abs(amount);
			} else {
				amount = (*comp % abs(amount)) * -1;
			}
		} else {
			break;
		}
	}

	return red << 16 | green << 8 | blue;	
}





