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

/* Includes */
#include "sdl_engine.h"
#include "common.h"
#include "zx81.h"

/* Defines */

/* Variables */
//unsigned char vga_graphmemory[64 * 1024]; // Sorry about that!
unsigned char vga_graphmemory[ZX_VID_X_WIDTH * ZX_VID_X_HEIGHT];

extern ZX81 zx81;

char *runtime_options_text0[24];
char *runtime_options_text1[24];
char *runtime_options_text2[24];
char *runtime_options_text3[24];

video_ video;

/* \x1 means that a value needs to be placed here.
 * \x2 means to invert the colours.
 * \x80 to \x95 are Sinclair graphics characters.
 * \x96 to \x97 are up and down </> equivalents */
char *runtime_options_text0[24] = {
	"\x2 Hardware Options           1/4 \x2",
	"",
	"Machine Model:",
	"",
	"  (\x1 \x1) ZX80  (\x1 \x1) ZX81",
	"",
	"RAM Size \x90\x2<\x2\x85 \x1  K \x90\x2>\x2\x85",
	"",
	"M1NOT:",
	"",
	"  (\x1 \x1) No    (\x1 \x1) Yes",
	"",
	"Frameskip\x90\x2<\x2\x85   \x1  \x90\x2>\x2\x85",
	"",
#ifdef ENABLE_EMULATION_SPEED_ADJUST
	"Emu Speed\x90\x2<\x2\x85\x1    %\x90\x2>\x2\x85",
#else
	"",
#endif
	"",
	"WRX:",
	"  (\x1 \x1) No    (\x1 \x1) Yes",
	"UDG:",
	"  (\x1 \x1) No    (\x1 \x1) Yes",
	"",
	"\x1 ",
	"",
	"          Save    Exit   Next\x90\x2>\x2\x85"
};

char *runtime_options_text1[24] = {
	"\x2 Sound Options              2/4 \x2",
	"",
	"Sound Device:",
	"",
	"  (\x1 \x1) None",
	"",
	"  (\x1 \x1) AY Chip Based",
	"",
	"     (\x1 \x1) Quicksilva Sound Board",
	"",
	"     (\x1 \x1) BI-PAK ZON X-81",
	"",
	"     (\x1 \x1) Unreal",
	"",
	"  (\x1 \x1) VSYNC (TV Speaker)",
	"",
	"",
	"",
	"",
	"",
	"",
	"\x1 ",
	"",
	"\x90\x2<\x2\x85" "Back   Save    Exit   Next\x90\x2>\x2\x85"
};

char *runtime_options_text2[24] = {
	"\x2 GUI Options                3/4 \x2",
	"",
	"Volume    \x90\x2<\x2\x85  \x1   \x90\x2>\x2\x85",
	"",
	"Key Repeat:",
	"",
	"  Delay   \x90\x2<\x2\x85\x1   ms\x90\x2>\x2\x85",
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
	"\x1 ",
	"",
	"\x90\x2<\x2\x85" "Back   Save    Exit   Next\x90\x2>\x2\x85"
};

char *runtime_options_text3[24] = {
	"\x2 Joystick Options           4/4 \x2",
	"",
	"Axis Dead Zone\x90\x2<\x2\x85\x1  \%\x90\x2>\x2\x85",
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
	"  \x1 ",
	"  \x1 ",
	"",
	"\x1 ",
	"",
	"\x90\x2<\x2\x85" "Back   Save    Exit          "
};

char save_state_dialog_icons[72] = {
	0x00, 0x18, 0x28, 0x08, 0x08, 0x08, 0x3e, 0x00,
	0x00, 0x3c, 0x42, 0x02, 0x3c, 0x40, 0x7e, 0x00,
	0x00, 0x3c, 0x42, 0x0c, 0x02, 0x42, 0x3c, 0x00,
	0x00, 0x08, 0x18, 0x28, 0x48, 0x7e, 0x08, 0x00,
	0x00, 0x7e, 0x40, 0x7c, 0x02, 0x42, 0x3c, 0x00,
	0x00, 0x3c, 0x40, 0x7c, 0x42, 0x42, 0x3c, 0x00,
	0x00, 0x7e, 0x02, 0x04, 0x08, 0x10, 0x10, 0x00,
	0x00, 0x3c, 0x42, 0x3c, 0x42, 0x42, 0x3c, 0x00,
	0x00, 0x3c, 0x42, 0x42, 0x3e, 0x02, 0x3c, 0x00
};

char dialog_icons[32] = {
	0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x10, 0x00,
	0x00, 0x10, 0x00, 0x30, 0x10, 0x10, 0x38, 0x00,
	0x00, 0x3c, 0x42, 0x04, 0x08, 0x00, 0x08, 0x00,
	0x00, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x00
};


/* Function prototypes */


/***************************************************************************
 * Set Video Mode                                                          *
 ***************************************************************************/
/* This can be called multiple times and it is if the resolution is switched.
 * 
 * On exit: returns TRUE on error
 *          else FALSE */

