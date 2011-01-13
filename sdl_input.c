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

/* Transit states */
#define TRANSIT_OUT 0
#define TRANSIT_IN 1
#define TRANSIT_SAVE 2

/* Variables */
int device, id, mod_id, state;
int last_runopts_comp = COMP_RUNOPTS0;
SDL_Event event, virtualevent;
struct MSG_Box msg_box;
int runopts_joy_cfg_id[12];

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
void manage_cursor_input(void);
void manage_all_input(void);
void manage_vkeyb_input(void);
void manage_runopts_input(void);
void manage_ldfile_input(void);
void runopts_transit(int state);


/***************************************************************************
 * Keyboard Initialise                                                     *
 ***************************************************************************/
/* This initialises the keyboard buffer, opens joystick 0 if available
 * and sets up the control remappings. It's only called once */

void sdl_keyboard_init(void) {
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
		#ifdef SDL_DEBUG_JOYSTICK
			printf("%s: SDL_JoystickNumAxes=%i SDL_JoystickNumButtons=%i\n",
				__func__, SDL_JoystickNumAxes(joystick),
				SDL_JoystickNumButtons(joystick));
		#endif
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

	/* Active within emulator/vkeyb/ctb */
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

	/* Active within the load selector only */
	ctrl_remaps[++index].components = COMP_LOAD;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_PAGEUP;
	ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
	ctrl_remaps[index].remap_id = SDLK_q;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	ctrl_remaps[++index].components = COMP_LOAD;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_PAGEDOWN;
	ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
	ctrl_remaps[index].remap_id = SDLK_a;
	ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

	/* Active within ldfile/runopts */
	ctrl_remaps[++index].components = COMP_LDFILE | COMP_RUNOPTS_ALL;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_LEFT;
	ctrl_remaps[index].remap_device = DEVICE_CURSOR;
	ctrl_remaps[index].remap_id = CURSOR_W;

	ctrl_remaps[++index].components = COMP_LDFILE | COMP_RUNOPTS_ALL;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_RIGHT;
	ctrl_remaps[index].remap_device = DEVICE_CURSOR;
	ctrl_remaps[index].remap_id = CURSOR_E;

	ctrl_remaps[++index].components = COMP_LDFILE | COMP_RUNOPTS_ALL;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_RETURN;
	ctrl_remaps[index].remap_device = DEVICE_CURSOR;
	ctrl_remaps[index].remap_id = CURSOR_HIT;

	/* Active within runopts only */
	ctrl_remaps[++index].components = COMP_RUNOPTS_ALL;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_UP;
	ctrl_remaps[index].remap_device = DEVICE_CURSOR;
	ctrl_remaps[index].remap_id = CURSOR_N;

	ctrl_remaps[++index].components = COMP_RUNOPTS_ALL;
	ctrl_remaps[index].protected = TRUE;
	ctrl_remaps[index].device = DEVICE_KEYBOARD;
	ctrl_remaps[index].id = SDLK_DOWN;
	ctrl_remaps[index].remap_device = DEVICE_CURSOR;
	ctrl_remaps[index].remap_id = CURSOR_S;

	/* Joystick to some other device.
	 * Platform specific joysticks can be hardcoded with some defaults
	 * here but otherwise their configurations should be entirely read
	 * in from the rcfile since their button/axis layouts are unknown */
	if (joystick) {
		#if defined(PLATFORM_GP2X)
			/* Universally active */
			ctrl_remaps[++index].components = COMP_ALL;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_START;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_F1;

			/* Active within emulator only */
			ctrl_remaps[++index].components = COMP_EMU;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_SELECT;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_ESCAPE;

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
			ctrl_remaps[index].id = GP2X_LTRIG;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_q;
			ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

			ctrl_remaps[++index].components = COMP_LOAD;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_RTRIG;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_a;
			ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

			/* Active within ldfile only */
			ctrl_remaps[++index].components = COMP_LDFILE;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_N;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_UP;

			ctrl_remaps[++index].components = COMP_LDFILE;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_S;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_DOWN;

			/* Active within load/ldfile/vkeyb/ctb/runopts */
			ctrl_remaps[++index].components = COMP_VKEYB | COMP_RUNOPTS_ALL;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_SELECT;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_REMAP;

			ctrl_remaps[++index].components = COMP_VKEYB | COMP_RUNOPTS_ALL;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_N;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_N;

			ctrl_remaps[++index].components = COMP_VKEYB | COMP_RUNOPTS_ALL;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_S;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_S;

			ctrl_remaps[++index].components = COMP_LOAD | COMP_LDFILE | COMP_VKEYB | COMP_RUNOPTS_ALL;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_JOY_W;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_W;

			ctrl_remaps[++index].components = COMP_LOAD | COMP_LDFILE | COMP_VKEYB | COMP_RUNOPTS_ALL;
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

			ctrl_remaps[++index].components = COMP_LOAD | COMP_VKEYB;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_X;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_SPACE;

			ctrl_remaps[++index].components = COMP_LOAD | COMP_LDFILE | COMP_VKEYB | COMP_RUNOPTS_ALL;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_BTN_A;
			ctrl_remaps[index].remap_device = DEVICE_CURSOR;
			ctrl_remaps[index].remap_id = CURSOR_HIT;

			ctrl_remaps[++index].components = COMP_LOAD | COMP_VKEYB;
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

			/* Active within ldfile/runopts */
			ctrl_remaps[++index].components = COMP_LDFILE | COMP_RUNOPTS_ALL;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_LTRIG;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_PAGEUP;

			ctrl_remaps[++index].components = COMP_LDFILE | COMP_RUNOPTS_ALL;
			ctrl_remaps[index].protected = TRUE;
			ctrl_remaps[index].device = DEVICE_JOYSTICK;
			ctrl_remaps[index].id = GP2X_RTRIG;
			ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
			ctrl_remaps[index].remap_id = SDLK_PAGEDOWN;
		#elif defined(PLATFORM_ZAURUS)
		#elif defined(__amigaos4__)
		#else
			#ifdef SDL_DEBUG_JOYSTICK
				if (strcmp(SDL_JoystickName(0),
					"Microsoft SideWinder Game Pad Pro USB version 1.0") == 0) {
					/* This is my own PC joystick which I've been using for testing */
					/* Universally active */
					ctrl_remaps[++index].components = COMP_ALL;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 5;	/* Z */
					ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
					ctrl_remaps[index].remap_id = SDLK_F1;

					/* Active within emulator */
					ctrl_remaps[++index].components = COMP_EMU;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 2;	/* C */
					ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
					ctrl_remaps[index].remap_id = SDLK_ESCAPE;

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
					ctrl_remaps[index].id = 6;	/* LTrig */
					ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
					ctrl_remaps[index].remap_id = SDLK_q;
					ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

					ctrl_remaps[++index].components = COMP_LOAD;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 7;	/* RTrig */
					ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
					ctrl_remaps[index].remap_id = SDLK_a;
					ctrl_remaps[index].remap_mod_id = SDLK_LSHIFT;

					/* Active within ldfile only */
					ctrl_remaps[++index].components = COMP_LDFILE;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 12;	/* Up */
					ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
					ctrl_remaps[index].remap_id = SDLK_UP;

					ctrl_remaps[++index].components = COMP_LDFILE;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 13;	/* Down */
					ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
					ctrl_remaps[index].remap_id = SDLK_DOWN;

					/* Active within load/ldfile/vkeyb/ctb/runopts */
					ctrl_remaps[++index].components = COMP_VKEYB | COMP_RUNOPTS_ALL;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 2;	/* C */
					ctrl_remaps[index].remap_device = DEVICE_CURSOR;
					ctrl_remaps[index].remap_id = CURSOR_REMAP;

					ctrl_remaps[++index].components = COMP_VKEYB | COMP_RUNOPTS_ALL;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 12;	/* Up */
					ctrl_remaps[index].remap_device = DEVICE_CURSOR;
					ctrl_remaps[index].remap_id = CURSOR_N;

					ctrl_remaps[++index].components = COMP_VKEYB | COMP_RUNOPTS_ALL;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 13;	/* Down */
					ctrl_remaps[index].remap_device = DEVICE_CURSOR;
					ctrl_remaps[index].remap_id = CURSOR_S;

					ctrl_remaps[++index].components = COMP_LOAD | COMP_LDFILE | COMP_VKEYB | COMP_RUNOPTS_ALL;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 10;	/* Left */
					ctrl_remaps[index].remap_device = DEVICE_CURSOR;
					ctrl_remaps[index].remap_id = CURSOR_W;

					ctrl_remaps[++index].components = COMP_LOAD | COMP_LDFILE | COMP_VKEYB | COMP_RUNOPTS_ALL;
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

					ctrl_remaps[++index].components = COMP_LOAD | COMP_VKEYB;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 0;	/* A */
					ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
					ctrl_remaps[index].remap_id = SDLK_SPACE;

					ctrl_remaps[++index].components = COMP_LOAD | COMP_LDFILE | COMP_VKEYB | COMP_RUNOPTS_ALL;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 3;	/* X */
					ctrl_remaps[index].remap_device = DEVICE_CURSOR;
					ctrl_remaps[index].remap_id = CURSOR_HIT;

					ctrl_remaps[++index].components = COMP_LOAD | COMP_VKEYB;
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

					/* Active within ldfile/runopts */
					ctrl_remaps[++index].components = COMP_LDFILE | COMP_RUNOPTS_ALL;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 6;	/* LTrig */
					ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
					ctrl_remaps[index].remap_id = SDLK_PAGEUP;

					ctrl_remaps[++index].components = COMP_LDFILE | COMP_RUNOPTS_ALL;
					ctrl_remaps[index].protected = TRUE;
					ctrl_remaps[index].device = DEVICE_JOYSTICK;
					ctrl_remaps[index].id = 7;	/* RTrig */
					ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
					ctrl_remaps[index].remap_id = SDLK_PAGEDOWN;
				}
			#endif
		#endif
	}
	#ifdef SDL_DEBUG_EVENTS
		printf("%s: ctrl_remaps index=%i\n", __func__, index);
	#endif

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
 * +-------------------------+
 * | Physical hardware input |
 * +-------------------------+
 *              |
 *              v
 * +-------------------------+
 * |  Dynamic virtual input  |
 * +-------------------------+
 *              |
 *              v
 * +-------------------------+
 * |     Target actions      |
 * +-------------------------+
 * 
 * On exit: returns non-zero if there are keyboard events else 0 */

int keyboard_update(void) {
	static int skip_update = TRUE, last_hs_pressed[2];
	static int axisstates[MAX_JOY_AXES * 2], init = TRUE;
	int eventfound = FALSE, count, found;
	int hs_vkeyb_ctb_selected;
	int hs_runopts_selected;
	int axis_end = 0;
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
					} else if (event.button.button % 128 == SDL_BUTTON_WHEELUP ||
						event.button.button % 128 == SDL_BUTTON_WHEELDOWN) {
						/* Remap mouse wheel movement to SDLK_UP & SDLK_DOWN */
						device = DEVICE_KEYBOARD;
						if (event.button.button % 128 == SDL_BUTTON_WHEELUP) {
							id = SDLK_UP;
						} else {
							id = SDLK_DOWN;
						}
						if (event.type == SDL_MOUSEBUTTONUP) {
							state = SDL_RELEASED;
						} else {
							state = SDL_PRESSED;
						}
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
					#ifdef SDL_DEBUG_JOYSTICK
						printf("%s: axis=%i value=%i\n", __func__,
							event.jaxis.axis, event.jaxis.value);
					#endif
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
					/* Simulate an F10 press which will execute the exit code */
					device = DEVICE_KEYBOARD;
					id = SDLK_F10;
					state = SDL_PRESSED;
					break;
				default:
					break;
			}

			/* If the user wants to see the input ids then record the
			 * currently pressed id for later displaying on-screen */
			if (show_input_id && device != UNDEFINED) {
				if (state == SDL_PRESSED) {
					current_input_id = id;
				} else {
					current_input_id = UNDEFINED;
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
							ctrl_remaps[count].components & get_active_component() &&
							ctrl_remaps[count].device == device && 
							ctrl_remaps[count].id == id) {
							device = ctrl_remaps[count].remap_device;
							id = ctrl_remaps[count].remap_id;
							mod_id = ctrl_remaps[count].remap_mod_id;
							break;
						}
					}
				}

				/* Manage virtual cursor device input here. This is how it works :-
				 * A real hardware event occurs which will go through the control
				 * remapper above and reach here as either a keyboard event or a
				 * cursor event. Keyboard events will just continue on down through
				 * this function, possibly getting grabbed (ooh matron) or intercepted
				 * and stored in the keyboard buffer for the emulator to pick up.
				 * Cursor events though need to be dealt with now as either they are
				 * moving a selection box around a GUI component or the user has hit
				 * a hotspot which will result in the hit being remapped again to a
				 * mouse button */

				/* Manage DEVICE_CURSOR input */
				manage_cursor_input();

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

				/* Manage COMP_RUNOPTSx input */
				if (get_active_component() & COMP_RUNOPTS_ALL)
					manage_runopts_input();
				
				/* Manage COMP_VKEYB and COMP_CTB input */
				if (get_active_component() == COMP_VKEYB ||
					get_active_component() == COMP_CTB) manage_vkeyb_input();

				/* Manage COMP_LDFILE input */
				if (get_active_component() & COMP_LDFILE)
					manage_ldfile_input();

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
			
				/* Record the event within the keyboard buffer */
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
				
			} else if (ctrl_remapper.state) {
				/* A new joystick control is currently being remapped */
				if (device != UNDEFINED) {
					if (state == SDL_PRESSED) {
						ctrl_remapper.state = FALSE;
						if (vkeyb.state) {
							found = FALSE;
							if (device == DEVICE_JOYSTICK) {
								/* Locate currently selected hotspot for group VKEYB + CTB */
								if ((hs_vkeyb_ctb_selected = get_selected_hotspot(HS_GRP_VKEYB)) == MAX_HOTSPOTS)
									hs_vkeyb_ctb_selected = get_selected_hotspot(HS_GRP_CTB);
								for (count = 0; count < MAX_CTRL_REMAPS; count++) {
									if (ctrl_remaps[count].components & COMP_VKEYB &&
										ctrl_remaps[count].device == device && 
										ctrl_remaps[count].id == id &&
										ctrl_remaps[count].remap_device == DEVICE_CURSOR &&
										ctrl_remaps[count].remap_id == CURSOR_REMAP) {
										/* The user cancelled with CURSOR_REMAP */
										break;
									} else if (ctrl_remaps[count].components & COMP_EMU &&
										ctrl_remaps[count].device == device && 
										ctrl_remaps[count].id == id) {
										/* Update existing if not protected */
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
							/* Notify the user of the outcome */
							strcpy(msg_box.title, "Remap");
							if (found) {
								strcpy(msg_box.text, "Accepted");
							} else {
								strcpy(msg_box.text, "Cancelled");
							}
							msg_box.timeout = MSG_BOX_TIMEOUT_CONTROL_REMAPPER;
							message_box_manager(MSG_BOX_SHOW, &msg_box);
						} else if (runtime_options[3].state) {
							/* Locate currently selected hotspot for group RUNOPTS3 */
							hs_runopts_selected = get_selected_hotspot(HS_GRP_RUNOPTS3);
							if (device == DEVICE_JOYSTICK) {
								/* Store id for later use within runopts-transit when saving */
								runopts_joy_cfg_id[hs_runopts_selected - HS_RUNOPTS3_JOY_CFG_LTRIG] = id;
								/* Update the joycfg text */
								set_joy_cfg_text(JOY_CFG_TEXT_ACCEPTED);
							} else {
								/* Set id to UNDEFINED */
								runopts_joy_cfg_id[hs_runopts_selected - HS_RUNOPTS3_JOY_CFG_LTRIG] = UNDEFINED;
								/* Update the joycfg text */
								set_joy_cfg_text(JOY_CFG_TEXT_CANCELLED);
							}
						}
					}
				}
			}
		}
	}

	return eventfound;
}

