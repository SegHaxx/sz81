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

/* Defines */
#define MAX_CTRL_REMAPS 64
#define MAX_JOY_AXES 20

#define DEVICE_KEYBOARD 1
#define DEVICE_JOYSTICK 2
#define DEVICE_CURSOR 3

#define KEY_REPEAT_DELAY 320		/* Default granularity of 40ms */
#define KEY_REPEAT_INTERVAL 120		/* Default granularity of 40ms */

#define JOYSTICK_DEAD_ZONE 75

#define CTRL_REMAPPER_INTERVAL 520	/* Default granularity of 40ms */

/* Key repeat manager function IDs */
#define KRM_FUNC_RELEASE 0
#define KRM_FUNC_REPEAT 1
#define KRM_FUNC_TICK 2

/* Transit states */
#define TRANSIT_OUT 0
#define TRANSIT_IN 1
#define TRANSIT_SAVE 2

/* Extended SDL keysyms used for hotspots+lists */
#define SDLK_ROW00 330
#define SDLK_ROW01 331
#define SDLK_ROW02 332
#define SDLK_ROW03 333
#define SDLK_ROW04 334
#define SDLK_ROW05 335
#define SDLK_ROW06 336
#define SDLK_ROW07 337
#define SDLK_ROW08 338
#define SDLK_ROW09 339
#define SDLK_ROW10 340
#define SDLK_ROW11 341
#define SDLK_ROW12 342
#define SDLK_ROW13 343
#define SDLK_ROW14 344
#define SDLK_ROW15 345
#define SDLK_ROW16 346
#define SDLK_ROW17 347
#define SDLK_ROW18 348
#define SDLK_ROW19 349
/* Extended SDL keysyms used for the mousewheel */
#define SDLK_MULTIUP 350
#define SDLK_MULTIDOWN 351
/* Extended SDL keysyms used for other things */
#define SDLK_ACCEPT 352
#define SDLK_SBUP 353
#define SDLK_SBPGUP 354
#define SDLK_SBHDLE 355
#define SDLK_SBPGDN 356
#define SDLK_SBDOWN 357

/* Variables */
extern SDL_Joystick *joystick;
extern int joystick_dead_zone;
extern int show_input_id;
extern int current_input_id;
extern int runopts_emulator_speed;
extern int runopts_emulator_model;
extern int runopts_emulator_ramsize;
extern int runopts_emulator_m1not;
extern int runopts_emulator_wrx;
extern int runopts_emulator_chrgen;
extern int runopts_sound_device;
extern int runopts_sound_stereo;
extern int runopts_sound_ay_unreal;

typedef struct ctrlremap {
	int components;		/* An OR'd combination of COMP_ IDs */
	int protected;		/* TRUE to prevent this from being runtime modified */
	int device;			/* The source device e.g. DEVICE_JOYSTICK */
	int id;				/* The source control id */
	int remap_device;	/* The destination device e.g. DEVICE_KEYBOARD */
	int remap_id;		/* The main destination control id */
	int remap_mod_id;	/* An additional modifier destination control id */
} ctrlremap_;

typedef struct {
	int state;
	int master_interval;
	int interval;
} ctrl_remapper_;

typedef struct {
	int state;
	char text[2][33];
} joy_cfg_;

extern ctrlremap_ ctrl_remaps[MAX_CTRL_REMAPS];
extern ctrl_remapper_ ctrl_remapper;
extern joy_cfg_ joy_cfg;

/* Function prototypes */
void toggle_emulator_paused(int force);
void toggle_ldfile_state(void);
void toggle_sstate_state(int mode);
int runopts_is_a_reset_scheduled(void);
void runopts_transit(int state);
void key_repeat_manager(int funcid, SDL_Event *event, int eventid);
void keyboard_buffer_reset(int shift_reset, int exclude1, int exclude2);
int keysym_to_keycode(char *keysym);
char *keycode_to_keysym(int keycode);



