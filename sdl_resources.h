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
#define MAX_FONTS 8

/* Bitmap font IDs */
#define BMF_FONT_ZX80 0
#define BMF_FONT_ZX81 1
#define BMF_FONT_ZX82 2

#define IMG_WM_ICON "sz81.bmp"
#define IMG_ZX80_KYBD "zx80kybd.bmp"
#define IMG_ZX81_KYBD "zx81kybd.bmp"
#define IMG_ZX80_FONT "zx80font.bmp"
#define IMG_ZX81_FONT "zx81font.bmp"
#define IMG_ZX82_FONT "zx82font.bmp"
#define IMG_SZ81_ICONS "sz81icons.bmp"

/* Variables */
SDL_Surface *wm_icon;

struct {
	int rewrite;
} rcfile;

struct colourtable {
	Uint32 colour_key;
	Uint32 bmf_fg_default;
	Uint32 emu_fg;
	Uint32 emu_bg;
	Uint32 hs_load_selected;
	Uint32 hs_load_pressed;
	Uint32 hs_vkeyb_zx80_selected;
	Uint32 hs_vkeyb_zx80_pressed;
	Uint32 hs_vkeyb_zx80_toggle_pressed;
	Uint32 hs_vkeyb_zx81_selected;
	Uint32 hs_vkeyb_zx81_pressed;
	Uint32 hs_vkeyb_zx81_toggle_pressed;
	Uint32 hs_ctb_selected;
	Uint32 hs_ctb_pressed;
};
struct colourtable colours;

struct bmpfont {
	SDL_Surface *original;
	SDL_Surface *scaled[MAX_FONTS];
	Uint32 fg_colour[MAX_FONTS];
};
struct bmpfont zx80font, zx81font, zx82font;	

struct {
	SDL_Surface *original;
	SDL_Surface *scaled;
} sz81icons;

struct {
	SDL_Surface *original;
	SDL_Surface *scaled;
	int state;
	int xoffset;
	int yoffset;
	int alpha;
	int autohide;		/* TRUE to hide on newline */
	int toggle_shift;	/* TRUE for toggle, FALSE for sticky */
} vkeyb;

struct {
	SDL_Surface *scaled;
	int xoffset;
	int yoffset;
} control_bar;

/* Function prototypes */
void rcfile_write(void);
int fonts_init(void);
int vkeyb_init(void);
int sz81icons_init(void);
int control_bar_init(void);