/***************************************************************************
 * Manage DEVICE_CURSOR Input                                              *
 ***************************************************************************/
/* When there's a situation whereby the path splits or converges then I use
 * this intuitive system: down the left, up the right.
 * 
 * This function annoys me since it looks as though it could do with
 * automating, but it is in fact dealing with every possible route that
 * can be taken from every hotspot and the patterns have been identified
 * and the code compressed anyway. Also the arrangement of the joycfg
 * joypad buttons doesn't relate to the array indices */

void manage_cursor_input(void) {
	int hs_currently_selected = 0;
	
	if (device == DEVICE_CURSOR) {

		/* Locate currently selected hotspot for active component (there can be only one) */
		if (get_active_component() == COMP_LOAD) {
			hs_currently_selected = get_selected_hotspot(HS_GRP_LOAD);
		} else if (get_active_component() == COMP_LDFILE) {
			hs_currently_selected = get_selected_hotspot(HS_GRP_LDFILE);
		} else if (get_active_component() == COMP_VKEYB || get_active_component() == COMP_CTB) {
			if ((hs_currently_selected = get_selected_hotspot(HS_GRP_VKEYB)) == MAX_HOTSPOTS)
				hs_currently_selected = get_selected_hotspot(HS_GRP_CTB);
		} else if (get_active_component() & COMP_RUNOPTS_ALL) {
			hs_currently_selected = get_selected_hotspot(HS_GRP_RUNOPTS0 << runtime_options_which());
		}

		/* Process the events */
		if (state == SDL_PRESSED) {
			if (id == CURSOR_HIT) {
				/* Remap the virtual cursor event to a mouse button event
				 * which will hit a hotspot and generate a keyboard event.
				 * I'm adding 128 to the mouse button index to make them
				 * uniquely identifiable in keyboard_update's hotspot code */
				virtualevent.type = SDL_MOUSEBUTTONDOWN;
				virtualevent.button.button = 128 + SDL_BUTTON_LEFT;
				virtualevent.button.state = SDL_PRESSED;
				if (load_selector_state || load_file_dialog.state || vkeyb.state ||
					(runtime_options_which() < MAX_RUNTIME_OPTIONS)) {
					virtualevent.button.x = hotspots[hs_currently_selected].hit_x +
						hotspots[hs_currently_selected].hit_w / 2;
					virtualevent.button.y = hotspots[hs_currently_selected].hit_y +
						hotspots[hs_currently_selected].hit_h / 2;
					SDL_PushEvent(&virtualevent);
				}
			} else if (id == CURSOR_REMAP) {
				/* Initiate joystick control remapping if a joystick is present */
				if (joystick) {
					if (vkeyb.state) {
						ctrl_remapper.state = TRUE;
					} else if (runtime_options[3].state) {
						if (hs_currently_selected >= HS_RUNOPTS3_JOY_CFG_LTRIG &&
							hs_currently_selected <= HS_RUNOPTS3_JOY_CFG_X) {
							/* Activate the control remapper and joycfg */
							ctrl_remapper.state = joy_cfg.state = TRUE;
							/* Update the joycfg text */
							set_joy_cfg_text(JOY_CFG_TEXT_PRESS_SOMETHING);
						}
					}
				}
			} else if (id == CURSOR_N) {
				/* Move the selector up */
				if (vkeyb.state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_VKEYB * CURSOR_N);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected >= HS_CTB_EXIT && 
						hs_currently_selected <= HS_CTB_ALPHA_UP) {
						hotspots[HS_VKEYB_SHIFT + hs_currently_selected -
							HS_CTB_EXIT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_CTB_LDFILE) {
						hotspots[HS_VKEYB_SHIFT + 8].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_CTB_RUNOPTS) {
						hotspots[HS_VKEYB_SHIFT + 9].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected >= HS_VKEYB_1 &&
						hs_currently_selected <= HS_VKEYB_1 + 6) {
						hotspots[HS_CTB_EXIT + hs_currently_selected -
							HS_VKEYB_1].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_VKEYB_1 + 7) {
						hotspots[HS_VKEYB_SHIFT + hs_currently_selected -
							HS_VKEYB_1].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_VKEYB_1 + 8) {
						hotspots[HS_CTB_LDFILE].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_VKEYB_1 + 9) {
						hotspots[HS_CTB_RUNOPTS].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected >= HS_VKEYB_Q) {
						hotspots[hs_currently_selected - 10].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[0].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS0 * CURSOR_N);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS0_ZX80 ||
						hs_currently_selected == HS_RUNOPTS0_ZX81) {
						hotspots[hs_currently_selected + 8].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS0_NEXT) {
						hotspots[hs_currently_selected - 3].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected - 2].flags |= HS_PROP_SELECTED;
					}
					#ifndef ENABLE_EMULATION_SPEED_ADJUST
						hs_currently_selected = get_selected_hotspot(HS_GRP_RUNOPTS0 << runtime_options_which());
						if (hs_currently_selected == HS_RUNOPTS0_SPEED_DN ||
							hs_currently_selected == HS_RUNOPTS0_SPEED_UP) {
							hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
							hotspots[hs_currently_selected - 2].flags |= HS_PROP_SELECTED;
						}
					#endif
				} else if (runtime_options[1].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS1 * CURSOR_N);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS1_DEVICE_NONE) {
						hotspots[HS_RUNOPTS1_SAVE].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected >= HS_RUNOPTS1_BACK &&
						hs_currently_selected <= HS_RUNOPTS1_NEXT) {
						hotspots[HS_RUNOPTS1_DEVICE_VSYNC].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected - 1].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[2].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * CURSOR_N);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS2_VOLUME_DN) {
						hotspots[HS_RUNOPTS2_SAVE].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS2_VOLUME_UP) {
						hotspots[HS_RUNOPTS2_NEXT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected >= HS_RUNOPTS2_KRDELAY_DN &&
						hs_currently_selected <= HS_RUNOPTS2_FGC_R_DN) {
						hotspots[hs_currently_selected - 2].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected >= HS_RUNOPTS2_FGC_R_UP &&
						hs_currently_selected <= HS_RUNOPTS2_BGC_R_DN) {
						hotspots[hs_currently_selected - 3].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected >= HS_RUNOPTS2_BGC_R_UP &&
						hs_currently_selected <= HS_RUNOPTS2_NEXT) {
						hotspots[hs_currently_selected - 4].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[3].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS3 * CURSOR_N);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS3_JDEADZ_DN) {
						hotspots[HS_RUNOPTS3_BACK].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JDEADZ_UP) {
						hotspots[HS_RUNOPTS3_EXIT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_BACK) {
						hotspots[HS_RUNOPTS3_JOY_CFG_DOWN].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_SAVE) {
						hotspots[HS_RUNOPTS3_JOY_CFG_START].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_EXIT) {
						hotspots[HS_RUNOPTS3_JOY_CFG_X].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_DOWN ||
						hs_currently_selected == HS_RUNOPTS3_JOY_CFG_SELECT) {
						hotspots[HS_RUNOPTS3_JOY_CFG_RIGHT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_START) {
						hotspots[HS_RUNOPTS3_JOY_CFG_A].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_X) {
						hotspots[HS_RUNOPTS3_JOY_CFG_B].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_LEFT ||
						hs_currently_selected == HS_RUNOPTS3_JOY_CFG_RIGHT) {
						hotspots[HS_RUNOPTS3_JOY_CFG_UP].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_A ||
						hs_currently_selected == HS_RUNOPTS3_JOY_CFG_B) {
						hotspots[HS_RUNOPTS3_JOY_CFG_Y].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_UP) {
						hotspots[HS_RUNOPTS3_JOY_CFG_LTRIG].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_Y) {
						hotspots[HS_RUNOPTS3_JOY_CFG_RTRIG].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_LTRIG) {
						hotspots[HS_RUNOPTS3_JDEADZ_DN].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_RTRIG) {
						hotspots[HS_RUNOPTS3_JDEADZ_UP].flags |= HS_PROP_SELECTED;
					}
				}
			} else if (id == CURSOR_S) {
				/* Move the selector down */
				if (vkeyb.state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_VKEYB * CURSOR_S);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected >= HS_VKEYB_SHIFT &&
						hs_currently_selected <= HS_VKEYB_SHIFT + 6) {
						hotspots[HS_CTB_EXIT + hs_currently_selected -
							HS_VKEYB_SHIFT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_VKEYB_SHIFT + 7) {
						hotspots[HS_VKEYB_1 + hs_currently_selected -
							HS_VKEYB_SHIFT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_VKEYB_SHIFT + 8) {
						hotspots[HS_CTB_LDFILE].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_VKEYB_SHIFT + 9) {
						hotspots[HS_CTB_RUNOPTS].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected >= HS_CTB_EXIT && 
						hs_currently_selected <= HS_CTB_ALPHA_UP) {
						hotspots[HS_VKEYB_1 + hs_currently_selected -
							HS_CTB_EXIT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_CTB_LDFILE) {
						hotspots[HS_VKEYB_1 + 8].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_CTB_RUNOPTS) {
						hotspots[HS_VKEYB_1 + 9].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected <= HS_VKEYB_A + 9) {
						hotspots[hs_currently_selected + 10].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[0].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS0 * CURSOR_S);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS0_SAVE) {
						hotspots[HS_RUNOPTS0_ZX80].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS0_EXIT ||
						hs_currently_selected == HS_RUNOPTS0_NEXT) {
						hotspots[HS_RUNOPTS0_ZX81].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected + 2].flags |= HS_PROP_SELECTED;
					}
					#ifndef ENABLE_EMULATION_SPEED_ADJUST
						hs_currently_selected = get_selected_hotspot(HS_GRP_RUNOPTS0 << runtime_options_which());
						if (hs_currently_selected == HS_RUNOPTS0_SPEED_DN ||
							hs_currently_selected == HS_RUNOPTS0_SPEED_UP) {
							hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
							hotspots[hs_currently_selected + 2].flags |= HS_PROP_SELECTED;
						}
					#endif
				} else if (runtime_options[1].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS1 * CURSOR_S);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected >= HS_RUNOPTS1_BACK &&
						hs_currently_selected <= HS_RUNOPTS1_NEXT) {
						hotspots[HS_RUNOPTS1_DEVICE_NONE].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS1_DEVICE_VSYNC) {
						hotspots[HS_RUNOPTS1_SAVE].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected + 1].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[2].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * CURSOR_S);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS2_BACK ||
						hs_currently_selected == HS_RUNOPTS2_SAVE) {
						hotspots[HS_RUNOPTS2_VOLUME_DN].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS2_EXIT ||
						hs_currently_selected == HS_RUNOPTS2_NEXT) {
						hotspots[HS_RUNOPTS2_VOLUME_UP].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected >= HS_RUNOPTS2_VOLUME_DN && 
						hs_currently_selected <= HS_RUNOPTS2_KRINTERVAL_DN) {
						hotspots[hs_currently_selected + 2].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS2_KRINTERVAL_UP) {
						hotspots[hs_currently_selected + 3].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected >= HS_RUNOPTS2_FGC_R_DN && 
						hs_currently_selected <= HS_RUNOPTS2_BGC_B_UP) {
						hotspots[hs_currently_selected + 4].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[3].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS3 * CURSOR_S);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS3_BACK ||
						hs_currently_selected == HS_RUNOPTS3_SAVE) {
						hotspots[HS_RUNOPTS3_JDEADZ_DN].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_EXIT) {
						hotspots[HS_RUNOPTS3_JDEADZ_UP].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JDEADZ_DN) {
						hotspots[HS_RUNOPTS3_JOY_CFG_LTRIG].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JDEADZ_UP) {
						hotspots[HS_RUNOPTS3_JOY_CFG_RTRIG].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_LTRIG) {
						hotspots[HS_RUNOPTS3_JOY_CFG_UP].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_RTRIG) {
						hotspots[HS_RUNOPTS3_JOY_CFG_Y].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_UP) {
						hotspots[HS_RUNOPTS3_JOY_CFG_LEFT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_Y) {
						hotspots[HS_RUNOPTS3_JOY_CFG_A].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_LEFT ||
						hs_currently_selected == HS_RUNOPTS3_JOY_CFG_RIGHT) {
						hotspots[HS_RUNOPTS3_JOY_CFG_DOWN].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_A ||
						hs_currently_selected == HS_RUNOPTS3_JOY_CFG_B) {
						hotspots[HS_RUNOPTS3_JOY_CFG_X].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_DOWN) {
						hotspots[HS_RUNOPTS3_BACK].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_SELECT ||
						hs_currently_selected == HS_RUNOPTS3_JOY_CFG_START) {
						hotspots[HS_RUNOPTS3_SAVE].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_X) {
						hotspots[HS_RUNOPTS3_EXIT].flags |= HS_PROP_SELECTED;
					}
				}
			} else if (id == CURSOR_W) {
				/* Move the selector left */
				if (load_selector_state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_LOAD * CURSOR_W);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (--hs_currently_selected < HS_LOAD_Q) hs_currently_selected = HS_LOAD_SPACE;
					hotspots[hs_currently_selected].flags |= HS_PROP_SELECTED;
				} else if (load_file_dialog.state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_LDFILE * CURSOR_W);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (--hs_currently_selected < HS_LDFILE_LOAD) hs_currently_selected = HS_LDFILE_EXIT;
					hotspots[hs_currently_selected].flags |= HS_PROP_SELECTED;
				} else if (vkeyb.state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_VKEYB * CURSOR_W);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hotspots[hs_currently_selected].gid == HS_GRP_CTB) {
						if (--hs_currently_selected < HS_CTB_EXIT)
							hs_currently_selected = HS_CTB_RUNOPTS;
						hotspots[hs_currently_selected].flags |= HS_PROP_SELECTED;
					} else {
						if (hs_currently_selected == HS_VKEYB_1 ||
							hs_currently_selected == HS_VKEYB_Q ||
							hs_currently_selected == HS_VKEYB_A ||
							hs_currently_selected == HS_VKEYB_SHIFT) {
							hotspots[hs_currently_selected + 9].flags |= HS_PROP_SELECTED;
						} else {
							hotspots[--hs_currently_selected].flags |= HS_PROP_SELECTED;
						}
					}
				} else if (runtime_options[0].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS0 * CURSOR_W);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS0_ZX80 ||
						hs_currently_selected == HS_RUNOPTS0_RAM_DN ||
						hs_currently_selected == HS_RUNOPTS0_FRAMESKIP_DN ||
						hs_currently_selected == HS_RUNOPTS0_SPEED_DN) {
						hotspots[hs_currently_selected + 1].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS0_SAVE) {
						hotspots[hs_currently_selected + 2].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected - 1].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[1].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS1 * CURSOR_W);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected < HS_RUNOPTS1_BACK) {
						hotspots[hs_currently_selected].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS1_BACK) {
						hotspots[hs_currently_selected + 3].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected - 1].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[2].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * CURSOR_W);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS2_VOLUME_DN || 
						hs_currently_selected == HS_RUNOPTS2_KRDELAY_DN ||
						hs_currently_selected == HS_RUNOPTS2_KRINTERVAL_DN) {
						hotspots[hs_currently_selected + 1].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS2_FGC_R_DN || 
						hs_currently_selected == HS_RUNOPTS2_FGC_G_DN ||
						hs_currently_selected == HS_RUNOPTS2_FGC_B_DN ||
						hs_currently_selected == HS_RUNOPTS2_BACK) {
						hotspots[hs_currently_selected + 3].flags |= HS_PROP_SELECTED;
					} else { 
						hotspots[hs_currently_selected - 1].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[3].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS3 * CURSOR_W);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS3_JDEADZ_DN) {
						hotspots[HS_RUNOPTS3_JDEADZ_UP].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_LTRIG) {
						hotspots[HS_RUNOPTS3_JOY_CFG_RTRIG].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_UP ||
						hs_currently_selected == HS_RUNOPTS3_JOY_CFG_DOWN) {
						hotspots[HS_RUNOPTS3_JOY_CFG_LEFT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_Y ||
						hs_currently_selected == HS_RUNOPTS3_JOY_CFG_X) {
						hotspots[HS_RUNOPTS3_JOY_CFG_A].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_LEFT) {
						hotspots[HS_RUNOPTS3_JOY_CFG_B].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_RIGHT) {
						hotspots[HS_RUNOPTS3_JOY_CFG_DOWN].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_SELECT) {
						hotspots[HS_RUNOPTS3_JOY_CFG_RIGHT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_START) {
						hotspots[HS_RUNOPTS3_JOY_CFG_SELECT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_A) {
						hotspots[HS_RUNOPTS3_JOY_CFG_START].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_B) {
						hotspots[HS_RUNOPTS3_JOY_CFG_X].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_BACK) {
						hotspots[HS_RUNOPTS3_EXIT].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected - 1].flags |= HS_PROP_SELECTED;
					}
				}
			} else if (id == CURSOR_E) {
				/* Move the selector right */
				if (load_selector_state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_LOAD * CURSOR_E);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (++hs_currently_selected > HS_LOAD_SPACE) hs_currently_selected = HS_LOAD_Q;
					hotspots[hs_currently_selected].flags |= HS_PROP_SELECTED;
				} else if (load_file_dialog.state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_LDFILE * CURSOR_E);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (++hs_currently_selected > HS_LDFILE_EXIT) hs_currently_selected = HS_LDFILE_LOAD;
					hotspots[hs_currently_selected].flags |= HS_PROP_SELECTED;
				} else if (vkeyb.state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_VKEYB * CURSOR_E);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hotspots[hs_currently_selected].gid == HS_GRP_CTB) {
						if (++hs_currently_selected > HS_CTB_RUNOPTS)
							hs_currently_selected = HS_CTB_EXIT;
						hotspots[hs_currently_selected].flags |= HS_PROP_SELECTED;
					} else {
						if (hs_currently_selected == HS_VKEYB_1 + 9 ||
							hs_currently_selected == HS_VKEYB_Q + 9 ||
							hs_currently_selected == HS_VKEYB_A + 9 ||
							hs_currently_selected == HS_VKEYB_SHIFT + 9) {
							hotspots[hs_currently_selected - 9].flags |= HS_PROP_SELECTED;
						} else {
							hotspots[hs_currently_selected + 1].flags |= HS_PROP_SELECTED;
						}
					}
				} else if (runtime_options[0].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS0 * CURSOR_E);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS0_ZX81 ||
						hs_currently_selected == HS_RUNOPTS0_RAM_UP ||
						hs_currently_selected == HS_RUNOPTS0_FRAMESKIP_UP ||
						hs_currently_selected == HS_RUNOPTS0_SPEED_UP) {
						hotspots[hs_currently_selected - 1].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS0_NEXT) {
						hotspots[hs_currently_selected - 2].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected + 1].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[1].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS1 * CURSOR_E);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected < HS_RUNOPTS1_BACK) {
						hotspots[hs_currently_selected].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS1_NEXT) {
						hotspots[hs_currently_selected - 3].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected + 1].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[2].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * CURSOR_E);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS2_VOLUME_UP || 
						hs_currently_selected == HS_RUNOPTS2_KRDELAY_UP ||
						hs_currently_selected == HS_RUNOPTS2_KRINTERVAL_UP) {
						hotspots[hs_currently_selected - 1].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS2_BGC_R_UP || 
						hs_currently_selected == HS_RUNOPTS2_BGC_G_UP ||
						hs_currently_selected == HS_RUNOPTS2_BGC_B_UP) {
						hotspots[hs_currently_selected - 3].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS2_NEXT) {
						hotspots[hs_currently_selected - 3].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected + 1].flags |= HS_PROP_SELECTED;
					}
				} else if (runtime_options[3].state) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS3 * CURSOR_E);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					if (hs_currently_selected == HS_RUNOPTS3_JDEADZ_UP) {
						hotspots[HS_RUNOPTS3_JDEADZ_DN].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_RTRIG) {
						hotspots[HS_RUNOPTS3_JOY_CFG_LTRIG].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_UP ||
						hs_currently_selected == HS_RUNOPTS3_JOY_CFG_DOWN) {
						hotspots[HS_RUNOPTS3_JOY_CFG_RIGHT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_Y ||
						hs_currently_selected == HS_RUNOPTS3_JOY_CFG_X) {
						hotspots[HS_RUNOPTS3_JOY_CFG_B].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_LEFT) {
						hotspots[HS_RUNOPTS3_JOY_CFG_UP].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_RIGHT) {
						hotspots[HS_RUNOPTS3_JOY_CFG_SELECT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_SELECT) {
						hotspots[HS_RUNOPTS3_JOY_CFG_START].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_START) {
						hotspots[HS_RUNOPTS3_JOY_CFG_A].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_A) {
						hotspots[HS_RUNOPTS3_JOY_CFG_Y].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_JOY_CFG_B) {
						hotspots[HS_RUNOPTS3_JOY_CFG_LEFT].flags |= HS_PROP_SELECTED;
					} else if (hs_currently_selected == HS_RUNOPTS3_EXIT) {
						hotspots[HS_RUNOPTS3_BACK].flags |= HS_PROP_SELECTED;
					} else {
						hotspots[hs_currently_selected + 1].flags |= HS_PROP_SELECTED;
					}
				}
			}
			/* Update the joycfg text if the cursor was moved there */
			if (id == CURSOR_N || id == CURSOR_S || id == CURSOR_W || id == CURSOR_E)
				set_joy_cfg_text(JOY_CFG_TEXT_DEFAULT_SETTINGS);
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
		device = UNDEFINED;	/* Erase it */
	}
}

