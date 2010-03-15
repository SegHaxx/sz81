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

/* Variables */

/* Function prototypes */


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

