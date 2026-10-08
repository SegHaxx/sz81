/* sz81 browser host. Copyright (C) 2026 Monte Ohrt <monte@ohrt.com>
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
 *
 * host.c - runs the sz81 ZX81 core in a browser via Emscripten, set up
 * as a Timex Sinclair 1000, in place of the SDL front end.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "sdl.h"
#include "sdl_loadsave.h"
#include "sdl_sound.h"
#include "sound.h"
#include "common.h"
#include "zx81.h"
#include "zx81config.h"
#include "z80/z80.h"

extern int zx81_do_scanlines(int tstotal);
extern int signal_int_flag;
extern int load_hook, save_hook;
extern ZX81 zx81;

int keyboard_buffer[MAX_KEYCODES];

sdl_com_line_ sdl_com_line;
sdl_emulator_ sdl_emulator;
sdl_sound_ sdl_sound;
sdl_zx80rom_ sdl_zx80rom;
sdl_zx81rom_ sdl_zx81rom;
sdl_aszmicrom_ sdl_aszmicrom;
keyrepeat_ sdl_key_repeat;

int sound_enabled = 0;

static unsigned char staged[48 * 1024];
static int staged_len = 0;
static unsigned char savebuf[48 * 1024];
static int save_len = 0;
static int save_pending = 0;

void SDL_Delay(Uint32 ms) { (void)ms; }

void sound_ay_write(int reg, int val, int dev) {
	(void)reg; (void)val; (void)dev;
}
void sound_fnmi(int data) { (void)data; }
void sound_beeper(int on) { (void)on; }
void sound_frame(void) {}
void sound_ay_setvol(void) {}
int sound_framesiz_init(void) { return 0; }
void sound_reset(void) {}
void sound_init(void) {}
void sound_end(void) {}
void sound_ay_reset(void) {}

void w_init(void) {}
void w_exit(void) {}
void w_write(int port, int val) { (void)port; (void)val; }
unsigned int w_read(int port) { (void)port; return 0xff; }

void sdl_set_redraw_video(void) {}
void update_scrn(void) {}

void strcatdelimiter(char *toappendto) {
	size_t n = strlen(toappendto);
	if (n && toappendto[n - 1] != '/') strcat(toappendto, "/");
}

static void apply_ts1000_timing(void) {
	zx81.NTSC = 1;
	machine.tperscanline = 207;
	machine.scanlines = 310 - (55 - 31) * 2;
	machine.tperframe = machine.tperscanline * machine.scanlines - 7;
}

static int install_rom(void) {
	FILE *fp = fopen("/zx81.rom", "rb");
	if (!fp) return 1;
	sdl_zx81rom.state = (int)fread(sdl_zx81rom.data, 1, 8192, fp);
	fclose(fp);
	return sdl_zx81rom.state == 8192 ? 0 : 1;
}

static int boot_machine(void) {
	if (install_rom()) return 1;
	memset(keyboard_buffer, 0, sizeof keyboard_buffer);
	for (int i = 0; i < 9; i++) keyports[i] = 0xff;

	zx80 = 0;
	sdl_emulator.model = &zx80;
	sdl_emulator.state = 1;
	sdl_emulator.paused = 0;
	sdl_emulator.frameskip = 0;
	sdl_emulator.speed = 20;
	sdl_emulator.autoload = 0;
	sdl_emulator.networking = 0;
	sdl_emulator.m1not = 0;
	sdl_emulator.chrgen = CHRGENSINCLAIR;
	sdl_emulator.wrx = HIRESDISABLED;
	sdl_sound.device = DEVICE_NONE;
	load_hook = 1;
	save_hook = 1;

	zx81_initialise();
	apply_ts1000_timing();
	initmem();
	zx81.NTSC = 1;
	zx81.aytype = AY_TYPE_NONE;
	refresh_screen = 1;
	signal_int_flag = 1;
	return 0;
}

int ts1000_set_ram(int kb) {
	if (kb != 1 && kb != 2 && kb != 3 && kb != 4 &&
	    kb != 16 && kb != 32 && kb != 48 && kb != 56)
		kb = 2;
	sdl_emulator.ramsize = kb;
	return boot_machine();
}

int ts1000_init(void) {
	sdl_emulator.ramsize = 2;
	return boot_machine();
}

int ts1000_reset(void) {
	if (sdl_emulator.ramsize < 1) sdl_emulator.ramsize = 2;
	return boot_machine();
}

int ts1000_ram_kb(void) {
	return sdl_emulator.ramsize;
}

void check_events(void) {
	if (!(sdl_emulator.state && !sdl_emulator.paused)) return;

	for (int y = 0; y < 8; y++) {
		int b = 0;
		switch (y) {
		case 0:
			if (keyboard_buffer[SDLK_LSHIFT] || keyboard_buffer[SDLK_RSHIFT] ||
			    keyboard_buffer[SDLK_LCTRL] || keyboard_buffer[SDLK_RCTRL] ||
			    keyboard_buffer[SDLK_BACKSPACE] || keyboard_buffer[SDLK_COMMA] ||
			    keyboard_buffer[SDLK_LEFT] || keyboard_buffer[SDLK_DOWN] ||
			    keyboard_buffer[SDLK_UP] || keyboard_buffer[SDLK_RIGHT])
				b |= 1;
			if (keyboard_buffer[SDLK_z]) b |= 2;
			if (keyboard_buffer[SDLK_x]) b |= 4;
			if (keyboard_buffer[SDLK_c]) b |= 8;
			if (keyboard_buffer[SDLK_v]) b |= 16;
			break;
		case 1:
			if (keyboard_buffer[SDLK_a]) b |= 1;
			if (keyboard_buffer[SDLK_s]) b |= 2;
			if (keyboard_buffer[SDLK_d]) b |= 4;
			if (keyboard_buffer[SDLK_f]) b |= 8;
			if (keyboard_buffer[SDLK_g]) b |= 16;
			break;
		case 2:
			if (keyboard_buffer[SDLK_q]) b |= 1;
			if (keyboard_buffer[SDLK_w]) b |= 2;
			if (keyboard_buffer[SDLK_e]) b |= 4;
			if (keyboard_buffer[SDLK_r]) b |= 8;
			if (keyboard_buffer[SDLK_t]) b |= 16;
			break;
		case 3:
			if (keyboard_buffer[SDLK_1]) b |= 1;
			if (keyboard_buffer[SDLK_2]) b |= 2;
			if (keyboard_buffer[SDLK_3]) b |= 4;
			if (keyboard_buffer[SDLK_4]) b |= 8;
			if (keyboard_buffer[SDLK_5] || keyboard_buffer[SDLK_LEFT]) b |= 16;
			break;
		case 4:
			if (keyboard_buffer[SDLK_0] || keyboard_buffer[SDLK_BACKSPACE]) b |= 1;
			if (keyboard_buffer[SDLK_9]) b |= 2;
			if (keyboard_buffer[SDLK_8] || keyboard_buffer[SDLK_RIGHT]) b |= 4;
			if (keyboard_buffer[SDLK_7] || keyboard_buffer[SDLK_UP]) b |= 8;
			if (keyboard_buffer[SDLK_6] || keyboard_buffer[SDLK_DOWN]) b |= 16;
			break;
		case 5:
			if (keyboard_buffer[SDLK_p]) b |= 1;
			if (keyboard_buffer[SDLK_o]) b |= 2;
			if (keyboard_buffer[SDLK_i]) b |= 4;
			if (keyboard_buffer[SDLK_u]) b |= 8;
			if (keyboard_buffer[SDLK_y]) b |= 16;
			break;
		case 6:
			if (keyboard_buffer[SDLK_RETURN]) b |= 1;
			if (keyboard_buffer[SDLK_l]) b |= 2;
			if (keyboard_buffer[SDLK_k]) b |= 4;
			if (keyboard_buffer[SDLK_j]) b |= 8;
			if (keyboard_buffer[SDLK_h]) b |= 16;
			break;
		case 7:
			if (keyboard_buffer[SDLK_SPACE]) b |= 1;
			if (keyboard_buffer[SDLK_PERIOD] || keyboard_buffer[SDLK_COMMA]) b |= 2;
			if (keyboard_buffer[SDLK_m]) b |= 4;
			if (keyboard_buffer[SDLK_n]) b |= 8;
			if (keyboard_buffer[SDLK_b]) b |= 16;
			break;
		}
		keyports[y] = (b ^ 31) | 0xe0;
	}
}

void ts1000_key(int key, int down) {
	if (key < 0 || key >= MAX_KEYCODES) return;
	keyboard_buffer[key] = down ? 1 : 0;
}

void ts1000_frame(void) {
	check_events();
	signal_int_flag = 1;
	zx81.aytype = AY_TYPE_NONE;
	zx81_do_scanlines(machine.tperframe);
	if (interrupted == 1) interrupted = 0;
}

int ts1000_ink(void) {
	int n = 0;
	int size = ZX_VID_FULLWIDTH * ZX_VID_FULLHEIGHT / 8;
	for (int i = 0; i < size; i++) {
		unsigned char b = scrnbmp[i];
		while (b) {
			n += b & 1;
			b >>= 1;
		}
	}
	return n;
}

unsigned char *ts1000_screen(void) { return scrnbmp; }

int ts1000_screen_size(void) {
	return ZX_VID_FULLWIDTH * ZX_VID_FULLHEIGHT / 8;
}

static int ram_bytes(void) {
	int n = sdl_emulator.ramsize * 1024 - 9;
	if (n < 1) n = 1;
	return n;
}

static void place_program(const unsigned char *data, int len) {
	int n = len;
	int max = ram_bytes();
	if (n > max) n = max;
	memcpy(mem + 0x4009, data, n);
}

/* Called from the patched ROM LOAD routine. The CPU is already inside LOAD. */
int sdl_load_file(int parameter, int method) {
	(void)parameter;
	(void)method;
	if (!staged_len) return 1;
	place_program(staged, staged_len);
	return 0;
}

