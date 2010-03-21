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

/* Defines */
#define MAX_CTRL_REMAPS 96
#define MAX_JOY_AXES 20
#define MAX_SCANCODES 128

#define DEVICE_KEYBOARD 1
#define DEVICE_JOYSTICK 2
#define DEVICE_CURSOR 3

#define JOYSTICK_DEAD_ZONE 50

#define KEY_REPEAT_DELAY 400		/* Default granularity of 20ms */
#define KEY_REPEAT_INTERVAL 80		/* Default granularity of 20ms */

#define CTRL_REMAPPER_INTERVAL 520	/* Default granularity of 40ms */

/* Key repeat manager function IDs */
#define KRM_FUNC_RELEASE 0
#define KRM_FUNC_REPEAT 1
#define KRM_FUNC_TICK 2

/* Variables */
char keyboard_buffer[MAX_SCANCODES];
SDL_Joystick *joystick;
int joystick_dead_zone;

struct ctrlremap {
	int components;		/* An OR'd combination of COMP_ ids */
	int protected;		/* TRUE to prevent this from being runtime modified */
	int device;			/* The source device e.g. DEVICE_JOYSTICK */
	int id;				/* The source control id */
	int remap_device;	/* The destination device e.g. DEVICE_KEYBOARD */
	int remap_id;		/* The main destination control id */
	int remap_mod_id;	/* An additional modifier destination control id */
};
struct ctrlremap ctrl_remaps[MAX_CTRL_REMAPS];

struct {
	int state;
	int master_interval;
	int interval;
} ctrl_remapper;

/* Function prototypes */
void key_repeat_manager(int funcid, SDL_Event *event, int eventid);
void keyboard_buffer_reset(void);
int keysym_to_scancode(int reverse, int value);
int keysym_to_keycode(char *keysym);
char *keycode_to_keysym(int keycode);



