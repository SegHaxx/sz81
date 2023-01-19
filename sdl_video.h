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
#define MAX_RUNTIME_OPTIONS 4
#define MAX_FRAMESKIP 9

/* Joystick configurator text IDs */
#define JOY_CFG_TEXT_DEFAULT_SETTINGS 0
#define JOY_CFG_TEXT_PRESS_SOMETHING 1
#define JOY_CFG_TEXT_ACCEPTED 2
#define JOY_CFG_TEXT_CANCELLED 3

/* Variables */
extern char *runtime_options_text0[24];
extern char *runtime_options_text1[24];
extern char *runtime_options_text2[24];
extern char *runtime_options_text3[24];

typedef struct video_options {
	SDL_Surface *screen;
	int bigscreen;
	int fullscreen;		        /* 0 or SDL_FULLSCREEN */
	int scale;			/* 1, 2 or 3 */
	int xres;
	int yres;
	int redraw;			/* TRUE to redraw entire screen */
} video_;

extern video_ video;

/* Function prototypes */
SDL_Surface *BMF_RenderText(int font, char *text, Uint32 fg_colour, Uint32 bg_colour);
Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void scale_surface(SDL_Surface *original, SDL_Surface *scaled);
void draw_shadow(SDL_Rect dstrect, int alpha);
void cycle_resolutions(void);
Uint32 adjust_colour_component(Uint32 rgb, Uint32 mask, int amount, int granulate);
void set_joy_cfg_text(int textid);
void save_screenshot(void);