int sdl_save_file(int parameter, int method) {
	int eline, n;
	(void)parameter;
	(void)method;
	eline = (mem[0x4015] << 8) | mem[0x4014];
	n = eline - 0x4009;
	if (n < 1 || n > (int)sizeof savebuf) return 1;
	memcpy(savebuf, mem + 0x4009, n);
	save_len = n;
	save_pending = 1;
	return 0;
}

int ts1000_stage(const unsigned char *data, int len) {
	if (!data || len < 1 || len > (int)sizeof staged) return 1;
	memcpy(staged, data, len);
	staged_len = len;
	return 0;
}

static void run_frames(int n) {
	while (n-- > 0) ts1000_frame();
}

static void tap(int key, int shifted) {
	if (shifted) keyboard_buffer[SDLK_LSHIFT] = 1;
	keyboard_buffer[key] = 1;
	run_frames(6);
	keyboard_buffer[key] = 0;
	keyboard_buffer[SDLK_LSHIFT] = 0;
	run_frames(6);
}

/* True when the edit line holds exactly `text` followed by the cursor. */
static int edit_line_is(const unsigned char *text, int n) {
	int e = mem[0x4014] | mem[0x4015] << 8;
	return memcmp(mem + e, text, n) == 0 && mem[e + n] == 0x7f;
}

