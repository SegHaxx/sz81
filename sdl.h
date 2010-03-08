/* sz81 Copyright (C) 2007-2010 Thunor <thunorsif@hotmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

/* My defines */
#define MAX_SCANCODES 128


/* Defines relevant to SVGAlib's vga */
#define TEXTMODE 0
#define G320x200x256 5


/* Defines relevant to SVGAlib's vgakeyboard */
#define DONT_CATCH_CTRLC 8
#define KEY_NOTPRESSED 0
#define KEY_PRESSED 1
#define SCANCODE_ESCAPE 1
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
#define SCANCODE_END 107
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


/* Variables relevant to SVGAlib's vga */
unsigned char vga_graphmemory[64 * 1024];


/* Variables relevant to SVGAlib's vgakeyboard */
char keyboard_buffer[MAX_SCANCODES];


/* Functions relevant to SVGAlib's vga */
int vga_init(void);
int vga_setmode(int mode);
unsigned char *vga_getgraphmem(void);


/* Functions relevant to SVGAlib's vgakeyboard */
int keyboard_init(void);
void keyboard_close(void);
char *keyboard_getstate(void);
int keyboard_update(void);
int keyboard_keypressed(int scancode);
void keyboard_translatekeys(int mask);


/* My functions */
void sdl_video_refresh(void);
void sdl_delay(void);