/***************************************************************************
 * Manage All Input                                                        *
 ***************************************************************************/

void manage_all_input(void) {
	static int last_vkeyb_state = FALSE;
	int count;

	/* Note that I'm currently ignoring modifier states */
	if (device == DEVICE_KEYBOARD) {
		if (id == SDLK_F1) {
			/* Toggle the virtual keyboard */
			if (state == SDL_PRESSED) {
				if (!load_selector_state && !load_file_dialog.state &&
					runtime_options_which() == MAX_RUNTIME_OPTIONS) {
					vkeyb.state = !vkeyb.state;
				}
			}
		} else if (id == SDLK_F3) {
			/* Toggle the load file dialog */
			if (state == SDL_PRESSED) {
				toggle_load_file_dialog_state();
			}
		} else if (id == SDLK_F4) {
			/* Reserved for future Save State */
			if (state == SDL_PRESSED) {
			}
		} else if (id == SDLK_F5) {
			/* Reserved for future Load State */
			if (state == SDL_PRESSED) {
			}
		} else if (id == SDLK_F8) {
			/* Toggle invert screen */
			if (state == SDL_PRESSED) {
				/* The component executive monitors this variable and
				 * manages emulator and component reinitialisation */
				sdl_emulator.invert = !sdl_emulator.invert;
				rcfile.rewrite = TRUE;
				refresh_screen = 1;
			}
		} else if (id == SDLK_F9) {
			/* Simulate a control remapper press */
			if (state == SDL_PRESSED) {
				device = DEVICE_CURSOR; id = CURSOR_REMAP;
				manage_cursor_input();
			}
		} else if (id == SDLK_F10) {
			/* Exit the emulator */
			if (state == SDL_PRESSED) {
				if (runtime_options_which() < MAX_RUNTIME_OPTIONS) 
					runopts_transit(TRANSIT_OUT);	/* Restore variables */
				interrupted = INTERRUPT_PROGRAM_QUIT;
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
				if (!load_selector_state && !load_file_dialog.state &&
					runtime_options_which() == MAX_RUNTIME_OPTIONS) {
					if (!ignore_esc) {	/* ignore_esc is only used within the load selector so it can eventually go temp temp */
						reset81();
					}
				}
			}
		} else if (id == SDLK_ESCAPE) {
			/* Toggle runtime options */
			if (state == SDL_PRESSED) {
				if (!load_selector_state && !load_file_dialog.state) {
					if (runtime_options_which() == MAX_RUNTIME_OPTIONS) {
						for (count = 0; count < MAX_RUNTIME_OPTIONS; count++) {
							if (last_runopts_comp == (COMP_RUNOPTS0 << count)) {
								runtime_options[count].state = TRUE;
								break;
							}
						}
						sdl_emulator.state = FALSE;
						last_vkeyb_state = vkeyb.state;	/* Preserve vkeyb state */
						runopts_transit(TRANSIT_IN);
					} else {
						for (count = 0; count < MAX_RUNTIME_OPTIONS; count++)
							runtime_options[count].state = FALSE;
						sdl_emulator.state = TRUE;
						vkeyb.state = last_vkeyb_state;	/* Restore vkeyb state */
						runopts_transit(TRANSIT_OUT);
					}
				}
			}
		} else if (id == SDLK_MINUS || id == SDLK_EQUALS) {
			/* Adjust the volume */
			#ifdef OSS_SOUND_SUPPORT
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * id);
					if (id == SDLK_MINUS) {
						if (sdl_sound.volume > 0) {
							sdl_sound.volume -= 2;
							if (sdl_sound.state && 
								(sdl_sound.device == DEVICE_QUICKSILVA ||
								sdl_sound.device == DEVICE_ZONX)) 
								sound_ay_setvol();
						}
					} else {
						if (sdl_sound.volume < 128) {
							sdl_sound.volume += 2;
							if (sdl_sound.state && 
								(sdl_sound.device == DEVICE_QUICKSILVA ||
								sdl_sound.device == DEVICE_ZONX)) 
								sound_ay_setvol();
						}
					}
					strcpy(msg_box.title, "Sound");
					sprintf(msg_box.text, "Volume:%i", sdl_sound.volume);
					msg_box.timeout = MSG_BOX_TIMEOUT_SOUND_VOLUME;
					message_box_manager(MSG_BOX_SHOW, &msg_box);
					rcfile.rewrite = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			#endif
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
				/* The component executive monitors this variable and
				 * manages emulator and component reinitialisation */
				vkeyb.autohide = !vkeyb.autohide;
				rcfile.rewrite = TRUE;
			}
		} else if (id == SDLK_F7) {
			/* Toggle the vkeyb shift type */
			if (state == SDL_PRESSED) {
				/* The component executive monitors this variable and
				 * manages emulator and component reinitialisation */
				vkeyb.toggle_shift = !vkeyb.toggle_shift;
				rcfile.rewrite = TRUE;
			}
		} else if (id == SDLK_HOME || id == SDLK_END) {
			/* Adjust the vkeyb alpha */
			if (state == SDL_PRESSED) {
				/* The component executive monitors this variable and
				 * manages emulator and component reinitialisation */
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
				rcfile.rewrite = TRUE;
			} else {
				key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
			}
		}
	}
}

