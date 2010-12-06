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
/* Message box manager function IDs */
#define MSG_BOX_SHOW 1
#define MSG_BOX_KILL 2

/* Message box timeouts in ms */
#define MSG_BOX_TIMEOUT_RUNOPTS_SAVE 1500
#define MSG_BOX_TIMEOUT_SOUND_VOLUME 1500
#define MSG_BOX_TIMEOUT_CONTROL_REMAPPER 750

/* Joystick configurator text IDs */
#define JOY_CFG_TEXT_DEFAULT_SETTINGS 0
#define JOY_CFG_TEXT_PRESS_SOMETHING 1
#define JOY_CFG_TEXT_ACCEPTED 2
#define JOY_CFG_TEXT_CANCELLED 3

/* Variables */
char *runtime_options_text0[24];
char *runtime_options_text1[24];
char *runtime_options_text2[24];
char *runtime_options_text3[24];

struct {
	SDL_Surface *screen;
	int fullscreen;		/* 0 or SDL_FULLSCREEN */
	int scale;			/* 1, 2 or 3 */
	int xres;
	int yres;
	int redraw;			/* TRUE to redraw entire screen */
} video;

struct MSG_Box {
	char title[33];		/* The title bar text */
	char text[33];		/* A single line message */
	int timeout;		/* In ms */
};

/* Function prototypes */
void scale_surface(SDL_Surface *original, SDL_Surface *scaled);
void cycle_resolutions(void);
Uint32 adjust_colour_component(Uint32 rgb, Uint32 mask, int amount, int granulate);
void message_box_manager(int funcid, struct MSG_Box *msg_box);
void set_joy_cfg_text(int textid);




