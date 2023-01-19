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
#define MAX_FONTS 4
#define MAX_DIALOG_ROWS 19

/* Local resources */
#if defined(__amigaos4__)
	#define LOCAL_DATA_DIR amiga_data_dir
#elif defined(PLATFORM_GP2X) || defined(_WIN32)
	#define LOCAL_DATA_DIR "local"
#else
	#define LOCAL_DATA_DIR ".sz81"
#endif
#if defined(_WIN32)
	#define RESOURCE_FILE "sz81.ini"
#else
	#define RESOURCE_FILE "sz81rc"
#endif
#define LOCAL_PRTOUT_DIR "prtout"
#define LOCAL_SAVSTA_DIR "savsta"
#define LOCAL_SCNSHT_DIR "scnsht"
#define LOCAL_PROGRM_DIR "progrm"

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

#define ROM_ZX80 "zx80.rom"
#ifdef ZXPAND
#define ROM_ZX81 "zx81.zxpand.ovl"
#else
#ifdef ZXNU
#define ROM_ZX81 "zxnu.rom"
#else
#define ROM_ZX81 "zx81.rom"
#endif
#endif 
#define ASZMIC   "aszmic.rom"

/* Notification function IDs */
#define NOTIFICATION_SHOW 1
#define NOTIFICATION_KILL 2

/* Notification timeouts in ms */
#define NOTIFICATION_TIMEOUT_1250 1250
#define NOTIFICATION_TIMEOUT_750 750

/* Dialog properties */
#define DIALOG_ICON_EXCLAMATION 1
#define DIALOG_ICON_INFORMATION 2
#define DIALOG_ICON_QUESTION 4
#define DIALOG_ICON_STOP 8
#define DIALOG_BUTTONS_YES_NO 16
#define DIALOG_BUTTONS_YES_NO_CANCEL 32
#define DIALOG_BUTTONS_OK_CANCEL 64
#define DIALOG_DEFAULT_YES 128
#define DIALOG_DEFAULT_OK 256
#define DIALOG_DEFAULT_NO 512
#define DIALOG_DEFAULT_CANCEL 1024


/* Variables */
extern SDL_Surface *wm_icon;

struct Notification {
	char title[31];		/* The title bar text */
	char text[31];		/* A single line message */
	int timeout;		/* In ms */
};

typedef struct {
	int state;
	int flags;			/* An OR'd combination of DIALOG_ properties */
	int retval;			/* The remap_id of the button pressed */
	int xoffset;
	int yoffset;
	int width;
	int height;
	char *title;					/* The title bar text */
	char *text[MAX_DIALOG_ROWS];	/* A multi-line message */
} dialog_;

typedef struct {
	char filename[256];
	int rewrite;
} rcfile_;

typedef struct colourtable {
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
	Uint32 hs_options_selected;
	Uint32 hs_options_pressed;
} colourtable_;

typedef struct bmpfont {
	SDL_Surface *original;
	SDL_Surface *scaled[MAX_FONTS];
	Uint32 fg_colour[MAX_FONTS];
	Uint32 requested[MAX_FONTS];
} bmpfont_;

typedef struct {
	SDL_Surface *original;
	SDL_Surface *scaled;
} sz81icons_;

typedef struct {
	SDL_Surface *zx80original;
	SDL_Surface *zx81original;
	SDL_Surface *scaled;
	int state;
	int xoffset;
	int yoffset;
	int alpha;
	int autohide;		/* TRUE to hide on newline */
	int toggle_shift;	/* TRUE for toggle, FALSE for sticky */
} vkeyb_;

typedef struct {
	SDL_Surface *scaled;
	int xoffset;
	int yoffset;
} control_bar_;

extern dialog_ dialog;
extern rcfile_ rcfile;
extern colourtable_ colours;
extern bmpfont_ zx80font, zx81font, zx82font;	
extern sz81icons_ sz81icons;
extern vkeyb_ vkeyb;
extern control_bar_ control_bar;

/* Function prototypes */
void local_data_dir_init(void);
void rcfile_write(void);
int fonts_init(void);
int vkeyb_init(void);
int vkeyb_alpha_apply(void);
int sz81icons_init(void);
int control_bar_init(void);
void notification_show(int funcid, struct Notification *notification);