/***************************************************************************
 * Manage Runtime Options Input                                            *
 ***************************************************************************/
/* A check was done before calling this function so it is certain that one
 * of the runtime options pages is currently active */

void manage_runopts_input(void) {
	int hs_currently_selected;
	int amount, index;
	
	/* Note that I'm currently ignoring modifier states */
	if (device == DEVICE_KEYBOARD) {
		if (id == SDLK_F2) {
			/* Save */
			if (state == SDL_PRESSED) {
				runopts_transit(TRANSIT_SAVE);
			}
		} else if (id == SDLK_PAGEUP || id == SDLK_PAGEDOWN) {
			/* < Back and Next > */
			if (state == SDL_PRESSED) {
				if (id == SDLK_PAGEUP) {
					if ((index = runtime_options_which()) > 0) {
						runtime_options[index].state = FALSE;
						runtime_options[index - 1].state = TRUE;
						last_runopts_comp = get_active_component();
					}
				} else if (id == SDLK_PAGEDOWN) {
					if ((index = runtime_options_which()) < MAX_RUNTIME_OPTIONS - 1) {
						runtime_options[index].state = FALSE;
						runtime_options[index + 1].state = TRUE;
						last_runopts_comp = get_active_component();
					}
				}
			}
		} else if (id == SDLK_HOME || id == SDLK_END) {
			if (runtime_options[0].state) {
				/* Machine model */
				if (state == SDL_PRESSED) {
					if (id == SDLK_HOME) {
						runopts_emulator_model = TRUE;
					} else {
						runopts_emulator_model = FALSE;
					}
				}
			} else if (runtime_options[2].state) {
				/* Key repeat delay < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * id);
					if (id == SDLK_HOME) {
						if (sdl_key_repeat.delay > 80) sdl_key_repeat.delay -= 10;
					} else {
						if (sdl_key_repeat.delay < 520) sdl_key_repeat.delay += 10;
					}
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			} else if (runtime_options[3].state) {
				/* Joy Dead Zone < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS3 * id);
					if (id == SDLK_HOME) {
						if (joystick_dead_zone > 2) joystick_dead_zone -= 2;
					} else {
						if (joystick_dead_zone < 98) joystick_dead_zone += 2;
					}
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_INSERT || id == SDLK_DELETE) {
			if (runtime_options[0].state) {
				/* RAM Size < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS0 * id);
					if (id == SDLK_INSERT) {
						if (runopts_emulator_ramsize == 56) {
							runopts_emulator_ramsize = 48;
						} else if (runopts_emulator_ramsize >= 32 &&
							runopts_emulator_ramsize <= 48) {
							runopts_emulator_ramsize -= 16;
						} else if (runopts_emulator_ramsize == 16) {
							runopts_emulator_ramsize = 4;
						} else if (runopts_emulator_ramsize > 1 &&
							runopts_emulator_ramsize <= 4) {
							runopts_emulator_ramsize -= 1;
						}
					} else {
						if (runopts_emulator_ramsize < 4) {
							runopts_emulator_ramsize += 1;
						} else if (runopts_emulator_ramsize == 4) {
							runopts_emulator_ramsize = 16;
						} else if (runopts_emulator_ramsize >= 16 &&
							runopts_emulator_ramsize <= 32) {
							runopts_emulator_ramsize += 16;
						} else if (runopts_emulator_ramsize == 48) {
							runopts_emulator_ramsize = 56;
						}
					}
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			} else if (runtime_options[2].state) {
				/* Key repeat interval < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * id);
					if (id == SDLK_INSERT) {
						if (sdl_key_repeat.interval > 80) sdl_key_repeat.interval -= 10;
					} else {
						if (sdl_key_repeat.interval < 520) sdl_key_repeat.interval += 10;
					}
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_1 || id == SDLK_2) {
			if (runtime_options[0].state) {
				#ifdef ENABLE_EMULATION_SPEED_ADJUST
					/* Emulation Speed < and > */
					if (state == SDL_PRESSED) {
						key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS0 * id);
						if (id == SDLK_1) {
							if (runopts_emulator_speed < 40) runopts_emulator_speed += 10;
						} else {
							if (runopts_emulator_speed > 10) runopts_emulator_speed -= 10;
						}
					} else {
						key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
					}
				#endif
			} else if (runtime_options[1].state) {
				#ifdef OSS_SOUND_SUPPORT
					if (state == SDL_PRESSED) {
						if (id == SDLK_1) {
							/* Sound Device: None */
							runopts_sound_device = DEVICE_NONE;
						} else {
							/* AY Chip Based */
							if (runopts_sound_device != DEVICE_ZONX)
								runopts_sound_device = DEVICE_QUICKSILVA;
						}
					}
				#endif
			} else if (runtime_options[2].state) {
				/* Foreground colour red < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * id);
					amount = 8; if (id == SDLK_1) amount = -8;
					colours.emu_fg = adjust_colour_component(colours.emu_fg, 0xff0000, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_3 || id == SDLK_4) {
			if (runtime_options[1].state) {
				#ifdef OSS_SOUND_SUPPORT
					if (state == SDL_PRESSED) {
						if (id == SDLK_3) {
							/* Sound Device: Quicksilva Sound Board */
							runopts_sound_device = DEVICE_QUICKSILVA;
						} else {
							/* Sound Device: BI-PAK ZON X81 */
							runopts_sound_device = DEVICE_ZONX;
						}
					}
				#endif
			} else if (runtime_options[2].state) {
				/* Foreground colour green < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * id);
					amount = 8; if (id == SDLK_3) amount = -8;
					colours.emu_fg = adjust_colour_component(colours.emu_fg, 0x00ff00, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_5 || id == SDLK_6) {
			if (runtime_options[1].state) {
				#ifdef OSS_SOUND_SUPPORT
					if (state == SDL_PRESSED) {
						if (id == SDLK_5) {
							/* ACB Stereo */
							if (runopts_sound_device == DEVICE_QUICKSILVA ||
								runopts_sound_device == DEVICE_ZONX)
								runopts_sound_stereo = !runopts_sound_stereo;
						} else {
							/* Sound Device: VSYNC */
							runopts_sound_device = DEVICE_VSYNC;
						}
					}
				#endif
			} else if (runtime_options[2].state) {
				/* Foreground colour blue < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * id);
					amount = 8; if (id == SDLK_5) amount = -8;
					colours.emu_fg = adjust_colour_component(colours.emu_fg, 0x0000ff, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_7 || id == SDLK_8) {
			if (runtime_options[2].state) {
				/* Background colour red < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * id);
					amount = 8; if (id == SDLK_7) amount = -8;
					colours.emu_bg = adjust_colour_component(colours.emu_bg, 0xff0000, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_9 || id == SDLK_0) {
			if (runtime_options[2].state) {
				/* Background colour green < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * id);
					amount = 8; if (id == SDLK_9) amount = -8;
					colours.emu_bg = adjust_colour_component(colours.emu_bg, 0x00ff00, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id == SDLK_LEFTBRACKET || id == SDLK_RIGHTBRACKET) {
			if (runtime_options[0].state) {
				/* Frameskip < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS0 * id);
					if (id == SDLK_LEFTBRACKET) {
						if (sdl_emulator.frameskip > 0) sdl_emulator.frameskip--;
					} else {
						if (sdl_emulator.frameskip < MAX_FRAMESKIP) sdl_emulator.frameskip++;
					}
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			} else if (runtime_options[2].state) {
				/* Background colour blue < and > */
				if (state == SDL_PRESSED) {
					key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_RUNOPTS2 * id);
					amount = 8; if (id == SDLK_LEFTBRACKET) amount = -8;
					colours.emu_bg = adjust_colour_component(colours.emu_bg, 0x0000ff, amount, TRUE);
					video.redraw = TRUE;
				} else {
					key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
				}
			}
		} else if (id >= SDLK_a && id <= SDLK_l) {
			if (runtime_options[3].state && joystick) {
				if (state == SDL_PRESSED) {
					/* Because the control remapper is being activated with hit,
					 * the user could be hitting a hotspot with the mouse or
					 * (unlikely) pressing a to l, so the selector is moved to
					 * the hit hotspot */
					/* Locate currently selected hotspot for group RUNOPTS3 */
					hs_currently_selected = get_selected_hotspot(HS_GRP_RUNOPTS3);
					hotspots[hs_currently_selected].flags &= ~HS_PROP_SELECTED;
					hotspots[HS_RUNOPTS3_JOY_CFG_LTRIG + id - SDLK_a].flags |= HS_PROP_SELECTED;
					/* Simulate a control remapper press */
					device = DEVICE_CURSOR; id = CURSOR_REMAP;
					manage_cursor_input();
				}
			}
		}
	}
}