/* The ROM ignores keys for a while after reset (a few hundred frames with
 * 16K), so keep tapping until the edit line shows the expected text. */
static int type_key(int key, int shifted, const unsigned char *text, int n) {
	int tries;
	for (tries = 0; tries < 100; tries++) {
		tap(key, shifted);
		if (edit_line_is(text, n)) return 0;
	}
	return 1;
}

/* Reset, then type LOAD "" so the ROM's own LOAD routine takes the staged
 * program and starts it the way a cassette load would. */
int ts1000_load_staged(void) {
	/* LOAD "" : the LOAD token, then two quote characters */
	static const unsigned char load_line[] = { 0xef, 0x0b, 0x0b };
	int need;
	if (!staged_len) return 1;
	need = 2;
	if (staged_len + 9 > 2 * 1024) need = 16;
	if (staged_len + 9 > 16 * 1024) need = 32;
	if (staged_len + 9 > 32 * 1024) need = 48;
	if (sdl_emulator.ramsize < need) sdl_emulator.ramsize = need;
	if (boot_machine()) return 1;
	if (type_key(SDLK_j, 0, load_line, 1) ||
	    type_key(SDLK_p, 1, load_line, 2) ||
	    type_key(SDLK_p, 1, load_line, 3))
		return 1;
	tap(SDLK_RETURN, 0);
	return 0;
}

int ts1000_save_len(void) { return save_pending ? save_len : 0; }

unsigned char *ts1000_save_ptr(void) { return savebuf; }

void ts1000_save_ack(void) { save_pending = 0; }

/* Capture the BASIC program currently in RAM. Returns byte count. */
int ts1000_program_len(void) {
	int eline = (mem[0x4015] << 8) | mem[0x4014];
	int n = eline - 0x4009;
	if (n < 1 || n > 48 * 1024) return 0;
	return n;
}

unsigned char *ts1000_program_ptr(void) { return mem + 0x4009; }
