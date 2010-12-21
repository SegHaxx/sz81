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


/***************************************************************************
 *                                                                         *
 ***************************************************************************/
/* On exit: returns FALSE on success else
 *          returns TRUE on error */







/***************************************************************************
 * Message Box Manager                                                     *
 ***************************************************************************/
/* This recreates and draws an existing message box in the middle of the
 * screen every frame. It is recreated because the fg/bg colours or the
 * resolution could have changed in the meantime - not that it's a problem.
 * 
 * struct MSG_Box {
 *     char title[33];    The title bar text
 *     char text[33];     A single line message
 *     int timeout;       In ms
 * };
 * 
 * On entry: funcid = MSG_BOX_SHOW with
 *                    msg_box pointing to the msg_box to show
 *           funcid = MSG_BOX_SHOW with
 *                    msg_box = NULL to show an existing msgbox
 *           funcid = MSG_BOX_KILL to kill an existing msgbox */

void message_box_manager(int funcid, struct MSG_Box *msg_box) {
	static struct MSG_Box the_box;
	static Uint32 last_time;
	static int init = TRUE;
	int window_x, window_y, window_w, window_h;
	Uint32 colour, fg_colour, bg_colour;
	SDL_Surface *renderedtext, *shadow;
	SDL_Rect dstrect;
	int count;

	if (init) {
		init = FALSE;
		the_box.title[0] = 0; the_box.text[0] = 0; the_box.timeout = 0;
	}

	if (funcid == MSG_BOX_SHOW && msg_box != NULL) {
		/* Initialise a new msgbox / overwrite an existing msgbox */
		the_box = *msg_box;
		last_time = SDL_GetTicks();
	} else if (funcid == MSG_BOX_SHOW && msg_box == NULL) {
		/* Show an existing msgbox */
		if (the_box.timeout > 0) {
			/* Prepare the colours we shall be using */
			if (!sdl_emulator.invert) {
				fg_colour = colours.emu_fg; bg_colour = colours.emu_bg;
			} else {
				fg_colour = colours.emu_bg; bg_colour = colours.emu_fg;
			}
			/* window_x/y/w/h will be the reference coords of the window
			 * that everything else will be relative to */
			if (strlen(the_box.title) > strlen(the_box.text)) {
				window_w = (strlen(the_box.title) + 2) * 8 * video.scale;
			} else {
				window_w = (strlen(the_box.text) + 2) * 8 * video.scale;
			}
			window_h = 4.5 * 8 * video.scale;
			window_x = video.screen->w / 2 - (window_w + 8 * video.scale) / 2;
			window_y = video.screen->h / 2 - (window_h + 8 * video.scale) / 2;
			/* Draw the window background */
			dstrect.x = window_x; dstrect.y = window_y; 
			dstrect.w = window_w; dstrect.h = window_h; 
			if (SDL_FillRect(video.screen, &dstrect, SDL_MapRGB(video.screen->format,
				fg_colour >> 16 & 0xff, fg_colour >> 8 & 0xff, fg_colour & 0xff)) < 0) {
				fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			/* Invert the colours */
			colour = fg_colour; fg_colour = bg_colour; bg_colour = colour;
			/* Write the title */
			renderedtext = BMF_RenderText(BMF_FONT_ZX82, the_box.title, fg_colour, bg_colour);
			dstrect.x = window_x + 8 * video.scale; dstrect.y = window_y;
			dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
			if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			SDL_FreeSurface(renderedtext);
			/* Draw the window interior */
			dstrect.x = window_x + 4 * video.scale; dstrect.y = window_y + 8 * video.scale;
			dstrect.w = window_w - 8 * video.scale; dstrect.h = window_h - 1.5 * 8 * video.scale; 
			if (SDL_FillRect(video.screen, &dstrect, SDL_MapRGB(video.screen->format,
				fg_colour >> 16 & 0xff, fg_colour >> 8 & 0xff, fg_colour & 0xff)) < 0) {
				fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			/* Invert the colours */
			colour = fg_colour; fg_colour = bg_colour; bg_colour = colour;
			/* Write the text */
			renderedtext = BMF_RenderText(BMF_FONT_ZX82, the_box.text, fg_colour, bg_colour);
			dstrect.x = window_x + 8 * video.scale; dstrect.y = window_y + 2 * 8 * video.scale;
			dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
			if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			SDL_FreeSurface(renderedtext);
			/* Draw the right-hand and bottom shadows */
			for (count = 0; count < 2; count++) {

				/* I want to document some odd behaviour that I experienced here
				 * on my GP2X using the GPH SDK, but not anything else:
				 * Originally window_x/y/w/h was an SDL_Rect winrect and I found 
				 * that at this point winrect.y was 98 and after this 'if' 
				 * statement it would become 0! It doesn't make any sense at all 
				 * as only dstrect is being written to and I found a way to fix 
				 * this issue by moving SDL_Rect winrect out of this function to 
				 * the top of this file. I decided to simply not use an SDL_Rect
				 * to store data and replaced it with four integers instead.
				 * Since I've only experienced this with the GPH SDK I'm expecting
				 * that there's an issue with that and not anything else */

				if (count == 0) {
					dstrect.w = 8 * video.scale; dstrect.h = window_h;
				} else {
					dstrect.w = window_w - 8 * video.scale; dstrect.h = 8 * video.scale;
				}
				shadow = SDL_CreateRGBSurface(SDL_SWSURFACE, dstrect.w, dstrect.h,
					video.screen->format->BitsPerPixel,
					video.screen->format->Rmask, video.screen->format->Gmask,
					video.screen->format->Bmask, video.screen->format->Amask);
				if (shadow == NULL) {
					fprintf(stderr, "%s: Cannot create RGB surface: %s\n", __func__, 
						SDL_GetError ());
					exit(1);
				}
				/* Fill the shadow with black */
				if (SDL_FillRect(shadow, NULL, 0) < 0) {
					fprintf(stderr, "%s: FillRect error: %s\n", __func__,
						SDL_GetError ());
					exit(1);
				}
				/* Set the alpha for the entire surface */
				if (SDL_SetAlpha(shadow, SDL_SRCALPHA, 64) < 0) {	/* 25% */
					fprintf(stderr, "%s: Cannot set surface alpha: %s\n", __func__,
						SDL_GetError());
					exit(1);
				}
				/* Blit it to the screen */
				if (count == 0) {
					dstrect.x = window_x + window_w; dstrect.y = window_y + 8 * video.scale;
				} else {
					dstrect.x = window_x + 8 * video.scale; dstrect.y = window_y + window_h;
				}
				dstrect.w = shadow->w; dstrect.h = shadow->h;
				if (SDL_BlitSurface (shadow, NULL, video.screen, &dstrect) < 0) {
					fprintf(stderr, "%s: BlitSurface error: %s\n", __func__,
						SDL_GetError ());
					exit(1);
				}
				SDL_FreeSurface(shadow);
			}
			/* Decrement the timeout */			
			the_box.timeout -= SDL_GetTicks() - last_time;
			last_time = SDL_GetTicks();
		}
	} else if (funcid == MSG_BOX_KILL) {
		/* Kill an existing msgbox */
		the_box.timeout = 0;
	}
}