/***************************************************************************
 * Manage Load File Dialog Input                                           *
 ***************************************************************************/

void manage_ldfile_input(void) {
	char lastsubdir[256];
	int found = FALSE;
	int count, index;
	char *direntry;

	/* Note that I'm currently ignoring modifier states */
	if (device == DEVICE_KEYBOARD) {
		if (id == SDLK_UP || id == SDLK_PAGEUP) {
			if (state == SDL_PRESSED) {
				found = TRUE;
				key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_LDFILE * id);
				/* Page up */
				if (id == SDLK_PAGEUP) {
					if (load_file_dialog.dirlist_selected - 19 < 0) {
						load_file_dialog.dirlist_selected = 0;
					} else {
						load_file_dialog.dirlist_selected -= 19;
					}
				/* Select the previous item */
				} else {
					if (load_file_dialog.dirlist_selected - 1 < 0) {
						load_file_dialog.dirlist_selected = 0;
					} else {
						load_file_dialog.dirlist_selected -= 1;
					}
				}
			} else {
				key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
			}
		} else if (id == SDLK_DOWN || id == SDLK_PAGEDOWN) {
			if (state == SDL_PRESSED) {
				found = TRUE;
				key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_LDFILE * id);
				/* Page down */
				if (id == SDLK_PAGEDOWN) {
					if (load_file_dialog.dirlist_selected + 19 > 
						load_file_dialog.dirlist_count - 1) {
						load_file_dialog.dirlist_selected = 
						load_file_dialog.dirlist_count - 1;
					} else {
						load_file_dialog.dirlist_selected += 19;
					}
				/* Select the next item */
				} else {
					if (load_file_dialog.dirlist_selected + 1 > 
						load_file_dialog.dirlist_count - 1) {
						load_file_dialog.dirlist_selected = 
						load_file_dialog.dirlist_count - 1;
					} else {
						load_file_dialog.dirlist_selected += 1;
					}
				}
			} else {
				key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
			}
		} else if (id == SDLK_HOME) {
			/* Select topmost item */
			if (state == SDL_PRESSED) {
				found = TRUE;
				load_file_dialog.dirlist_selected = 0;
			}
		} else if (id == SDLK_END) {
			/* Select last item */
			if (state == SDL_PRESSED) {
				found = TRUE;
				load_file_dialog.dirlist_selected = load_file_dialog.dirlist_count - 1;
			}
		} else if (id >= SDLK_ROW00 && id <= SDLK_ROW19) {
			/* Update the selected item */
			if (state == SDL_PRESSED) {
				if (load_file_dialog.dirlist_top + id - SDLK_ROW00 < 
					load_file_dialog.dirlist_count)
					load_file_dialog.dirlist_selected = 
					load_file_dialog.dirlist_top + id - SDLK_ROW00;
			}
		} else if ((id >= SDLK_0 && id <= SDLK_9) || (id >= SDLK_a && id <= SDLK_z)) {
			/* Select the next item in the list that starts with id */
			if (state == SDL_PRESSED) {
				found = TRUE;
				key_repeat_manager(KRM_FUNC_REPEAT, &event, COMP_LDFILE * id);
				index = load_file_dialog.dirlist_selected;
				for (count = 0; count < load_file_dialog.dirlist_count; count++) {
					if (++index > load_file_dialog.dirlist_count - 1) index = 0;
					direntry = load_file_dialog.dirlist + index * 
						load_file_dialog.dirlist_sizeof;
					if (*direntry == '(') direntry++;
					if (*direntry == id || *direntry == toupper(id)) {
						load_file_dialog.dirlist_selected = index;
						break;
					}
				}
			} else {
				key_repeat_manager(KRM_FUNC_RELEASE, NULL, 0);
			}
		} else if (id == SDLK_F2) {
			/* Load */
			if (state == SDL_PRESSED) {
				found = TRUE;
				direntry = load_file_dialog.dirlist + 
					load_file_dialog.dirlist_selected * load_file_dialog.dirlist_sizeof;
				/* Is it a directory? */
				if (*direntry == '(') {
					*lastsubdir = 0;
					if ((strcmp(direntry, "(..)") == 0) &&
						(strcmp(file_dialog_basename(load_file_dialog.dir), "/") != 0))
						/* Record the current subdirectory for later reselection */
						sprintf(lastsubdir, "(%s)", 
							file_dialog_basename(load_file_dialog.dir));

					/* Update the directory string */
					file_dialog_cd(load_file_dialog.dir, direntry);
					/* Reinitialise the list */
					load_file_dialog_dirlist_init();

					if (*lastsubdir) {
						/* Auto-select the previously recorded subdirectory */
						for (count = 0; count < load_file_dialog.dirlist_count; count++)
							if (strcmp(lastsubdir, load_file_dialog.dirlist + 
								load_file_dialog.dirlist_sizeof * count) == 0) break;
						if (count < load_file_dialog.dirlist_count)
							load_file_dialog.dirlist_selected = count;
					}
				} else {
					/* It's a file */
					if (load_file_dialog.method == LOAD_FILE_METHOD_NONE) {

						printf("%s: Todo LOAD_FILE_METHOD_FORCEDLOAD\n", __func__);	/* temp temp */

					} else if (load_file_dialog.method == LOAD_FILE_METHOD_SELECTLOAD) {
						/* sdl_load_file is currently waiting in a loop
						 * and this response will initiate the file load */
						load_file_dialog.method = LOAD_FILE_METHOD_SELECTLOADOK;
					}
					toggle_load_file_dialog_state();
				}
			}
		}
		if (found) {
			/* Adjust the list top marker */
			if (load_file_dialog.dirlist_top > load_file_dialog.dirlist_selected) {
				load_file_dialog.dirlist_top = load_file_dialog.dirlist_selected;
			} else if (load_file_dialog.dirlist_top < 
				load_file_dialog.dirlist_selected - 19) {
				load_file_dialog.dirlist_top = load_file_dialog.dirlist_selected - 19;
			}
			/* Resize hotspots to match new text */
			hotspots_resize(HS_GRP_LDFILE);
		}
	}
}

