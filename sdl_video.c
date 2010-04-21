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
unsigned char vga_graphmemory[64 * 1024];

/* \x1 means that a value needs to be placed here.
 * \x2 means to invert the colours.
 * \x80 to \x95 are Sinclair graphics characters */
char *runtime_options0_text[24] = {
	"\x2 Runtime Options                \x2",
	"",
	"Volume\x90\x2<\x2\x85\x1   \x90\x2>\x2\x85",
	"",
	"Key Repeat:",
	"",
	"  Delay\x90\x2<\x2\x85\x1   ms\x90\x2>\x2\x85",
	"",
	"  Interval\x90\x2<\x2\x85\x1   ms\x90\x2>\x2\x85",
	"",
	"Colours:",
	"",
	"  Fore:R\x90\x2<\x2\x85\x1  \x90\x2>\x2\x85 Back:R\x90\x2<\x2\x85\x1  \x90\x2>\x2\x85",
	"",
	"       G\x90\x2<\x2\x85\x1  \x90\x2>\x2\x85      G\x90\x2<\x2\x85\x1  \x90\x2>\x2\x85",
	"",
	"       B\x90\x2<\x2\x85\x1  \x90\x2>\x2\x85      B\x90\x2<\x2\x85\x1  \x90\x2>\x2\x85",
	"",
	"", 
	"",
	"",
	"",
	"",
	"          Save    Exit   Next\x90\x2>\x2\x85"
};

char *runtime_options1_text[24] = {
	"\x2 Runtime Options                \x2",
	"",
	"Joy Axis Dead Zone\x90\x2<\x2\x85\x1  \%\x90\x2>\x2\x85",
	"",
	"Joystick Configurator:",
	"",
	"  Selector u/d/l/r and hit are",
	"  the minimum requirements.",
	"",
	"   \x92\x2---\x2\x84      \x92\x2---\x2\x84",
	"  \x92\x81   \x82\x83\x83\x83\x83\x83\x83\x81   \x82\x84",
	"  \x86  \x8e          \x8e  \x91",
	"  \x85 \x88\x8b\x88  \x89\x8a\x89\x8a  /\x83  \x90",
	"  \x85\x90\x8b\x2O\x2\x8b\x85      \x90\x85/\x90\x85\x90",
	"  \x85 \x88\x8b\x88 \x90\x2.\x2\x85\x90\x2>\x2\x85  \x8e/ \x90",
	"  \x91  \x83          \x83  \x86",
	"  \x82\x84   \x92\x8e\x8e\x8e\x8e\x8e\x8e\x84   \x92\x81",
	"   \x82\x83\x83\x83\x81      \x82\x83\x83\x83\x81",
	"",
	"  \x1 ",
	"  \x1 ",
	"  \x1 ",
	"",
	"\x90\x2<\x2\x85" "Back   Save    Exit          "
};

/* Function prototypes */
SDL_Surface *BMF_RenderText(int font, char *text, Uint32 fg_colour, Uint32 bg_colour);
Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);


/***************************************************************************
 * Set Video Mode                                                          *
 ***************************************************************************/
/* This can be called multiple times and it is if the resolution is switched.
 * 
 * On exit: returns TRUE on error
 *          else FALSE */

int sdl_video_setmode(void) {
	int original_xres = video.xres;
	
	/* Try the requested video resolution and if it's unavailable
	 * try the next one down and continue until one is accepted.
	 * Note: I first used SDL_VideoModeOK() but it didn't work on
	 * my Zaurus so it's probably best to use brute force.
	 * 
	 * sz81 uses alpha and so it must use an SDL_SWSURFACE */
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

	/* Set-up runtime options' screen offset */
	runtime_options0.xoffset = (video.xres - 256 * video.scale) / 2;
	if (runtime_options0.xoffset < 0) runtime_options0.xoffset = 0;
	runtime_options0.yoffset = (video.yres - 192 * video.scale) / 2;
	if (runtime_options0.yoffset < 0) runtime_options0.yoffset = 0;

	/* Set-up runtime options' screen offset */
	runtime_options1.xoffset = (video.xres - 256 * video.scale) / 2;
	if (runtime_options1.xoffset < 0) runtime_options1.xoffset = 0;
	runtime_options1.yoffset = (video.yres - 192 * video.scale) / 2;
	if (runtime_options1.yoffset < 0) runtime_options1.yoffset = 0;
	
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

	return FALSE;
}

