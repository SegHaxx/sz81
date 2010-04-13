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

/* This is how the SDL part of sz81 is organised :-
 * 
 *      sdl.h <-- sdl_engine.h --> sdl_*.h
 *        ^             ^
 *       /              |
 *   smain.c         sdl_*.c
 */ 

/* Includes */
#include <SDL/SDL.h>

/* Defines */
#define SOUND_BUFFER_SIZE (1024 * 16)

/* SVGAlib keyboard scancodes (smain.c currently uses these) */
#define SCANCODE_ESCAPE 1
#define SCANCODE_MINUS 12
#define SCANCODE_EQUAL 13
#define SCANCODE_BRACKET_LEFT 26
#define SCANCODE_BRACKET_RIGHT 27
#define SCANCODE_F1 59
#define SCANCODE_F2 60
#define SCANCODE_F3 61
#define SCANCODE_F4 62
#define SCANCODE_F5 63
#define SCANCODE_F6 64
#define SCANCODE_F7 65
#define SCANCODE_F8 66
#define SCANCODE_F9 67
#define SCANCODE_F10 68
#define SCANCODE_F11 87
#define SCANCODE_F12 88
#define SCANCODE_HOME 102
#define SCANCODE_PAGEUP 104
#define SCANCODE_END 107
#define SCANCODE_PAGEDOWN 109
#define SCANCODE_INSERT 110
#define SCANCODE_REMOVE 111
#define SCANCODE_LEFTSHIFT 42
#define SCANCODE_RIGHTSHIFT 54
#define SCANCODE_LEFTCONTROL 29
#define SCANCODE_RIGHTCONTROL 97
#define SCANCODE_Z 44
#define SCANCODE_X 45
#define SCANCODE_C 46
#define SCANCODE_V 47
#define SCANCODE_A 30
#define SCANCODE_S 31
#define SCANCODE_D 32
#define SCANCODE_F 33
#define SCANCODE_G 34
#define SCANCODE_Q 16
#define SCANCODE_W 17
#define SCANCODE_E 18
#define SCANCODE_R 19
#define SCANCODE_T 20
#define SCANCODE_1 2
#define SCANCODE_2 3
#define SCANCODE_3 4
#define SCANCODE_4 5
#define SCANCODE_5 6
#define SCANCODE_0 11
#define SCANCODE_9 10
#define SCANCODE_8 9
#define SCANCODE_7 8
#define SCANCODE_6 7
#define SCANCODE_P 25
#define SCANCODE_O 24
#define SCANCODE_I 23
#define SCANCODE_U 22
#define SCANCODE_Y 21
#define SCANCODE_ENTER 28
#define SCANCODE_L 38
#define SCANCODE_K 37
#define SCANCODE_J 36
#define SCANCODE_H 35
#define SCANCODE_SPACE 57
#define SCANCODE_PERIOD 52
#define SCANCODE_M 50
#define SCANCODE_N 49
#define SCANCODE_B 48

/* Variables */
int sdl_cl_show_input_id;

struct {
	int state;
	int xoffset;
	int yoffset;
	SDL_TimerID timer_id;
	int speed;
} emulator;

struct {
	int state;
	int volume;
	Uint8 buffer[SOUND_BUFFER_SIZE];
	int buffer_start;
	int buffer_end;
} sdl_sound;

struct {
	int delay;
	int interval;
} sdl_key_repeat;

/* Function prototypes */
int sdl_init(void);
int sdl_video_setmode(void);
unsigned char *vga_getgraphmem(void);
int keyboard_init(void);
void sdl_rcfile_read(void);
void sdl_timer_init(void);
char *keyboard_getstate(void);
int keyboard_update(void);
void sdl_video_update(void);
int sdl_sound_init(int freq, int *stereo, int *sixteenbit);
void sdl_sound_callback(void *userdata, Uint8 *stream, int len);
void sdl_sound_frame(unsigned char *data, int len);
void sdl_sound_end(void);