int sdl_video_setmode(void) {
	int original_xres = video.xres;
	int count;

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
	#if defined(PLATFORM_GP2X) || defined(PLATFORM_ZAURUS)
		SDL_ShowCursor(SDL_DISABLE);
	#endif
	
	/* Set-up the emulator's screen offset - not used at the moment */
	if (video.bigscreen) {
		sdl_emulator.xoffset = (video.xres - 400 * video.scale) / 2;
		sdl_emulator.yoffset = (video.yres - 300 * video.scale) / 2;
	} else {
		sdl_emulator.xoffset = (video.xres - 320 * video.scale) / 2;
		sdl_emulator.yoffset = (video.yres - 240 * video.scale) / 2;
	}

	#ifdef SDL_DEBUG_VIDEO
		printf("%s: sdl_emulator.xoffset=%i sdl_emulator.yoffset=%i\n", 
			__func__, sdl_emulator.xoffset, sdl_emulator.yoffset);
	#endif

	/* Set-up runtime options' screen offset */
	for (count = 0; count < MAX_RUNTIME_OPTIONS; count++) {
		runtime_options[count].xoffset = (video.xres - 256 * video.scale) / 2;
		if (runtime_options[count].xoffset < 0) runtime_options[count].xoffset = 0;
		runtime_options[count].yoffset = (video.yres - 192 * video.scale) / 2;
		if (runtime_options[count].yoffset < 0) runtime_options[count].yoffset = 0;
	}

	/* Set-up the load file dialog's screen offset */
	load_file_dialog.xoffset = (video.xres - 256 * video.scale) / 2;
	if (load_file_dialog.xoffset < 0) load_file_dialog.xoffset = 0;
	load_file_dialog.yoffset = (video.yres - 192 * video.scale) / 2;
	if (load_file_dialog.yoffset < 0) load_file_dialog.yoffset = 0;

	/* Set-up the save state dialog's screen offset */
	save_state_dialog.xoffset = (video.screen->w - 15 * 8 * video.scale) / 2;
	save_state_dialog.yoffset = (video.screen->h - 17 * 8 * video.scale) / 2;

	/* The dynamic dialog's screen offset is set-up in hotspots_resize */

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
	if (hotspots[HS_VKEYB_VKEYB].gid == HS_GRP_VKEYB) hotspots_resize(HS_GRP_ALL);

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

void sdl_set_redraw_video() {
	video.redraw = TRUE;
}

int cvtChroma(unsigned char c) {
	int crgb=0;
#ifdef ZXMORE
	switch (c) {
	  case 0x00 : crgb=SDL_MapRGB(video.screen->format, 0x00,0x00,0x00); break;
	  case 0x01 : crgb=SDL_MapRGB(video.screen->format, 0xbf,0xbf,0xdf); break;
	  case 0x02 : crgb=SDL_MapRGB(video.screen->format, 0xdf,0xbf,0xbf); break;
	  case 0x03 : crgb=SDL_MapRGB(video.screen->format, 0xdf,0xbf,0xdf); break;
	  case 0x04 : crgb=SDL_MapRGB(video.screen->format, 0xbf,0xdf,0xbf); break;
	  case 0x05 : crgb=SDL_MapRGB(video.screen->format, 0xbf,0xdf,0xdf); break;
	  case 0x06 : crgb=SDL_MapRGB(video.screen->format, 0xdf,0xdf,0xbf); break;
	  case 0x07 : crgb=SDL_MapRGB(video.screen->format, 0xdf,0xdf,0xdf); break;
	  case 0x08 : crgb=SDL_MapRGB(video.screen->format, 0xbf,0xbf,0xbf); break;
	  case 0x09 : crgb=SDL_MapRGB(video.screen->format, 0xbf,0xbf,0xff); break;
	  case 0x0a : crgb=SDL_MapRGB(video.screen->format, 0xff,0xbf,0xbf); break;
	  case 0x0b : crgb=SDL_MapRGB(video.screen->format, 0xff,0xbf,0xff); break;
	  case 0x0c : crgb=SDL_MapRGB(video.screen->format, 0xbf,0xff,0xbf); break;
	  case 0x0d : crgb=SDL_MapRGB(video.screen->format, 0xbf,0xff,0xff); break;
	  case 0x0e : crgb=SDL_MapRGB(video.screen->format, 0xff,0xff,0xbf); break;
	  case 0x0f : crgb=SDL_MapRGB(video.screen->format, 0xff,0xff,0xff); break;
	}
#else
	switch (c) {
	  case 0x00 : crgb=SDL_MapRGB(video.screen->format, 0x00,0x00,0x00); break;
	  case 0x01 : crgb=SDL_MapRGB(video.screen->format, 0x00,0x00,0x7f); break;
	  case 0x02 : crgb=SDL_MapRGB(video.screen->format, 0x7f,0x00,0x00); break;
	  case 0x03 : crgb=SDL_MapRGB(video.screen->format, 0x7f,0x00,0x7f); break;
	  case 0x04 : crgb=SDL_MapRGB(video.screen->format, 0x00,0x7f,0x00); break;
	  case 0x05 : crgb=SDL_MapRGB(video.screen->format, 0x00,0x7f,0x7f); break;
	  case 0x06 : crgb=SDL_MapRGB(video.screen->format, 0x7f,0x7f,0x00); break;
	  case 0x07 : crgb=SDL_MapRGB(video.screen->format, 0x7f,0x7f,0x7f); break;
	  case 0x08 : crgb=SDL_MapRGB(video.screen->format, 0x00,0x00,0x00); break;
	  case 0x09 : crgb=SDL_MapRGB(video.screen->format, 0x00,0x00,0xff); break;
	  case 0x0a : crgb=SDL_MapRGB(video.screen->format, 0xff,0x00,0x00); break;
	  case 0x0b : crgb=SDL_MapRGB(video.screen->format, 0xff,0x00,0xff); break;
	  case 0x0c : crgb=SDL_MapRGB(video.screen->format, 0x00,0xff,0x00); break;
	  case 0x0d : crgb=SDL_MapRGB(video.screen->format, 0x00,0xff,0xff); break;
	  case 0x0e : crgb=SDL_MapRGB(video.screen->format, 0xff,0xff,0x00); break;
	  case 0x0f : crgb=SDL_MapRGB(video.screen->format, 0xff,0xff,0xff); break;
	}
#endif
	return crgb;
}

/***************************************************************************
 * Update Video                                                            *
 ***************************************************************************/
/* This does the following things :-
 * 
 * Firstly the component executive is called to check everything is up-to-date.
 * It'll redraw the entire screen if video.redraw is TRUE.
 * The emulator's 8 bit 320x240 or 400x300 VGA memory is scaled-up into the SDL screen surface.
 * Possibly the load selector, vkeyb, control bar, runtime options and associated
 * hotspots will require overlaying */

void sdl_video_update(void) {
	static Uint32 fg_colourRGB, bg_colourRGB, colour0RGB, colour1RGB;
	static Uint32 fg_colour, bg_colour;
	static int first = TRUE;

	Uint32 colourRGB;
	int srcx, srcy, sx, sy, desx, desy, count, offset, invertcolours;
	int mrgx1, mrgx2, mrgy1, mrgy2;
	int xpos, ypos, xmask, ybyte;
	SDL_Surface *renderedtext;
	char text[33], *direntry;
	SDL_Rect dstrect;

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
	sdl_component_executive();

	if (first == TRUE)
	{
		first = FALSE;

		/* Prepare the colours we shall be using (these remain unchanged throughout) */
		if (!sdl_emulator.invert) {
			fg_colourRGB = SDL_MapRGB(video.screen->format, colours.emu_fg >> 16 & 0xff,
				colours.emu_fg >> 8 & 0xff, colours.emu_fg & 0xff);
			bg_colourRGB = SDL_MapRGB(video.screen->format, colours.emu_bg >> 16 & 0xff,
				colours.emu_bg >> 8 & 0xff, colours.emu_bg & 0xff);
			colour0RGB = fg_colourRGB;
			colour1RGB = bg_colourRGB;
			fg_colour = colours.emu_fg;
			bg_colour = colours.emu_bg;
		} else {
			fg_colourRGB = SDL_MapRGB(video.screen->format, colours.emu_bg >> 16 & 0xff,
				colours.emu_bg >> 8 & 0xff, colours.emu_bg & 0xff);
			bg_colourRGB = SDL_MapRGB(video.screen->format, colours.emu_fg >> 16 & 0xff,
				colours.emu_fg >> 8 & 0xff, colours.emu_fg & 0xff);
			colour0RGB = bg_colourRGB;
			colour1RGB = fg_colourRGB;
			fg_colour = colours.emu_bg;
			bg_colour = colours.emu_fg;
		}
	}

	/* Should everything be redrawn? */
	if (video.redraw) {
		video.redraw = FALSE;
		/* Wipe the entire screen surface */
		if (chromamode) {
			colourRGB = cvtChroma(bordercolour);
		} else {
		#ifdef SDL_DEBUG_VIDEO
			colourRGB = SDL_MapRGB(video.screen->format, 0x0, 0x80, 0xc0);
		#else
			colourRGB = bg_colourRGB;
		#endif
		}
		if (SDL_FillRect(video.screen, NULL, colourRGB) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
	}

	/* Is the emulator's output being rendered? */
	if (sdl_emulator.state) {
		if (SDL_MUSTLOCK(video.screen)) SDL_LockSurface(video.screen);

		/* Set-up destination y coordinates */
		desx = sdl_emulator.xoffset;
		desy = sdl_emulator.yoffset;

		if (video.bigscreen) {
			mrgx1 = mrgx2 = 0;
			mrgy1 = mrgy2 = 0;
		} else {
			mrgx1 = (video.xres/video.scale - 256) / 2;			
			mrgy1 = (video.yres/video.scale - 192) / 2;
			mrgx1 = (13+58-2-32)*2 - mrgx1;
			mrgy1 = 55+2 - mrgy1;
			mrgx2 = ZX_VID_X_WIDTH - mrgx1 - video.xres/video.scale;
			mrgy2 = ZX_VID_X_HEIGHT - mrgy1 - video.yres/video.scale;
		}

		if (mrgx1<0) mrgx2 = 0;
		if (mrgx2<0) mrgx2 = 0;
		if (mrgy1<0) mrgy1 = 0;
		if (mrgy2<0) mrgy2 = 0;

		desy = 0;
		desx = 0;
		sx = ZX_VID_X_WIDTH * mrgy1 + mrgx1;
		sy = sx;

		if (video.screen->format->BitsPerPixel == 16) {
			Uint16 *screen_pixels = video.screen->pixels;

			if (video.scale > 2) {
				int line1 = video.xres;
				int line2 = video.xres * 2;

				for (srcy = mrgy1; srcy < ZX_VID_X_HEIGHT-mrgy2; srcy++) {
					for (srcx = mrgx1; srcx < ZX_VID_X_WIDTH-mrgx2; srcx++) {
						/* Get 8 bit source pixel and convert to RGB */
						colourRGB = chromamode ? cvtChroma(vga_graphmemory[sx++]) : 
									(vga_graphmemory[sx++] == 0) ? colour0RGB : colour1RGB;

						/* Write the destination pixel[s] */
						screen_pixels[desx] = colourRGB;
						screen_pixels[line1 + desx] = colourRGB;
						screen_pixels[line2 + desx++] = colourRGB;
						screen_pixels[desx] = colourRGB;
						screen_pixels[line1 + desx] = colourRGB;
						screen_pixels[line2 + desx++] = colourRGB;
						screen_pixels[desx] = colourRGB;
						screen_pixels[line1 + desx] = colourRGB;
						screen_pixels[line2 + desx++] = colourRGB;
					}
					desy += video.xres * 3;
					desx = desy;
					sy += ZX_VID_X_WIDTH;
					sx = sy;
				}
			}
			else if (video.scale > 1) {
				for (srcy = mrgy1; srcy < ZX_VID_X_HEIGHT-mrgy2; srcy++) {
					for (srcx = mrgx1; srcx < ZX_VID_X_WIDTH-mrgx2; srcx++) {
						/* Get 8 bit source pixel and convert to RGB */
						colourRGB = chromamode ? cvtChroma(vga_graphmemory[sx++]) : 
									(vga_graphmemory[sx++] == 0) ? colour0RGB : colour1RGB;

						/* Write the destination pixel[s] */
						screen_pixels[desx] = colourRGB;
						screen_pixels[video.xres + desx++] = colourRGB;
						screen_pixels[desx] = colourRGB;
						screen_pixels[video.xres + desx++] = colourRGB;
					}
					desy += video.xres * 2;
					desx = desy;
					sy += ZX_VID_X_WIDTH;
					sx = sy;
				}
			}
			else
			{
				for (srcy = mrgy1; srcy < ZX_VID_X_HEIGHT-mrgy2; srcy++) {
					for (srcx = mrgx1; srcx < ZX_VID_X_WIDTH-mrgx2; srcx++) {
						/* Get 8 bit source pixel and convert to RGB */
						colourRGB = chromamode ? cvtChroma(vga_graphmemory[sx++]) : 
									(vga_graphmemory[sx++] == 0) ? colour0RGB : colour1RGB;

						/* Write the destination pixel[s] */
						screen_pixels[desx++] = colourRGB;
					}
					desy += video.xres;
					desx = desy;
					sy += ZX_VID_X_WIDTH;
					sx = sy;
				}				
			}
		}				
		else {
			Uint32 *screen_pixels = video.screen->pixels;

			if (video.scale > 2) {
				int line1 = video.xres;
				int line2 = video.xres * 2;

				for (srcy = mrgy1; srcy < ZX_VID_X_HEIGHT-mrgy2; srcy++) {
					for (srcx = mrgx1; srcx < ZX_VID_X_WIDTH-mrgx2; srcx++) {
						/* Get 8 bit source pixel and convert to RGB */
						colourRGB = chromamode ? cvtChroma(vga_graphmemory[sx++]) : 
									(vga_graphmemory[sx++] == 0) ? colour0RGB : colour1RGB;

						/* Write the destination pixel[s] */
						screen_pixels[desx] = colourRGB;
						screen_pixels[line1 + desx] = colourRGB;
						screen_pixels[line2 + desx++] = colourRGB;
						screen_pixels[desx] = colourRGB;
						screen_pixels[line1 + desx] = colourRGB;
						screen_pixels[line2 + desx++] = colourRGB;
						screen_pixels[desx] = colourRGB;
						screen_pixels[line1 + desx] = colourRGB;
						screen_pixels[line2 + desx++] = colourRGB;
					}
					desy += video.xres * 3;
					desx = desy;
					sy += ZX_VID_X_WIDTH;
					sx = sy;
				}
			}
			else if (video.scale > 1) {
				for (srcy = mrgy1; srcy < ZX_VID_X_HEIGHT-mrgy2; srcy++) {
					for (srcx = mrgx1; srcx < ZX_VID_X_WIDTH-mrgx2; srcx++) {
						/* Get 8 bit source pixel and convert to RGB */
						colourRGB = chromamode ? cvtChroma(vga_graphmemory[sx++]) : 
									(vga_graphmemory[sx++] == 0) ? colour0RGB : colour1RGB;

						/* Write the destination pixel[s] */
						screen_pixels[desx] = colourRGB;
						screen_pixels[video.xres + desx++] = colourRGB;
						screen_pixels[desx] = colourRGB;
						screen_pixels[video.xres + desx++] = colourRGB;
					}
					desy += video.xres * 2;
					desx = desy;
					sy += ZX_VID_X_WIDTH;
					sx = sy;
				}
			}
			else
			{
				for (srcy = mrgy1; srcy < ZX_VID_X_HEIGHT-mrgy2; srcy++) {
					for (srcx = mrgx1; srcx < ZX_VID_X_WIDTH-mrgx2; srcx++) {
						/* Get 8 bit source pixel and convert to RGB */
						colourRGB = chromamode ? cvtChroma(vga_graphmemory[sx++]) : 
									(vga_graphmemory[sx++] == 0) ? colour0RGB : colour1RGB;

						/* Write the destination pixel[s] */
						screen_pixels[desx++] = colourRGB;
					}
					desy += video.xres;
					desx = desy;
					sy += ZX_VID_X_WIDTH;
					sx = sy;
				}				
			}
		}
		if (SDL_MUSTLOCK(video.screen)) SDL_UnlockSurface(video.screen);

		/* Has the user paused the emulator? */
		if (sdl_emulator.paused) {
			/* Draw the shadow */
			dstrect.w = 8 * 8 * video.scale; dstrect.h = 3 * 8 * video.scale;
			dstrect.x = (video.screen->w - dstrect.w) / 2;
			dstrect.y = (video.screen->h - dstrect.h) / 2;
			draw_shadow(dstrect, 170);	/* 66% */
			/* Render the text using a transparent background */
			renderedtext = BMF_RenderText(BMF_FONT_ZX82, " Paused ", colours.emu_bg,
				colours.colour_key);
			dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
			dstrect.x = (video.screen->w - dstrect.w) / 2;
			dstrect.y = (video.screen->h - dstrect.h) / 2;
			/* Blit it to the screen */
			if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			SDL_FreeSurface(renderedtext);
		}

	}

	/* Is the save state dialog being rendered? */
	if (save_state_dialog.state) {
		srcx = save_state_dialog.xoffset;
		srcy = save_state_dialog.yoffset;
		/* Draw the vertical bars */
		dstrect.x = srcx; dstrect.y = srcy + 1 * 8 * video.scale;
		dstrect.w = 0.5 * 8 * video.scale; dstrect.h = 13 * 8 * video.scale;
		for (count = 0; count < 4; count++) {
			if (SDL_FillRect(video.screen, &dstrect, fg_colourRGB) < 0) {
				fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			dstrect.x += 4.5 * 8 * video.scale;
		}
		/* Draw the horizontal bars */
		dstrect.x = srcx; dstrect.y = srcy + 5 * 8 * video.scale;
		dstrect.w = 14 * 8 * video.scale; dstrect.h = 0.5 * 8 * video.scale;
		for (count = 0; count < 3; count++) {
			if (SDL_FillRect(video.screen, &dstrect, fg_colourRGB) < 0) {
				fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			if (count < 2) dstrect.y += 4.5 * 8 * video.scale;
		}
		dstrect.y += 1.5 * 8 * video.scale;
		if (SDL_FillRect(video.screen, &dstrect, fg_colourRGB) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		/* Draw the right-hand and bottom shadows */
		for (count = 0; count < 2; count++) {
			if (count == 0) {
				dstrect.x = srcx + 14 * 8 * video.scale;
				dstrect.y = srcy + 8 * video.scale;
				dstrect.w = 8 * video.scale;
				dstrect.h =  15 * 8 * video.scale;
			} else {
				dstrect.x = srcx + 8 * video.scale;
				dstrect.y = srcy + 16 * 8 * video.scale;
				dstrect.w = 14 * 8 * video.scale; 
				dstrect.h = 8 * video.scale;
			}
			draw_shadow(dstrect, 64);	/* 25% */
		}
		/* Draw the title bar */
		if (save_state_dialog.mode == SSTATE_MODE_SAVE) {
			strcpy(text, " Save State   ");
		} else {
			strcpy(text, " Load State   ");
		}
		renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
		dstrect.x = srcx; dstrect.y = srcy;
		dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
		if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		SDL_FreeSurface(renderedtext);
		/* Draw the controls */
		strcpy(text, "     Exit     ");
		renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
		dstrect.x = srcx; dstrect.y = srcy + 14.5 * 8 * video.scale;
		dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
		if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		SDL_FreeSurface(renderedtext);
		/* Draw the slot backgrounds */
		dstrect.y = srcy + 1 * 8 * video.scale;
		for (ypos = 0; ypos < 3; ypos++) {
			dstrect.x = srcx + 0.5 * 8 * video.scale;
			for (xpos = 0; xpos < 3; xpos++) {
				if (!save_state_dialog.slots[ypos * 3 + xpos]) {
					colourRGB = bg_colourRGB;
				} else {
					colourRGB = fg_colourRGB;
				}
				dstrect.w = 4 * 8 * video.scale; dstrect.h = 4 * 8 * video.scale;
				if (SDL_FillRect(video.screen, &dstrect, colourRGB) < 0) {
					fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
					exit(1);
				}
				dstrect.x += 4.5 * 8 * video.scale;
			}
			dstrect.y += 4.5 * 8 * video.scale;
		}
		/* Draw the slot text */
		for (ypos = 0; ypos < 3; ypos++) {
			for (xpos = 0; xpos < 3; xpos++) {
				dstrect.y = srcy + 1 * 8 * video.scale + ypos * 4.5 * 8 * video.scale;
				for (ybyte = 0; ybyte < 8; ybyte++) {
					dstrect.x = srcx + 0.5 * 8 * video.scale + xpos * 4.5 * 8 * video.scale;
					for (xmask = 0x80; xmask >= 1; xmask >>= 1) {
						if (save_state_dialog_icons[(ypos * 3 + xpos) * 8 + ybyte] & xmask) {
							if (!save_state_dialog.slots[ypos * 3 + xpos]) {
								colourRGB = fg_colourRGB;
							} else {
								colourRGB = bg_colourRGB;
							}
							dstrect.w = 0.5 * 8 * video.scale; dstrect.h = 0.5 * 8 * video.scale;
							if (SDL_FillRect(video.screen, &dstrect, colourRGB) < 0) {
								fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
								exit(1);
							}
						}
						dstrect.x += 0.5 * 8 * video.scale;
					}
					dstrect.y += 0.5 * 8 * video.scale;
				}
			}
		}
	}

	/* Is the load file dialog being rendered? */
	if (load_file_dialog.state) {
		srcx = load_file_dialog.xoffset;
		srcy = load_file_dialog.yoffset;
		/* Draw the background minus the header and nav control lines */
		dstrect.x = srcx; dstrect.y = srcy + 8 * video.scale;
		dstrect.w = 256 * video.scale; dstrect.h = 176 * video.scale;
		if (SDL_FillRect(video.screen, &dstrect, bg_colourRGB) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		/* Draw the header */
		strcpy(text, " Load File                  ");
		renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
		dstrect.x = srcx; dstrect.y = srcy;
		dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
		if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		SDL_FreeSurface(renderedtext);
		/* Write the details */
		sprintf(text, "%i/%i ", load_file_dialog.dirlist_selected + 1,
			load_file_dialog.dirlist_count);
		renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
		dstrect.x = srcx + 32 * 8 * video.scale - renderedtext->w; dstrect.y = srcy;
		dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
		if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		SDL_FreeSurface(renderedtext);
		/* Write the directory list */
		for (count = load_file_dialog.dirlist_top;
			count < load_file_dialog.dirlist_top + 20 && count < load_file_dialog.dirlist_count;
			count++) {
			direntry = load_file_dialog.dirlist + 
				load_file_dialog.dirlist_sizeof * count;
			/* Truncate filenames longer than 30 chars */
			if (strlen(direntry) <= 30) {
				strcpy(text, direntry);
			} else {
				strncpy(text, direntry, 15);
				text[15] = '~'; text[16] = 0;
				strcat(text, direntry + strlen(direntry) - 14);
			}
			/* Render the text highlighting the selected item */
			if (count == load_file_dialog.dirlist_selected) {
				renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
			} else {
				renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, fg_colour, bg_colour);
			}
			dstrect.x = srcx; dstrect.y = srcy + 
				(count - load_file_dialog.dirlist_top + 2) * 8 * video.scale;
			dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
			if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			SDL_FreeSurface(renderedtext);
		}
		/* Draw the controls */
		strcpy(text, "          Load    Exit          ");
		renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, fg_colour, bg_colour);
		dstrect.x = srcx; dstrect.y = srcy + 23 * 8 * video.scale;
		dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
		if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		SDL_FreeSurface(renderedtext);
		/* Draw the vertical scrollbar */
		/* Up button */
		strcpy(text, "\x96");
		renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
		dstrect.x = hotspots[HS_LDFILE_SBUP].hl_x;
		dstrect.y = hotspots[HS_LDFILE_SBUP].hl_y;
		dstrect.w = dstrect.h = 1 * 8 * video.scale;
		if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		SDL_FreeSurface(renderedtext);
		/* Down button */
		strcpy(text, "\x97");
		renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
		dstrect.x = hotspots[HS_LDFILE_SBDOWN].hl_x;
		dstrect.y = hotspots[HS_LDFILE_SBDOWN].hl_y;
		dstrect.w = dstrect.h = 1 * 8 * video.scale;
		if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		SDL_FreeSurface(renderedtext);
		/* PgUp/PgDn i.e. the scrollbar background */
		strcpy(text, "\x88");
		renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, fg_colour, bg_colour);
		dstrect.x = hotspots[HS_LDFILE_SBPGUP].hl_x;
		dstrect.w = dstrect.h = 1 * 8 * video.scale;
		for (count = 0; count < 18; count++) {
			dstrect.y = hotspots[HS_LDFILE_SBPGUP].hl_y + count * 8 * video.scale;
			if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
		}
		SDL_FreeSurface(renderedtext);
		/* Scrollbar handle */
		dstrect.x = hotspots[HS_LDFILE_SBHDLE].hl_x;
		dstrect.y = hotspots[HS_LDFILE_SBHDLE].hl_y;
		dstrect.w = hotspots[HS_LDFILE_SBHDLE].hl_w;
		dstrect.h = hotspots[HS_LDFILE_SBHDLE].hl_h;
		if (SDL_FillRect(video.screen, &dstrect, fg_colourRGB) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		/* Scrollbar handle middle */
		strcpy(text, "=");
		renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
		dstrect.x = hotspots[HS_LDFILE_SBHDLE].hl_x;
		dstrect.y = hotspots[HS_LDFILE_SBHDLE].hl_y + 
			hotspots[HS_LDFILE_SBHDLE].hl_h / 2 - 0.5 * 8 * video.scale;
		dstrect.w = dstrect.h = 1 * 8 * video.scale;
		if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		SDL_FreeSurface(renderedtext);
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
	if (runtime_options_which() < MAX_RUNTIME_OPTIONS) {
		srcx = runtime_options[runtime_options_which()].xoffset;
		srcy = runtime_options[runtime_options_which()].yoffset;
		/* Draw the background minus the header and nav control lines */
		dstrect.x = srcx; dstrect.y = srcy + 8 * video.scale;
		dstrect.w = 256 * video.scale; dstrect.h = 176 * video.scale;
		if (SDL_FillRect(video.screen, &dstrect, bg_colourRGB) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		/* Draw the static text */
		invertcolours = FALSE;
		for (desy = 0; desy < 24; desy++) {
			dstrect.x = srcx; desx = 0;
			for (;;) {
				do {
					text[0] = runtime_options[runtime_options_which()].text[desy][desx++];
					/* Invert the colours */
					if (text[0] == 2) invertcolours = !invertcolours;
				} while (text[0] == 1 || text[0] == 2);
				if (text[0]) {
					text[1] = 0;	/* Null terminate the string */
					if (!invertcolours) {
						renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, fg_colour, bg_colour);
					} else {
						renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
					}
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
					text[0] = runtime_options[runtime_options_which()].text[desy][desx++];
					/* Invert the colours */
					if (text[0] == 2) invertcolours = !invertcolours;
				} while (text[0] == 2);
				if (text[0] == 1) {
					text[0] = 0;	/* Erase it as we might put a value in it */
					if (runtime_options[0].state) {
						/* The colour inversion here is conditional and so there are
						 * two \x1's embedded within the text. The first \x1 will
						 * always draw a char which may be colour inverted, and the
						 * second \x1 will revert the colour if it was inverted */
						if (count >= 0 && count <= 3) {
							if (count == 0 || count == 2) strcpy(text, "O");
							if ((count <= 1 && runopts_emulator_model) || 
								(count >= 2 && !runopts_emulator_model)) {
								/* Invert the colours */
								invertcolours = !invertcolours;
							}
						} else if (count == 4) {
							sprintf(text, "%2i", runopts_emulator_ramsize);
						} else if (count >= 5 && count <= 8) {
							if (count == 5 || count == 7) strcpy(text, "O");
							if ((count <= 6 && !runopts_emulator_m1not) || 
								(count >= 7 && runopts_emulator_m1not)) {
								/* Invert the colours */
								invertcolours = !invertcolours;
							}
						} else if (count == 9) {
							sprintf(text, "%1i", sdl_emulator.frameskip);
					#ifdef ENABLE_EMULATION_SPEED_ADJUST
						} else if (count == 10) {
							sprintf(text, "%4i", 2000 / runopts_emulator_speed);
						} else if (count == 19) {
					#else
						} else if (count == 18) {
					#endif
							if (runopts_is_a_reset_scheduled())
								strcpy(text, "* A reset is scheduled on save *");
						}
// WRX and UDG
					#ifdef ENABLE_EMULATION_SPEED_ADJUST
						if (count >= 11 && count<=18) {
							if (count >= 11 && count <= 12 && runopts_emulator_wrx != HIRESWRX)
								invertcolours = !invertcolours;
							if (count >= 13 && count <= 14 && runopts_emulator_wrx == HIRESWRX)
								invertcolours = !invertcolours;
							if (count >= 15 && count <= 16 && runopts_emulator_chrgen!=CHRGENCHR16 )
								invertcolours = !invertcolours;
							if (count >= 17 && count <=18 && runopts_emulator_chrgen==CHRGENCHR16 )
								invertcolours = !invertcolours;
							if (count==11 || count==13 || count==15 || count==17)
								strcpy(text, "O");
						}
					#else
						if (count >= 10 && count<=17) {
							if (count >= 10 && count <= 11 && runopts_emulator_wrx != HIRESWRX)
								invertcolours = !invertcolours;
							if (count >= 12 && count <= 13 && runopts_emulator_wrx == HIRESWRX)
								invertcolours = !invertcolours;
							if (count >= 14 && count <= 15 && runopts_emulator_chrgen!=CHRGENCHR16 )
								invertcolours = !invertcolours;
							if (count >= 16 && count <=17 && runopts_emulator_chrgen==CHRGENCHR16 )
								invertcolours = !invertcolours;
							if (count==10 || count==12 || count==14 || count==16)
								strcpy(text, "O");
						}
					#endif
					} else if (runtime_options[1].state) {
						/* The colour inversion here is conditional and so there are
						 * two \x1's embedded within the text. The first \x1 will
						 * always draw a char which may be colour inverted, and the
						 * second \x1 will revert the colour if it was inverted */
						if (count >= 0 && count <= 1) {
							if (!(count % 2)) strcpy(text, "O");
							if (runopts_sound_device == DEVICE_NONE) {
								/* Invert the colours */
								invertcolours = !invertcolours;
							}
						} else if (count >= 2 && count <= 3) {
							if (!(count % 2)) strcpy(text, "O");
							if (runopts_sound_device == DEVICE_QUICKSILVA ||
								runopts_sound_device == DEVICE_ZONX) {
								/* Invert the colours */
								invertcolours = !invertcolours;
							}
						} else if (count >= 4 && count <= 5) {
							if (!(count % 2)) strcpy(text, "O");
							if (runopts_sound_device == DEVICE_QUICKSILVA) {
								/* Invert the colours */
								invertcolours = !invertcolours;
							}
						} else if (count >= 6 && count <= 7) {
							if (!(count % 2)) strcpy(text, "O");
							if (runopts_sound_device == DEVICE_ZONX) {
								/* Invert the colours */
								invertcolours = !invertcolours;
							}
						} else if (count >= 8 && count <= 9) {
							if (!(count % 2)) strcpy(text, "X");
							if (runopts_sound_ay_unreal) {
								/* Invert the colours */
								invertcolours = !invertcolours;
							}
						} else if (count >= 10 && count <= 11) {
							if (!(count % 2)) strcpy(text, "O");
							if (runopts_sound_device == DEVICE_VSYNC) {
								/* Invert the colours */
								invertcolours = !invertcolours;
							}
						} else if (count == 12) {
							#ifdef OSS_SOUND_SUPPORT
								if (runopts_is_a_reset_scheduled())
									strcpy(text, "* A reset is scheduled on save *");
							#else
								strcpy(text, "* Sound support isn't built in *");
							#endif
						}
					} else if (runtime_options[2].state) {
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
						} else if (count == 9) {
							if (runopts_is_a_reset_scheduled())
								strcpy(text, "* A reset is scheduled on save *");
						}
					} else if (runtime_options[3].state) {
						if (count == 0) {
							sprintf(text, "%2i", joystick_dead_zone);
						} else if (count >= 1 && count <= 2) {		
							sprintf(text, "%s", joy_cfg.text[count - 1]);
						} else if (count == 3) {
							if (!joystick) {
								strcpy(text, "* A joystick is not plugged in *");
							} else if (runopts_is_a_reset_scheduled()) {
								strcpy(text, "* A reset is scheduled on save *");
							}
						}
					}
					if (text[0]) {	/* Let's not go to that place - again ;) */
						dstrect.y = srcy + desy * 8 * video.scale;
						if (!invertcolours) {
							renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, fg_colour, bg_colour);
						} else {
							renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
						}
						dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
						if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
							fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
							exit(1);
						}
						SDL_FreeSurface(renderedtext);
					}
					count++; desx++;
				} else if (text[0] == 0) {
					break;	/* Process next row */
				}
				dstrect.x += 8 * video.scale;
			}
		}
	}

	/* Is the dialog being rendered? */
	if (dialog.state) {
		srcx = dialog.xoffset;
		srcy = dialog.yoffset;
		/* Draw the title and navigation bars */
		dstrect.x = srcx; dstrect.y = srcy;
		dstrect.w = dialog.width - 1 * 8 * video.scale;
		dstrect.h = 1 * 8 * video.scale;
		for (count = 0; count < 2; count++) {
			if (SDL_FillRect(video.screen, &dstrect, fg_colourRGB) < 0) {
				fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			dstrect.y += dialog.height - 3 * 8 * video.scale;
			dstrect.h += 1 * 8 * video.scale;
		}
		/* Draw the interior */
		dstrect.x = srcx + 0.5 * 8 * video.scale;
		dstrect.y = srcy + 1 * 8 * video.scale;
		dstrect.w = dialog.width - 2 * 8 * video.scale;
		dstrect.h = dialog.height - 4 * 8 * video.scale;
		if (SDL_FillRect(video.screen, &dstrect, bg_colourRGB) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		/* Draw the outer vertical bars */
		dstrect.x = srcx; dstrect.y = srcy + 1 * 8 * video.scale;
		dstrect.w = 0.5 * 8 * video.scale;
		dstrect.h = dialog.height - 4 * 8 * video.scale;
		for (count = 0; count < 2; count++) {
			if (SDL_FillRect(video.screen, &dstrect, fg_colourRGB) < 0) {
				fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			dstrect.x += dialog.width - 1.5 * 8 * video.scale;
		}
		/* Draw the right and bottom icon bars */
		dstrect.x = srcx + 4.5 * 8 * video.scale;
		dstrect.y = srcy + 1 * 8 * video.scale; dstrect.w = 0.5 * 8 * video.scale;
		dstrect.h = 4 * 8 * video.scale;
		if (SDL_FillRect(video.screen, &dstrect, fg_colourRGB) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		dstrect.x = srcx + 0.5 * 8 * video.scale;
		dstrect.y = srcy + 5 * 8 * video.scale;
		dstrect.w = 4.5 * 8 * video.scale;
		/* dstrect.h = 0.5 * 8 * video.scale;	Thin */
		dstrect.h = dialog.height - 8 * 8 * video.scale;	/* Full */
		if (SDL_FillRect(video.screen, &dstrect, fg_colourRGB) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		/* Draw the icon */
		if (dialog.flags & DIALOG_ICON_EXCLAMATION) {
			offset = 0 * 8;
		} else if (dialog.flags & DIALOG_ICON_INFORMATION) {
			offset = 1 * 8;
		} else if (dialog.flags & DIALOG_ICON_QUESTION) {
			offset = 2 * 8;
		} else /*if (dialog.flags & DIALOG_ICON_STOP)*/ {
			offset = 3 * 8;
		}
		dstrect.y = srcy + 1 * 8 * video.scale;
		for (ybyte = 0; ybyte < 8; ybyte++) {
			dstrect.x = srcx + 0.5 * 8 * video.scale;
			for (xmask = 0x80; xmask >= 1; xmask >>= 1) {
				if (dialog_icons[offset + ybyte] & xmask) {
					dstrect.w = dstrect.h = 0.5 * 8 * video.scale;
					if (SDL_FillRect(video.screen, &dstrect, fg_colourRGB) < 0) {
						fprintf(stderr, "%s: FillRect error: %s\n", __func__,
							SDL_GetError ());
						exit(1);
					}
				}
				dstrect.x += 0.5 * 8 * video.scale;
			}
			dstrect.y += 0.5 * 8 * video.scale;
		}
		/* Write the title */
		strcpy(text, dialog.title);
		renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
		dstrect.x = srcx + (dialog.width - 1 * 8 * video.scale) / 2 - 
			renderedtext->w / 2; dstrect.y = srcy;
		dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
		if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			exit(1);
		}
		SDL_FreeSurface(renderedtext);
		/* Write the text */
		dstrect.x = srcx + 6 * 8 * video.scale;
		dstrect.y = srcy + 2 * 8 * video.scale;
		for (count = 0; count < MAX_DIALOG_ROWS; count++) {
			if (dialog.text[count] != NULL) {
				renderedtext = BMF_RenderText(BMF_FONT_ZX82, dialog.text[count],
					fg_colour, bg_colour);
				dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
				if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
					fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, 
						SDL_GetError ());
					exit(1);
				}
				SDL_FreeSurface(renderedtext);
				dstrect.y += 1 * 8 * video.scale;
			} else {
				break;
			}
		}
		/* Draw the controls */
		for (count = 0; count < 3; count++) {
			if (count == 0) {
				if ((dialog.flags & DIALOG_BUTTONS_YES_NO) || 
					(dialog.flags & DIALOG_BUTTONS_YES_NO_CANCEL)) {
					strcpy(text, "Yes");
				} else if (dialog.flags & DIALOG_BUTTONS_OK_CANCEL) {
					strcpy(text, "OK");
				}
			} else if (count == 1) {
				if ((dialog.flags & DIALOG_BUTTONS_YES_NO) || 
					(dialog.flags & DIALOG_BUTTONS_YES_NO_CANCEL)) {
					strcpy(text, "No");
				} else if (dialog.flags & DIALOG_BUTTONS_OK_CANCEL) {
					strcpy(text, "Cancel");
				}
			} else if (count == 2) {
				if (dialog.flags & DIALOG_BUTTONS_YES_NO_CANCEL) {
					strcpy(text, "Cancel");
				} else {
					break;
				}
			}
			renderedtext = BMF_RenderText(BMF_FONT_ZX82, text, bg_colour, fg_colour);
			dstrect.x = hotspots[HS_DIALOG_BUTTON0 + count].hl_x;
			dstrect.y = hotspots[HS_DIALOG_BUTTON0 + count].hl_y;
			dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
			if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			SDL_FreeSurface(renderedtext);
		}
		/* Draw the right-hand and bottom shadows */
		for (count = 0; count < 2; count++) {
			if (count == 0) {
				dstrect.x = srcx + dialog.width - 1 * 8 * video.scale;
				dstrect.y = srcy + 1 * 8 * video.scale;
				dstrect.w = 1 * 8 * video.scale;
				dstrect.h =  dialog.height - 1 * 8 * video.scale;
			} else {
				dstrect.x = srcx + 1 * 8 * video.scale;
				dstrect.y = srcy + dialog.height - 1 * 8 * video.scale;
				dstrect.w = dialog.width - 2 * 8 * video.scale;
				dstrect.h = 1 * 8 * video.scale;
			}
			draw_shadow(dstrect, 64);	/* 25% */
		}
	}

	/* Render visible hotspots now */
	hotspots_render();

	/* Render an existing notification */
	notification_show(NOTIFICATION_SHOW, NULL);

	/* If the user wants to see the input ids then show
	 * the currently pressed control id on-screen */
	if (show_input_id && current_input_id != UNDEFINED) {
		sprintf(text, "%i", current_input_id);
		if (*sdl_emulator.model == MODEL_ZX80) {
			renderedtext = BMF_RenderText(BMF_FONT_ZX80, text, bg_colour, fg_colour);
		} else {
			renderedtext = BMF_RenderText(BMF_FONT_ZX81, text, bg_colour, fg_colour);
		}
		dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
		dstrect.x = (video.screen->w - dstrect.w) / 2;
		dstrect.y = (video.screen->h - dstrect.h) / 2;
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
 * Draw Shadow                                                             *
 ***************************************************************************/

void draw_shadow(SDL_Rect dstrect, int alpha) {
	SDL_Surface *shadow;

	/* Create an RGB surface to accommodate the shadow */
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
	if (SDL_SetAlpha(shadow, SDL_SRCALPHA, alpha) < 0) {
		fprintf(stderr, "%s: Cannot set surface alpha: %s\n", __func__,
			SDL_GetError());
		exit(1);
	}
	/* Blit it to the screen */
	if (SDL_BlitSurface (shadow, NULL, video.screen, &dstrect) < 0) {
		fprintf(stderr, "%s: BlitSurface error: %s\n", __func__,
			SDL_GetError ());
		exit(1);
	}
	SDL_FreeSurface(shadow);
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
		/* used to be (<2.2.0) 960x720 | 640x480 | 320x240 */
		if (video.bigscreen) {
			if (video.xres == 1200) {
				video.xres = 800; video.yres = 600; video.scale = 2;
			} else if (video.xres == 800) {
				video.xres = 400; video.yres = 300; video.scale = 1;
			} else {
				video.xres = 1200; video.yres = 900; video.scale = 3;
			}
		} else {
			if (video.xres == 960) {
				video.xres = 640; video.yres = 480; video.scale = 2;
			} else if (video.xres == 640) {
				video.xres = 320; video.yres = 240; video.scale = 1;
			} else {
				video.xres = 960; video.yres = 720; video.scale = 3;
			}
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
 * Set Joystick Configurator Text                                          *
 ***************************************************************************/
/* This sets the text shown underneath the joypad in runopts3. It does all
 * the checking for joystick present, runopts3 visible etc. so just call it.
 * 
 * On entry: textid = JOY_CFG_TEXT_DEFAULT_SETTINGS
 *           textid = JOY_CFG_TEXT_PRESS_SOMETHING
 *           textid = JOY_CFG_TEXT_ACCEPTED
 *           textid = JOY_CFG_TEXT_CANCELLED
 */

void set_joy_cfg_text(int textid) {
	int hs_selected;
	
	if (runtime_options[3].state) {	/* No point if you can't see it */
		if (joystick) {
			/* Locate currently selected hotspot for group RUNOPTS3 */
			hs_selected = get_selected_hotspot(HS_GRP_RUNOPTS3);
			/* Is one of the joycfg hotspots selected? */
			if (hs_selected >= HS_RUNOPTS3_JOY_CFG_LTRIG &&
				hs_selected <= HS_RUNOPTS3_JOY_CFG_X) {
				if (textid == JOY_CFG_TEXT_DEFAULT_SETTINGS) {
					strcpy(joy_cfg.text[0], "This control defaults to");
					if (hs_selected == HS_RUNOPTS3_JOY_CFG_LTRIG) {
						strcpy(joy_cfg.text[1], "Shift/Page Up");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_RTRIG) {
						strcpy(joy_cfg.text[1], "Page Down");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_LEFT) {
						strcpy(joy_cfg.text[1], "O/Selector Left");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_RIGHT) {
						strcpy(joy_cfg.text[1], "P/Selector Right");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_UP) {
						strcpy(joy_cfg.text[1], "Q/Selector Up");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_DOWN) {
						strcpy(joy_cfg.text[1], "A/Selector Down");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_SELECT) {
						strcpy(joy_cfg.text[1], "Options/Control Remapper");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_START) {
						strcpy(joy_cfg.text[1], "Toggle Virtual Keyboard");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_A) {
						strcpy(joy_cfg.text[1], "Newline/Selector Hit");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_B) {
						strcpy(joy_cfg.text[1], "Newline");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_Y) {
						strcpy(joy_cfg.text[1], "Rubout");
					} else if (hs_selected == HS_RUNOPTS3_JOY_CFG_X) {
						strcpy(joy_cfg.text[1], "Space");
					}
				} else if (textid == JOY_CFG_TEXT_PRESS_SOMETHING) {
					set_joy_cfg_text(JOY_CFG_TEXT_DEFAULT_SETTINGS);
					strcpy(joy_cfg.text[0], "Press a similar control for");
				} else if (textid == JOY_CFG_TEXT_ACCEPTED) {
					strcpy(joy_cfg.text[0], "Accepted. Commit changes with");
					strcpy(joy_cfg.text[1], "Save when you have finished.");
				} else if (textid == JOY_CFG_TEXT_CANCELLED) {
					strcpy(joy_cfg.text[0], "Cancelled. A joystick control");
					strcpy(joy_cfg.text[1], "was expected but not found.");
				}
			} else {
				strcpy(joy_cfg.text[0], "");
				strcpy(joy_cfg.text[1], "");
			}
		} else {
			strcpy(joy_cfg.text[0], "");
			strcpy(joy_cfg.text[1], "");
		}
	}
}

/***************************************************************************
 * Save Screenshot                                                         *
 ***************************************************************************/

void save_screenshot(void) {
	char fullpath[256], filename[256];
	int nextnum;

	#if defined(PLATFORM_GP2X) || defined(__amigaos4__) || defined(_WIN32)
		strcpy(fullpath, LOCAL_DATA_DIR);
	#else
		strcpy(fullpath, getenv ("HOME"));
		strcatdelimiter(fullpath);
		strcat(fullpath, LOCAL_DATA_DIR);
	#endif
	strcatdelimiter(fullpath);
	strcat(fullpath, LOCAL_SCNSHT_DIR);
	strcatdelimiter(fullpath);

	/* Create a unique filename using the next highest number
	 * (it'll return 0 if the directory couldn't be opened or 1 as
	 * the base number when no files exist that match the pattern) */
	nextnum = get_filename_next_highest(fullpath, "scnsht%4d");
	sprintf(filename, "scnsht%04i.bmp", nextnum);
	strcat(fullpath, filename);

	fprintf(stderr,"screenshot filename: %s\n", fullpath);

	if (SDL_SaveBMP(video.screen, fullpath) < 0) {
		fprintf(stderr, "%s: Cannot save screenshot: %s\n", __func__,
			SDL_GetError());
	}
}

