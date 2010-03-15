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

/* Function prototypes */
SDL_Surface *BMF_RenderText(int font, char *text, Uint32 fg_colour, Uint32 bg_colour);
Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);


/***************************************************************************
 * VGA Set Mode                                                            *
 ***************************************************************************/
/* This can be called multiple times and it is if the resolution is switched.
 * 
 * On exit: returns 0 on success or -1 on failure */

int vga_setmode(void) {
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

/***************************************************************************
 * Update Screen                                                           *
 ***************************************************************************/
/* This does the following things :-
 * 
 * Firstly the component_executive is called to check everything is up-to-date.
 * It'll redraw the entire screen if video.redraw is TRUE.
 * The emulator's 8 bit 320x200 VGA memory is scaled-up into the SDL screen surface.
 * Possibly the vkeyb, control bar and associated hotspots will require overlaying.
 * Possibly the load selector's hotspots will need to be overlayed */

void sdl_update_scrn(void) {
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

