#ifndef _SDL_H_
#define _SDL_H_

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

/* This is how the SDL part of sz81 is organised :-
 * 
 *         sdl.h <----- sdl_engine.h ---> sdl_*.h
 *         ^   ^              ^
 *        /     \             |
 *  common.c   sdl_main.c   sdl_*.c
 *   sound.c
 *     z80.c
 */

/* Includes */
#include <SDL/SDL.h>

/* Defines */
#define MAX_KEYCODES 358	/* SDL stops at 322 and then I extend them */

/* Interrupt types */
#define INTERRUPT_EMULATOR_RESET 3
#define INTERRUPT_EMULATOR_EXIT 4

/* Machine models */
#define MODEL_ZX81 0
#define MODEL_ZX80 1

/* Load file methods */
#define LOAD_FILE_METHOD_NONE 0
#define LOAD_FILE_METHOD_DETECT 1
#define LOAD_FILE_METHOD_AUTOLOAD 2
#define LOAD_FILE_METHOD_FORCEDLOAD 3
#define LOAD_FILE_METHOD_NAMEDLOAD 4
#define LOAD_FILE_METHOD_SELECTLOAD 5
#define LOAD_FILE_METHOD_SELECTLOADOK 6
#define LOAD_FILE_METHOD_STATELOAD 7

/* Save file methods */
#define SAVE_FILE_METHOD_NAMEDSAVE 1
#define SAVE_FILE_METHOD_UNNAMEDSAVE 2
#define SAVE_FILE_METHOD_STATESAVE 3

/* 16KB was fine for everything but the Wiz is currently experiencing
 * linear buffer overflow and so I'm quadrupling it for the Wiz only */
#if defined(PLATFORM_GP2X) && defined (TOOLCHAIN_OPENWIZ)
	#define SOUND_BUFFER_SIZE (1024 * 8 * 4)
#else
        #define SOUND_BUFFER_SIZE (1024 * 8)
#endif

/* Variables */
extern int keyboard_buffer[MAX_KEYCODES];

typedef struct {
	int nxtlin;
	int load_hook;
	int save_hook;
	int rsz81mem;
	int wsz81mem;
	int bigscreen;
	int fullscreen;
	int networking;
	int scale;
	int xres;
	int yres;
	int bdis;
	int edis;
	char filename[256];
} sdl_com_line_;

typedef struct {
	int state;		/* FALSE=video output/keyboard input disabled, TRUE=all active */
	int paused;		/* Via Pause key: TRUE=emulation on-hold, keyboard input disabled */
	int xoffset;
	int yoffset;
	SDL_TimerID timer_id;
	int m1not;
	int wrx;
	int chrgen;
	int speed;		/* 5ms=400%, 10ms=200%, 20ms=100%, 30ms=66%, 40ms=50% */
	int frameskip;	        /* 0 to MAX_FRAMESKIP */
	int *model;		/* Points to z81's zx80: 0=ZX81, 1=ZX80 */
	int ramsize;	        /* 1, 2, 3, 4, 16, 32, 48 or 56 */
	int invert;		/* This should really be in video but it's easier to put it here */
	int autoload;	        /* Set to TRUE when auto-loading or forced-loading */
	int networking;         /* enable calls to WIZ chip emulation */
	int bdis;
	int edis;
} sdl_emulator_;

typedef struct {
	int state;
	int volume;
	int device;		/* See DEVICE* defines in sdl_sound.h */
	int stereo;
	int ay_unreal;
	Uint16 buffer[SOUND_BUFFER_SIZE];
	int buffer_start;
	int buffer_end;
} sdl_sound_;

typedef struct {
	int state;
	unsigned char data[64 * 1024];
} sdl_zx80rom_;

typedef struct {
	int state;
	unsigned char data[64 * 1024];
} sdl_zx81rom_;

typedef struct {
	int state;
	unsigned char data[4 * 1024];
} sdl_aszmicrom_;

typedef struct keyrepeat {
	int delay;
	int interval;
} keyrepeat_;

extern sdl_com_line_ sdl_com_line;
extern sdl_emulator_ sdl_emulator;
extern sdl_sound_ sdl_sound;
extern sdl_zx80rom_ sdl_zx80rom;
extern sdl_zx81rom_ sdl_zx81rom;
extern sdl_aszmicrom_ sdl_aszmicrom;
extern keyrepeat_ sdl_key_repeat;

/* Function prototypes */
int sdl_init(void);
int sdl_com_line_process(int argc, char *argv[]);
int sdl_video_setmode(void);
unsigned char *vga_getgraphmem(void);
void sdl_keyboard_init(void);
void sdl_hotspots_init(void);
void sdl_rcfile_read(void);
int sdl_zxroms_init(void);
void sdl_component_executive(void);
void sdl_timer_init(void);
void sdl_zxprinter_init(void);
int keyboard_update(void);
void sdl_video_update(void);
int sdl_sound_init(int freq, int *stereo, int *sixteenbit);
void sdl_sound_callback(void *userdata, Uint8 *stream, int len);
void sdl_sound_frame(Uint16 *data, int len);
void sdl_sound_end(void);
int sdl_filetype_casecmp(char *filename, char *filetype);
int sdl_load_file(int parameter, int method);
int sdl_save_file(int parameter, int method);

#endif