/***************************************************************************
 * Toggle Load File Dialog State                                           *
 ***************************************************************************/

void toggle_load_file_dialog_state(void) {
	static int last_vkeyb_state = FALSE;

	if (!load_selector_state &&
		runtime_options_which() == MAX_RUNTIME_OPTIONS) {
		if (!load_file_dialog.state) {
			load_file_dialog.state = TRUE;
			sdl_emulator.state = FALSE;
			last_vkeyb_state = vkeyb.state;	/* Preserve vkeyb state */
		} else {
			load_file_dialog.state = FALSE;
			sdl_emulator.state = TRUE;
			vkeyb.state = last_vkeyb_state;	/* Restore vkeyb state */
		}
	}
}

/***************************************************************************
 * Runtime Options Is A Reset Scheduled?                                   *
 ***************************************************************************/
/* This is used to show the warning message at the bottom of selected runopts
 * pages when a reset is scheduled as a result of something being modified.
 * 
 * On exit: returns TRUE if an option change requires a reset
 *          else FALSE */

int runopts_is_a_reset_scheduled(void) {
	int retval = FALSE;

	if (runopts_emulator_model != *sdl_emulator.model || 
		runopts_emulator_ramsize != sdl_emulator.ramsize) {
		retval = TRUE;
#ifdef OSS_SOUND_SUPPORT
	} else if (runopts_sound_device != sdl_sound.device) {
		retval = TRUE;
	} else if (runopts_sound_stereo != sdl_sound.stereo &&
		runopts_sound_device != DEVICE_NONE && 
		runopts_sound_device != DEVICE_VSYNC) {
		retval = TRUE;
#endif
	}

	return retval;
}