/***************************************************************************
 * VGA Get Graph Mem                                                       *
 ***************************************************************************/
/* On exit: returns a pointer to the graphics memory */

unsigned char *vga_getgraphmem(void) {
	return vga_graphmemory;
}

/***************************************************************************
 * Update Video                                                            *
 ***************************************************************************/
/* This does the following things :-
 * 
 * Firstly the component_executive is called to check everything is up-to-date.
 * It'll redraw the entire screen if video.redraw is TRUE.
 * The emulator's 8 bit 320x200 VGA memory is scaled-up into the SDL screen surface.
 * Possibly the load selector, vkeyb, control bar, runtime options and associated
 * hotspots will require overlaying */

void sdl_video_update(void) {
	int srcx, srcy, desx, desy, srcw, count;
	Uint32 colour, fg_colour, bg_colour;
	Uint32 *screen_pixels_32;
	Uint16 *screen_pixels_16;
	SDL_Surface *renderedtext;
	SDL_Rect dstrect;
	char text[33];
	#ifdef SDL_DEBUG_FONTS
		struct bmpfont *ptrfont;
		int fontcount;
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

	/* Monitor and manage component states */
	component_executive();

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

	/* Is the emulator's output being rendered? */
	if (emulator.state) {
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
	}
	
	/* Are the virtual keyboard and control bar being rendered? */
	if (vkeyb.state) {
		if (vkeyb.alpha) {
			dstrect.x = vkeyb.xoffset; dstrect.y = vkeyb.yoffset;
			dstrect.w = vkeyb.scaled->w; dstrect.h =  vkeyb.scaled->h;
			if (SDL_BlitSurface (vkeyb.scaled, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__,
					SDL_GetError ());
				exit(1);
			}
		}
		/* Show the control bar */
		dstrect.x = control_bar.xoffset; dstrect.y = control_bar.yoffset;
		dstrect.w = control_bar.scaled->w; dstrect.h = control_bar.scaled->h;
		if (SDL_BlitSurface (control_bar.scaled, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
	}

	/* Are the runtime options being rendered? */
	if (runtime_options0.state || runtime_options1.state) {
		if (runtime_options0.state) {
			srcx = runtime_options0.xoffset; srcy = runtime_options0.yoffset;
		} else {
			srcx = runtime_options1.xoffset; srcy = runtime_options1.yoffset;
		}
		if (!invert_screen) {
			colour = bg_colour;
		} else {
			colour = fg_colour;
		}
		/* Draw the background minus the header and nav control lines */
		dstrect.x = srcx; dstrect.y = srcy + 8 * video.scale;
		dstrect.w = 256 * video.scale; dstrect.h = 176 * video.scale;
		if (SDL_FillRect(video.screen, &dstrect, colour) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		/* Set the font colours we'll be using */
		if (!invert_screen) {
			fg_colour = colours.emu_fg; bg_colour = colours.emu_bg;
		} else {
			fg_colour = colours.emu_bg; bg_colour = colours.emu_fg;
		}
		/* Draw the static text */
		for (desy = 0; desy < 24; desy++) {
			dstrect.x = srcx; desx = 0;
			for (;;) {
				do {
					if (runtime_options0.state) {
						text[0] = runtime_options0_text[desy][desx++];
					} else if (runtime_options1.state) {
						text[0] = runtime_options1_text[desy][desx++];
					}
					/* Invert the colours */
					if (text[0] == 2) {
						colour = fg_colour; fg_colour = bg_colour; bg_colour = colour;
					}
				} while (text[0] == 1 || text[0] == 2);
				if (text[0]) {
					text[1] = 0;	/* Null terminate the string */
					renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, fg_colour, bg_colour);
					dstrect.y = srcy + desy * 8 * video.scale;
					dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
					if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
						fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
						exit(1);
					}
					SDL_FreeSurface(renderedtext);
				} else {
					break;	/* Process next row */
				}
				dstrect.x += 8 * video.scale;
			}
		}
		/* Write the values at specific positions */
		count = 0;
		for (desy = 0; desy < 24; desy++) {
			dstrect.x = srcx; desx = 0;
			for (;;) {
				do {
					if (runtime_options0.state) {
						text[0] = runtime_options0_text[desy][desx++];
					} else if (runtime_options1.state) {
						text[0] = runtime_options1_text[desy][desx++];
					}
					/* Invert the colours */
					if (text[0] == 2) {
						colour = fg_colour; fg_colour = bg_colour; bg_colour = colour;
					}
				} while (text[0] == 2);
				if (text[0] == 1) {
					if (runtime_options0.state) {
						if (count == 0) {
							sprintf(text, "%3i", sdl_sound.volume);
						} else if (count == 1) {		
							sprintf(text, "%3i", sdl_key_repeat.delay);
						} else if (count == 2) {		
							sprintf(text, "%3i", sdl_key_repeat.interval);
						} else if (count == 3) {		
							sprintf(text, "%02x", colours.emu_fg >> 16 & 0xff);
						} else if (count == 4) {		
							sprintf(text, "%02x", colours.emu_bg >> 16 & 0xff);
						} else if (count == 5) {		
							sprintf(text, "%02x", colours.emu_fg >> 8 & 0xff);
						} else if (count == 6) {		
							sprintf(text, "%02x", colours.emu_bg >> 8 & 0xff);
						} else if (count == 7) {		
							sprintf(text, "%02x", colours.emu_fg & 0xff);
						} else if (count == 8) {		
							sprintf(text, "%02x", colours.emu_bg & 0xff);
						} else {		
							sprintf(text, "%s", "???");
						}
					} else if (runtime_options1.state) {
						if (count == 0) {
							sprintf(text, "%2i", joystick_dead_zone);
						} else if (count >= 1 && count <= 3) {		
							sprintf(text, "%s", joy_cfg.text[count - 1]);
						} else {		
							sprintf(text, "%s", "???");
						}
					}
					dstrect.y = srcy + desy * 8 * video.scale;
					renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, fg_colour, bg_colour);
					dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
					if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
						fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
						exit(1);
					}
					SDL_FreeSurface(renderedtext);
					count++; desx++;
				} else if (text[0] == 0) {
					break;	/* Process next row */
				}
				dstrect.x += 8 * video.scale;
			}
		}
	}

	/* Render visible hotspots now */
	hotspots_render();

	/* Render any existing message box */
	message_box_manager(MSG_BOX_SHOW, NULL);

	/* If the user has passed the -d option then show
	 * the currently pressed control id on-screen */
	if (sdl_cl_show_input_id && current_input_id != UNDEFINED) {
		if (invert_screen) {
			fg_colour = colours.emu_fg; bg_colour = colours.emu_bg;
		} else {
			fg_colour = colours.emu_bg; bg_colour = colours.emu_fg;
		}
		sprintf(text, "%i", current_input_id);
		if (zx80) {
			renderedtext = BMF_RenderText(BMF_FONT_ZX80, text, fg_colour, bg_colour);
		} else {
			renderedtext = BMF_RenderText(BMF_FONT_ZX81, text, fg_colour, bg_colour);
		}
		dstrect.x = video.screen->w / 2 - renderedtext->w / 2;
		dstrect.y = video.screen->h / 2 - renderedtext->h / 2;
		dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
		if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		SDL_FreeSurface(renderedtext);
	}

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
 * Render Bitmap Font Text                                                 *
 ***************************************************************************/