/***************************************************************************
 * Runtime Options Transit                                                 *
 ***************************************************************************/
/* There are two systems in operation here:
 *   Real variables (not copies) that are being updated live i.e. previewed.
 *   Copies of real variables that will update the real variables on save.
 * 
 * Changing the colours actually changes the real variables live, so when the
 * user saves there is nothing to do since the real variables have already been
 * updated, but if save wasn't selected then the original values must be restored
 * on exit.
 * 
 * Changing the machine model changes a copy of the real variable and the real
 * variable will be updated from the copy on save. If the user doesn't save then
 * no further action needs to be taken; the copy is simply discarded.
 * 
 */

void runopts_transit(int state) {
	static int last_state = TRANSIT_OUT;
	static struct keyrepeat runopts_key_repeat;
	static Uint32 runopts_colours_emu_fg;
	static Uint32 runopts_colours_emu_bg;
	static int runopts_joystick_dead_zone;
	static int runopts_emulator_frameskip;
	int protected, remap_device, remap_id, remap_mod_id;
	int count, index, ctrl, found, components;
	
	if (state == TRANSIT_OUT) {
		if (last_state != TRANSIT_SAVE) {
			/* Restore the original contents of these variables */
			sdl_emulator.frameskip = runopts_emulator_frameskip;
			sdl_key_repeat.delay = runopts_key_repeat.delay;
			sdl_key_repeat.interval = runopts_key_repeat.interval;
			colours.emu_fg = runopts_colours_emu_fg;
			colours.emu_bg = runopts_colours_emu_bg;
			joystick_dead_zone = runopts_joystick_dead_zone;
		}
		joy_cfg.state = FALSE;	/* Discard what was remapped and not saved */
	} else if (state == TRANSIT_IN) {
		/* Initialise copies of the variables modifiable within runopts.
		 * Sound volume isn't included because it's always available for 
		 * adjustment throughout the program i.e. it's live */
		runopts_emulator_speed = sdl_emulator.speed;
		runopts_emulator_model = *sdl_emulator.model;
		runopts_emulator_ramsize = sdl_emulator.ramsize;
		runopts_emulator_frameskip = sdl_emulator.frameskip;
		runopts_sound_device = sdl_sound.device;
		runopts_sound_stereo = sdl_sound.stereo;
		runopts_key_repeat.delay = sdl_key_repeat.delay;
		runopts_key_repeat.interval = sdl_key_repeat.interval;
		runopts_colours_emu_fg = colours.emu_fg;
		runopts_colours_emu_bg = colours.emu_bg;
		runopts_joystick_dead_zone = joystick_dead_zone;
		for (count = 0; count < 12; count++) runopts_joy_cfg_id[count] = UNDEFINED;
	} else if (state == TRANSIT_SAVE) {
		strcpy(msg_box.title, "Options");
		strcpy(msg_box.text, "Changes saved");
		msg_box.timeout = MSG_BOX_TIMEOUT_RUNOPTS_SAVE;
		message_box_manager(MSG_BOX_SHOW, &msg_box);
		rcfile.rewrite = TRUE;
		#ifdef ENABLE_EMULATION_SPEED_ADJUST
			/* Update the emulation speed */
			if (runopts_emulator_speed != sdl_emulator.speed) {
				sdl_emulator.speed = runopts_emulator_speed;
				#ifdef OSS_SOUND_SUPPORT
					if (sdl_sound.state) sound_framesiz_init();
				#endif
			}
		#endif
		/* Update the machine model */
		if (runopts_emulator_model != *sdl_emulator.model) {
			/* The component executive monitors this variable and
			 * manages emulator and component reinitialisation */
			*sdl_emulator.model = runopts_emulator_model;
		}
		/* Update the RAM size */
		if (runopts_emulator_ramsize != sdl_emulator.ramsize) {
			/* The component executive monitors this variable and
			 * manages emulator and component reinitialisation */
			sdl_emulator.ramsize = runopts_emulator_ramsize;
		}
		#ifdef OSS_SOUND_SUPPORT
			/* Update the sound device */
			if (runopts_sound_device != sdl_sound.device) {
				/* The component executive monitors this variable and
				 * manages emulator and component reinitialisation */
				sdl_sound.device = runopts_sound_device;
			}
			/* Update the sound stereo */
			if (runopts_sound_stereo != sdl_sound.stereo) {
				/* The component executive monitors this variable and
				 * manages emulator and component reinitialisation */
				sdl_sound.stereo = runopts_sound_stereo;
			}
		#endif
		/* If the joycfg was used then update/insert the controls that
		 * were remapped. Note that this needs to be kept in sync with
		 * any joystick controls hard-coded within keyboard_update */
		if (joy_cfg.state) {
			joy_cfg.state = FALSE;
			for (ctrl = 0; ctrl < 12; ctrl++) {
				if (runopts_joy_cfg_id[ctrl] != UNDEFINED) {
					/* Firstly and importantly expunge ctrl_remaps of
					 * all entries that use the same device and id 
					 * since duplicate controls are not supported */
					for (count = 0; count < MAX_CTRL_REMAPS; count++) {
						if (ctrl_remaps[count].device == DEVICE_JOYSTICK &&
							ctrl_remaps[count].id == runopts_joy_cfg_id[ctrl]) {
							ctrl_remaps[count].device = UNDEFINED;
						}
					}
					/* Now defragment ctrl_remaps since many other functions will
					 * assume the first device=UNDEFINED found as being the end */
					do {
						index = UNDEFINED; found = FALSE;
						for (count = 0; count < MAX_CTRL_REMAPS; count++) {
							if (ctrl_remaps[count].device == UNDEFINED && index == UNDEFINED) {
								index = count;
							} else if (ctrl_remaps[count].device != UNDEFINED && index != UNDEFINED) {
								ctrl_remaps[index] = ctrl_remaps[count];
								ctrl_remaps[count].device = index = UNDEFINED;
								found = TRUE;
							}
						}
					} while (found);
					/* Continue with updating/inserting */
					for (count = 0; ; count++) {
						/* Set some common defaults */
						components = 0;
						protected = TRUE;
						remap_device = DEVICE_KEYBOARD;
						remap_id = remap_mod_id = UNDEFINED;
						if (ctrl == 0) {
							/* Set-up LTRIG joystick controls */
							if (count == 0) {
								components = COMP_EMU;
								protected = FALSE;
								remap_id = SDLK_LSHIFT;
							} else if (count == 1) {
								components = COMP_LOAD;
								remap_id = SDLK_q;
								remap_mod_id = SDLK_LSHIFT;
							} else if (count == 2) {
								components = COMP_VKEYB;
								remap_id = SDLK_LSHIFT;
							} else if (count == 3) {
								components = COMP_LDFILE | COMP_RUNOPTS_ALL;
								remap_id = SDLK_PAGEUP;
							} else {
								break;
							}
						} else if (ctrl == 1) {
							/* Set-up RTRIG joystick controls */
							if (count == 0) {
								components = COMP_LOAD;
								remap_id = SDLK_a;
								remap_mod_id = SDLK_LSHIFT;
							} else if (count == 1) {
								components = COMP_LDFILE | COMP_RUNOPTS_ALL;
								remap_id = SDLK_PAGEDOWN;
							} else {
								break;
							}
						} else if (ctrl == 2) {
							/* Set-up LEFT joystick controls */
							if (count == 0) {
								components = COMP_EMU;
								protected = FALSE;
								remap_id = SDLK_o;
							} else if (count == 1) {
								components = COMP_LOAD | COMP_LDFILE | COMP_VKEYB | COMP_RUNOPTS_ALL;
								remap_device = DEVICE_CURSOR;
								remap_id = CURSOR_W;
							} else {
								break;
							}
						} else if (ctrl == 3) {
							/* Set-up RIGHT joystick controls */
							if (count == 0) {
								components = COMP_EMU;
								protected = FALSE;
								remap_id = SDLK_p;
							} else if (count == 1) {
								components = COMP_LOAD | COMP_LDFILE | COMP_VKEYB | COMP_RUNOPTS_ALL;
								remap_device = DEVICE_CURSOR;
								remap_id = CURSOR_E;
							} else {
								break;
							}
						} else if (ctrl == 4) {
							/* Set-up UP joystick controls */
							if (count == 0) {
								components = COMP_EMU;
								protected = FALSE;
								remap_id = SDLK_q;
							} else if (count == 1) {
								components = COMP_LOAD;
								remap_id = SDLK_q;
							} else if (count == 2) {
								components = COMP_LDFILE;
								remap_id = SDLK_UP;
							} else if (count == 3) {
								components = COMP_VKEYB | COMP_RUNOPTS_ALL;
								remap_device = DEVICE_CURSOR;
								remap_id = CURSOR_N;
							} else {
								break;
							}
						} else if (ctrl == 5) {
							/* Set-up DOWN joystick controls */
							if (count == 0) {
								components = COMP_EMU;
								protected = FALSE;
								remap_id = SDLK_a;
							} else if (count == 1) {
								components = COMP_LOAD;
								remap_id = SDLK_a;
							} else if (count == 2) {
								components = COMP_LDFILE;
								remap_id = SDLK_DOWN;
							} else if (count == 3) {
								components = COMP_VKEYB | COMP_RUNOPTS_ALL;
								remap_device = DEVICE_CURSOR;
								remap_id = CURSOR_S;
							} else {
								break;
							}
						} else if (ctrl == 6) {
							/* Set-up SELECT joystick controls */
							if (count == 0) {
								components = COMP_EMU;
								remap_id = SDLK_ESCAPE;
							} else if (count == 1) {
								components = COMP_VKEYB | COMP_RUNOPTS_ALL;
								remap_device = DEVICE_CURSOR;
								remap_id = CURSOR_REMAP;
							} else {
								break;
							}
						} else if (ctrl == 7) {
							/* Set-up START joystick controls */
							if (count == 0) {
								components = COMP_ALL;
								remap_id = SDLK_F1;
							} else {
								break;
							}
						} else if (ctrl == 8) {
							/* Set-up A joystick controls */
							if (count == 0) {
								components = COMP_EMU;
								protected = FALSE;
								remap_id = SDLK_RETURN;
							} else if (count == 1) {
								components = COMP_LOAD | COMP_LDFILE | COMP_VKEYB | COMP_RUNOPTS_ALL;
								remap_device = DEVICE_CURSOR;
								remap_id = CURSOR_HIT;
							} else {
								break;
							}
						} else if (ctrl == 9) {
							/* Set-up B joystick controls */
							if (count == 0) {
								components = COMP_EMU;
								protected = FALSE;
								remap_id = SDLK_RETURN;
							} else if (count == 1) {
								components = COMP_LOAD | COMP_VKEYB;
								remap_id = SDLK_RETURN;
							} else {
								break;
							}
						} else if (ctrl == 10) {
							/* Set-up Y joystick controls */
							if (count == 0) {
								components = COMP_EMU;
								protected = FALSE;
								remap_id = SDLK_0;
								remap_mod_id = SDLK_LSHIFT;
							} else if (count == 1) {
								components = COMP_VKEYB;
								remap_id = SDLK_0;
								remap_mod_id = SDLK_LSHIFT;
							} else {
								break;
							}
						} else if (ctrl == 11) {
							/* Set-up X joystick controls */
							if (count == 0) {
								components = COMP_EMU;
								protected = FALSE;
								remap_id = SDLK_SPACE;
							} else if (count == 1) {
								components = COMP_LOAD | COMP_VKEYB;
								remap_id = SDLK_SPACE;
							} else {
								break;
							}
						}
						/* Search for an already existing control remap */
						found = FALSE;
						for (index = 0; index < MAX_CTRL_REMAPS; index++) {
							if (ctrl_remaps[index].components == components &&
								ctrl_remaps[index].device == DEVICE_JOYSTICK && 
								ctrl_remaps[index].remap_device == remap_device &&
								ctrl_remaps[index].remap_id == remap_id &&
								ctrl_remaps[index].remap_mod_id == remap_mod_id) {
								found = TRUE; break;
							} else if (ctrl_remaps[index].device == UNDEFINED) {
								break;
							}
						}
						if (found) {
							/* Update existing */
							ctrl_remaps[index].id = runopts_joy_cfg_id[ctrl];
						} else {
							/* Insert new */
							ctrl_remaps[index].components = components;
							ctrl_remaps[index].protected = protected;
							ctrl_remaps[index].device = DEVICE_JOYSTICK;
							ctrl_remaps[index].id = runopts_joy_cfg_id[ctrl];
							ctrl_remaps[index].remap_device = remap_device;
							ctrl_remaps[index].remap_id = remap_id;
							ctrl_remaps[index].remap_mod_id = remap_mod_id;
						}
					}
				}
			}
		}
	}

	last_state = state;
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
		interval = sdl_key_repeat.delay / sdl_emulator.speed / 2;
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
				interval = sdl_key_repeat.interval / sdl_emulator.speed / 2;
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
/* This unpresses any pressed controls within the keyboard buffer.
 * It's used when changing the states of program components to make sure
 * that any controls that the user still has pressed don't remain unreleased
 * which can happen if the controls are component specific.
 * 
 * Resetting SHIFT is optional and is not required under normal operation.
 * 
 * On entry: shift_reset = TRUE to additionally reset SHIFT */

void keyboard_buffer_reset(int shift_reset) {
	int scancode;
	
	for (scancode = 0; scancode < MAX_SCANCODES; scancode++) {
		if ((scancode != SCANCODE_LEFTSHIFT) ||
			(scancode == SCANCODE_LEFTSHIFT && shift_reset)) {
			keyboard_buffer[scancode] = KEY_NOTPRESSED;
		}
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
			case SDLK_ROW00: return SCANCODE_ROW00;
			case SDLK_ROW01: return SCANCODE_ROW01;
			case SDLK_ROW02: return SCANCODE_ROW02;
			case SDLK_ROW03: return SCANCODE_ROW03;
			case SDLK_ROW04: return SCANCODE_ROW04;
			case SDLK_ROW05: return SCANCODE_ROW05;
			case SDLK_ROW06: return SCANCODE_ROW06;
			case SDLK_ROW07: return SCANCODE_ROW07;
			case SDLK_ROW08: return SCANCODE_ROW08;
			case SDLK_ROW09: return SCANCODE_ROW09;
			case SDLK_ROW10: return SCANCODE_ROW10;
			case SDLK_ROW11: return SCANCODE_ROW11;
			case SDLK_ROW12: return SCANCODE_ROW12;
			case SDLK_ROW13: return SCANCODE_ROW13;
			case SDLK_ROW14: return SCANCODE_ROW14;
			case SDLK_ROW15: return SCANCODE_ROW15;
			case SDLK_ROW16: return SCANCODE_ROW16;
			case SDLK_ROW17: return SCANCODE_ROW17;
			case SDLK_ROW18: return SCANCODE_ROW18;
			case SDLK_ROW19: return SCANCODE_ROW19;
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
			case SCANCODE_ROW00: return SDLK_ROW00;
			case SCANCODE_ROW01: return SDLK_ROW01;
			case SCANCODE_ROW02: return SDLK_ROW02;
			case SCANCODE_ROW03: return SDLK_ROW03;
			case SCANCODE_ROW04: return SDLK_ROW04;
			case SCANCODE_ROW05: return SDLK_ROW05;
			case SCANCODE_ROW06: return SDLK_ROW06;
			case SCANCODE_ROW07: return SDLK_ROW07;
			case SCANCODE_ROW08: return SDLK_ROW08;
			case SCANCODE_ROW09: return SDLK_ROW09;
			case SCANCODE_ROW10: return SDLK_ROW10;
			case SCANCODE_ROW11: return SDLK_ROW11;
			case SCANCODE_ROW12: return SDLK_ROW12;
			case SCANCODE_ROW13: return SDLK_ROW13;
			case SCANCODE_ROW14: return SDLK_ROW14;
			case SCANCODE_ROW15: return SDLK_ROW15;
			case SCANCODE_ROW16: return SDLK_ROW16;
			case SCANCODE_ROW17: return SDLK_ROW17;
			case SCANCODE_ROW18: return SDLK_ROW18;
			case SCANCODE_ROW19: return SDLK_ROW19;
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