/* On entry: font should be BMF_FONT_ZX80, BMF_FONT_ZX81 or BMF_FONT_ZX82
 *           text points to the ASCII text to render (BMF_FONT_ZX82 chars
 *               128 to 149 are Sinclair graphics characters)
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
	int reuseindex;

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
		/* Not found and no free slot remaining so reuse the least requested */
		index = 1;
		for (reuseindex = 1; reuseindex < MAX_FONTS; reuseindex++) {
			if (ptrfont->scaled[reuseindex] && ptrfont->requested[reuseindex] >= 
				ptrfont->requested[index]) index = reuseindex;
		}
		SDL_FreeSurface(ptrfont->scaled[index]);
	}	
	if (!found) {
		/* Didn't find one so we'll have to create it */
		/* Make a duplicate of font.scaled[0] the easy way */
		ptrfont->scaled[index] = SDL_DisplayFormat(ptrfont->scaled[0]);
		if (ptrfont->scaled[index] == NULL) {
			fprintf(stderr, "%s: Cannot create surface: %s\n", __func__, SDL_GetError ());
			return NULL;
		}
		/* Store fg_colour */
		ptrfont->fg_colour[index] = fg_colour;
		/* Initialise requested count */
		ptrfont->requested[index] = 0;
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

	/* Increment requested count */
	ptrfont->requested[index] += 1;

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
		srcrect.x = ((Uint8)text[count] - 32) * 8 * video.scale; srcrect.y = 0;
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
		/* 640x480 | 240x320 */
		if (video.xres == 640) {
			video.xres = 240; video.yres = 320; video.scale = 1;
		} else {
			video.xres = 640; video.yres = 480; video.scale = 2;
		}
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
	Uint32 colour, fg_colour, bg_colour;
	SDL_Surface *renderedtext, *shadow;
	SDL_Rect winrect, dstrect;
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
			if (!invert_screen) {
				fg_colour = colours.emu_fg; bg_colour = colours.emu_bg;
			} else {
				fg_colour = colours.emu_bg; bg_colour = colours.emu_fg;
			}
			/* winrect will be the reference coords of the window that
			 * everything else will be relative to and it shouldn't be
			 * passed to SDL since SDL will clip it if it's oversize */
			if (strlen(the_box.title) > strlen(the_box.text)) {
				winrect.w = (strlen(the_box.title) + 2) * 8 * video.scale;
			} else {
				winrect.w = (strlen(the_box.text) + 2) * 8 * video.scale;
			}
			winrect.h = 4.5 * 8 * video.scale;
			winrect.x = video.screen->w / 2 - (winrect.w + 8 * video.scale) / 2;
			winrect.y = video.screen->h / 2 - (winrect.h + 8 * video.scale) / 2;
			/* Draw the window background */
			dstrect.x = winrect.x; dstrect.y = winrect.y; 
			dstrect.w = winrect.w; dstrect.h = winrect.h; 
			if (SDL_FillRect(video.screen, &dstrect, SDL_MapRGB(video.screen->format,
				fg_colour >> 16 & 0xff, fg_colour >> 8 & 0xff, fg_colour & 0xff)) < 0) {
				fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			/* Invert the colours */
			colour = fg_colour; fg_colour = bg_colour; bg_colour = colour;
			/* Write the title */
			renderedtext = BMF_RenderText(BMF_FONT_ZX82, the_box.title, fg_colour, bg_colour);
			dstrect.x = winrect.x + 8 * video.scale; dstrect.y = winrect.y;
			dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
			if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			SDL_FreeSurface(renderedtext);
			/* Draw the window interior */
			dstrect.x = winrect.x + 4 * video.scale; dstrect.y = winrect.y + 8 * video.scale;
			dstrect.w = winrect.w - 8 * video.scale; dstrect.h = winrect.h - 1.5 * 8 * video.scale; 
			if (SDL_FillRect(video.screen, &dstrect, SDL_MapRGB(video.screen->format,
				fg_colour >> 16 & 0xff, fg_colour >> 8 & 0xff, fg_colour & 0xff)) < 0) {
				fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			/* Invert the colours */
			colour = fg_colour; fg_colour = bg_colour; bg_colour = colour;
			/* Write the text */
			renderedtext = BMF_RenderText(BMF_FONT_ZX82, the_box.text, fg_colour, bg_colour);
			dstrect.x = winrect.x + 8 * video.scale; dstrect.y = winrect.y + 2 * 8 * video.scale;
			dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
			if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			SDL_FreeSurface(renderedtext);
			/* Draw the right-hand and bottom shadows */
			for (count = 0; count < 2; count++) {
				if (count == 0) {
					dstrect.w = 8 * video.scale; dstrect.h = winrect.h;
				} else {
					dstrect.w = winrect.w - 8 * video.scale; dstrect.h = 8 * video.scale;
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
					dstrect.x = winrect.x + winrect.w; dstrect.y = winrect.y + 8 * video.scale;
				} else {
					dstrect.x = winrect.x + 8 * video.scale; dstrect.y = winrect.y + winrect.h;
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



