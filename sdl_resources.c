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
#if defined(PLATFORM_GP2X)
	#define LOCAL_DATA_DIR ""
	#define RESOURCE_FILE "sz81rc"
#elif defined(PLATFORM_ZAURUS)
	#define LOCAL_DATA_DIR ".sz81"
	#define RESOURCE_FILE ".sz81rc"
#elif defined(__amigaos4__)
	#define LOCAL_DATA_DIR amiga_data_dir
	#define RESOURCE_FILE amiga_resource_file
#else
	#define LOCAL_DATA_DIR ".sz81"
	#define RESOURCE_FILE ".sz81rc"
#endif

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
#define ICON_RUNOPTS_X 160
#define ICON_RUNOPTS_Y 0

/* Variables */
struct {
	char filename[256];
} zxprinter;

/* Function prototypes */


/***************************************************************************
 * ZX Printer Initialise                                                   *
 ***************************************************************************/

void sdl_zxprinter_init(void) {
	struct tm *timestruct;
	time_t rightnow;
	char unique[96];

	#if defined(PLATFORM_GP2X)
		/* If used, the load selector can change the working directory
		 * and this affects relatively referenced files.
		 * Once the load selector has been replaced this code
		 * should be reviewed. I'll mark it temp temp */
		strcpy(zxprinter.filename, workdir);
		strcat(zxprinter.filename, "/");
	#elif defined(PLATFORM_ZAURUS)
		strcpy(zxprinter.filename, getenv ("HOME"));
		strcat(zxprinter.filename, "/" LOCAL_DATA_DIR "/");
	#elif defined(__amigaos4__)
		strcpy(zxprinter.filename, LOCAL_DATA_DIR);
		if((zxprinter.filename[strlen(zxprinter.filename)-1] != '/') &&
			(zxprinter.filename[strlen(zxprinter.filename)-1] != ':'))
			strcat(zxprinter.filename, "/");
	#else
		strcpy(zxprinter.filename, getenv ("HOME"));
		strcat(zxprinter.filename, "/" LOCAL_DATA_DIR "/");
	#endif

	/* Create a unique filename using the date and time */
	rightnow = time(NULL);
	timestruct = localtime(&rightnow);
	strftime(unique, sizeof(unique), "zxprinter-%Y%m%d-%H%M%S.pbm", timestruct);
	strcat(zxprinter.filename, unique);

	/* Update z81's filename pointer */
	zxpfilename = zxprinter.filename;
}

/***************************************************************************
 * Local Data Dir Initialise                                               *
 ***************************************************************************/
/* This sets-up the local data directory which will
 * accommodate the various sz81-created files */

void local_data_dir_init(void) {
	char foldername[256];

	/* Create local data directory [structure] whilst ignoring errors */
	#if defined(PLATFORM_GP2X)
		strcpy(foldername, "");	/* Avoids warnings */
	#elif defined(PLATFORM_ZAURUS)
		strcpy(foldername, getenv ("HOME"));
		strcat(foldername, "/" LOCAL_DATA_DIR);
		mkdir(foldername, 0755);
	#elif defined(__amigaos4__)
		strcpy(foldername, LOCAL_DATA_DIR);
		mkdir(foldername, 0755);
	#else
		strcpy(foldername, getenv ("HOME"));
		strcat(foldername, "/" LOCAL_DATA_DIR);
		mkdir(foldername, 0755);
	#endif
}

/***************************************************************************
 * Read Resource File                                                      *
 ***************************************************************************/

void sdl_rcfile_read(void) {
	char line[256], key[64], value[192];
	struct ctrlremap read_ctrl_remaps[MAX_CTRL_REMAPS];
	struct keyrepeat read_key_repeat;
	struct colourtable read_colours;
	int read_vkeyb_alpha, read_vkeyb_autohide, read_vkeyb_toggle_shift;
	int read_emulator_frameskip, read_emulator_model;
	int read_emulator_ramsize, read_emulator_invert;
	int count, index, line_count, found;
	int read_joystick_dead_zone;
	char read_version[16];
	int read_sound_volume;
	FILE *fp;

	#if defined(PLATFORM_GP2X)
		/* If used, the load selector can change the working directory
		 * and this affects relatively referenced files.
		 * Once the load selector has been replaced this code
		 * should be reviewed. I'll mark it temp temp */
		strcpy(rcfile.filename, workdir);
		strcat(rcfile.filename, "/");
	#elif defined(PLATFORM_ZAURUS)
		strcpy(rcfile.filename, getenv ("HOME"));
		strcat(rcfile.filename, "/");
	#elif defined(__amigaos4__)
		rcfile.filename[0] = '\0';
	#else
		strcpy(rcfile.filename, getenv ("HOME"));
		strcat(rcfile.filename, "/");
	#endif
	strcat(rcfile.filename, RESOURCE_FILE);

	fprintf(stdout, "Reading from %s\n", rcfile.filename);
	if ((fp = fopen(rcfile.filename, "r")) == NULL) {
		fprintf(stderr, "Cannot read from %s\n", rcfile.filename);
		/* Schedule a new rcfile */
		rcfile.rewrite = TRUE;
		return;
	}

	strcpy(read_version, "");
	read_joystick_dead_zone = UNDEFINED;
	read_key_repeat.delay = UNDEFINED;
	read_key_repeat.interval = UNDEFINED;
	read_sound_volume = UNDEFINED;
	read_emulator_frameskip = UNDEFINED;
	read_emulator_model = UNDEFINED;
	read_emulator_ramsize = UNDEFINED;
	read_emulator_invert = UNDEFINED;
	read_vkeyb_alpha = UNDEFINED;
	read_vkeyb_autohide = UNDEFINED;
	read_vkeyb_toggle_shift = UNDEFINED;

	/* Undefine everything within read_colours */
	read_colours.emu_fg = UNDEFINED;
	read_colours.emu_bg = UNDEFINED;
	read_colours.hs_load_selected = UNDEFINED;
	read_colours.hs_load_pressed = UNDEFINED;
	read_colours.hs_vkeyb_zx80_selected = UNDEFINED;
	read_colours.hs_vkeyb_zx80_pressed = UNDEFINED;
	read_colours.hs_vkeyb_zx80_toggle_pressed = UNDEFINED;
	read_colours.hs_vkeyb_zx81_selected = UNDEFINED;
	read_colours.hs_vkeyb_zx81_pressed = UNDEFINED;
	read_colours.hs_vkeyb_zx81_toggle_pressed = UNDEFINED;
	read_colours.hs_ctb_selected = UNDEFINED;
	read_colours.hs_ctb_pressed = UNDEFINED;
	read_colours.hs_options_selected = UNDEFINED;
	read_colours.hs_options_pressed = UNDEFINED;

	/* Undefine everything within read_ctrl_remaps */
	for (count = 0; count < MAX_CTRL_REMAPS; count++) {
		read_ctrl_remaps[count].components = UNDEFINED;
		read_ctrl_remaps[count].protected = UNDEFINED;
		read_ctrl_remaps[count].device = UNDEFINED;
		read_ctrl_remaps[count].id = UNDEFINED;
		read_ctrl_remaps[count].remap_device = UNDEFINED;
		read_ctrl_remaps[count].remap_id = UNDEFINED;
		read_ctrl_remaps[count].remap_mod_id = UNDEFINED;
	}

	/* Read the rcfile one line at a time (trailing [CR]LFs are read too) */
	index = -1;
	line_count = 0;
	found = FALSE;
	while ((fgets(line, 256, fp)) != NULL) {
		line_count++;
		/* Remove the trailing [CR]LFs */
		for (count = strlen(line) - 1; count >= 0; count--)
			if (line[count] == 13 || line[count] == 10) line[count] = 0;
		/* Process the line ignoring comments */
		if (strlen(line) && line[0] != '#') {
			strcpy(key, "version=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%s", read_version);
			}
			strcpy(key, "joystick_dead_zone=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_joystick_dead_zone);
			}
			strcpy(key, "key_repeat.delay=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_key_repeat.delay);
			}
			strcpy(key, "key_repeat.interval=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_key_repeat.interval);
			}
			strcpy(key, "sound.volume=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_sound_volume);
			}
			strcpy(key, "emulator.frameskip=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_emulator_frameskip);
			}
			strcpy(key, "emulator.model=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_emulator_model);
			}
			strcpy(key, "emulator.ramsize=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_emulator_ramsize);
			}
			strcpy(key, "emulator.invert=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_emulator_invert);
			}
			strcpy(key, "vkeyb.alpha=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_vkeyb_alpha);
			}
			strcpy(key, "vkeyb.autohide=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_vkeyb_autohide);
			}
			strcpy(key, "vkeyb.toggle_shift=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%i", &read_vkeyb_toggle_shift);
			}
			/* Colours */
			strcpy(key, "colour.emu_fg=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.emu_fg);
			}
			strcpy(key, "colour.emu_bg=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.emu_bg);
			}
			strcpy(key, "colour.hs_load_selected=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_load_selected);
			}
			strcpy(key, "colour.hs_load_pressed=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_load_pressed);
			}
			strcpy(key, "colour.hs_vkeyb_zx80_selected=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_vkeyb_zx80_selected);
			}
			strcpy(key, "colour.hs_vkeyb_zx80_pressed=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_vkeyb_zx80_pressed);
			}
			strcpy(key, "colour.hs_vkeyb_zx80_toggle_pressed=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_vkeyb_zx80_toggle_pressed);
			}
			strcpy(key, "colour.hs_vkeyb_zx81_selected=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_vkeyb_zx81_selected);
			}
			strcpy(key, "colour.hs_vkeyb_zx81_pressed=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_vkeyb_zx81_pressed);
			}
			strcpy(key, "colour.hs_vkeyb_zx81_toggle_pressed=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_vkeyb_zx81_toggle_pressed);
			}
			strcpy(key, "colour.hs_ctb_selected=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_ctb_selected);
			}
			strcpy(key, "colour.hs_ctb_pressed=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_ctb_pressed);
			}
			strcpy(key, "colour.hs_options_selected=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_options_selected);
			}
			strcpy(key, "colour.hs_options_pressed=");
			if (!strncmp(line, key, strlen(key))) {
				sscanf(&line[strlen(key)], "%x", &read_colours.hs_options_pressed);
			}
			/* Control remappings */
			strcpy(key, "ctrl_remap.components=");
			if (!strncmp(line, key, strlen(key))) {
				strcpy(value, &line[strlen(key)]);
				if (++index >= MAX_CTRL_REMAPS) {
					fprintf(stderr, "%s: Too many remapped controls within rcfile:%03i: "
						"max %i\n", __func__, line_count, MAX_CTRL_REMAPS);
					index = MAX_CTRL_REMAPS - 1;	/* Simply overwrite last slot */
				}
				read_ctrl_remaps[index].components = 0;
				if (strstr(value, "COMP_ALL") != NULL)
					read_ctrl_remaps[index].components |= COMP_ALL;
				if (strstr(value, "COMP_EMU") != NULL)
					read_ctrl_remaps[index].components |= COMP_EMU;
				if (strstr(value, "COMP_LOAD") != NULL)
					read_ctrl_remaps[index].components |= COMP_LOAD;
				if (strstr(value, "COMP_VKEYB") != NULL)
					read_ctrl_remaps[index].components |= COMP_VKEYB;
				if (strstr(value, "COMP_CTB") != NULL)
					read_ctrl_remaps[index].components |= COMP_CTB;
				if (strstr(value, "COMP_RUNOPTS_ALL") != NULL)
					read_ctrl_remaps[index].components |= COMP_RUNOPTS_ALL;
			}
			strcpy(key, "ctrl_remap.protected=");
			if (!strncmp(line, key, strlen(key))) {
				strcpy(value, &line[strlen(key)]);
				if (index == -1) index++;
				if (strcmp(value, "TRUE") == 0 || strcmp(value, "1") == 0) {
					read_ctrl_remaps[index].protected = TRUE;
				} else if (strcmp(value, "FALSE") == 0 || strcmp(value, "0") == 0) {
					read_ctrl_remaps[index].protected = FALSE;
				}
			}
			strcpy(key, "ctrl_remap.device=");
			if (!strncmp(line, key, strlen(key))) {
				strcpy(value, &line[strlen(key)]);
				if (index == -1) index++;
				if (strcmp(value, "DEVICE_KEYBOARD") == 0) {
					read_ctrl_remaps[index].device = DEVICE_KEYBOARD;
				} else if (strcmp(value, "DEVICE_JOYSTICK") == 0) {
					read_ctrl_remaps[index].device = DEVICE_JOYSTICK;
				} else if (strcmp(value, "DEVICE_CURSOR") == 0) {
					read_ctrl_remaps[index].device = DEVICE_CURSOR;
				}
			}
			strcpy(key, "ctrl_remap.id=");
			if (!strncmp(line, key, strlen(key))) {
				strcpy(value, &line[strlen(key)]);
				if (index == -1) index++;
				read_ctrl_remaps[index].id = keysym_to_keycode(value);
			}
			strcpy(key, "ctrl_remap.remap_device=");
			if (!strncmp(line, key, strlen(key))) {
				strcpy(value, &line[strlen(key)]);
				if (index == -1) index++;
				if (strcmp(value, "DEVICE_KEYBOARD") == 0) {
					read_ctrl_remaps[index].remap_device = DEVICE_KEYBOARD;
				} else if (strcmp(value, "DEVICE_JOYSTICK") == 0) {
					read_ctrl_remaps[index].remap_device = DEVICE_JOYSTICK;
				} else if (strcmp(value, "DEVICE_CURSOR") == 0) {
					read_ctrl_remaps[index].remap_device = DEVICE_CURSOR;
				}
			}
			strcpy(key, "ctrl_remap.remap_id=");
			if (!strncmp(line, key, strlen(key))) {
				strcpy(value, &line[strlen(key)]);
				if (index == -1) index++;
				read_ctrl_remaps[index].remap_id = keysym_to_keycode(value);
			}
			strcpy(key, "ctrl_remap.remap_mod_id=");
			if (!strncmp(line, key, strlen(key))) {
				strcpy(value, &line[strlen(key)]);
				if (index == -1) index++;
				read_ctrl_remaps[index].remap_mod_id = keysym_to_keycode(value);
				found = TRUE;	/* A complete ctrl_remap has been read */
			}
		}
	}
	fclose(fp);

	#ifdef SDL_DEBUG_RCFILE
		printf("read_version=%s\n", read_version);
		printf("read_joystick_dead_zone=%i\n", read_joystick_dead_zone);
		printf("read_key_repeat.delay=%i\n", read_key_repeat.delay);
		printf("read_key_repeat.interval=%i\n", read_key_repeat.interval);
		printf("read_sound_volume=%i\n", read_sound_volume);
		printf("read_emulator_frameskip=%i\n", read_emulator_frameskip);
		printf("read_emulator_model=%i\n", read_emulator_model);
		printf("read_emulator_ramsize=%i\n", read_emulator_ramsize);
		printf("read_emulator_invert=%i\n", read_emulator_invert);
		printf("read_vkeyb_alpha=%i\n", read_vkeyb_alpha);
		printf("read_vkeyb_autohide=%i\n", read_vkeyb_autohide);
		printf("read_vkeyb_toggle_shift=%i\n", read_vkeyb_toggle_shift);
		printf("read_colours.emu_fg=%06x\n", read_colours.emu_fg);
		printf("read_colours.emu_bg=%06x\n", read_colours.emu_bg);
		printf("read_colours.hs_load_selected=%06x\n", read_colours.hs_load_selected);
		printf("read_colours.hs_load_pressed=%06x\n", read_colours.hs_load_pressed);
		printf("read_colours.hs_vkeyb_zx80_selected=%06x\n", read_colours.hs_vkeyb_zx80_selected);
		printf("read_colours.hs_vkeyb_zx80_pressed=%06x\n", read_colours.hs_vkeyb_zx80_pressed);
		printf("read_colours.hs_vkeyb_zx80_toggle_pressed=%06x\n", read_colours.hs_vkeyb_zx80_toggle_pressed);
		printf("read_colours.hs_vkeyb_zx81_selected=%06x\n", read_colours.hs_vkeyb_zx81_selected);
		printf("read_colours.hs_vkeyb_zx81_pressed=%06x\n", read_colours.hs_vkeyb_zx81_pressed);
		printf("read_colours.hs_vkeyb_zx81_toggle_pressed=%06x\n", read_colours.hs_vkeyb_zx81_toggle_pressed);
		printf("read_colours.hs_ctb_selected=%06x\n", read_colours.hs_ctb_selected);
		printf("read_colours.hs_ctb_pressed=%06x\n", read_colours.hs_ctb_pressed);
		printf("read_colours.hs_options_selected=%06x\n", read_colours.hs_options_selected);
		printf("read_colours.hs_options_pressed=%06x\n", read_colours.hs_options_pressed);
		for (count = 0; count < MAX_CTRL_REMAPS; count++) {
			if (read_ctrl_remaps[count].device != UNDEFINED) {
				printf("read_ctrl_remaps[%i].components=%i\n", count, read_ctrl_remaps[count].components);
				printf("  read_ctrl_remaps[%i].protected=%i\n", count, read_ctrl_remaps[count].protected);
				printf("  read_ctrl_remaps[%i].device=%i\n", count, read_ctrl_remaps[count].device);
				printf("  read_ctrl_remaps[%i].id=%i\n", count, read_ctrl_remaps[count].id);
				printf("  read_ctrl_remaps[%i].remap_device=%i\n", count, read_ctrl_remaps[count].remap_device);
				printf("  read_ctrl_remaps[%i].remap_id=%i\n", count, read_ctrl_remaps[count].remap_id);
				printf("  read_ctrl_remaps[%i].remap_mod_id=%i\n", count, read_ctrl_remaps[count].remap_mod_id);
			}
		}
	#endif

	if (strcmp(read_version, VERSION) != 0) {
		/* For the moment as features are being added and modified
		 * discard rcfiles from previous versions temp temp */
		fprintf(stderr, "Discarding rcfile from previous version\n");
		/* Schedule a new rcfile */
		rcfile.rewrite = TRUE;
	} else {
		/* Store settings after first checking their validity */
		/* Joystick dead zone */
		if (read_joystick_dead_zone != UNDEFINED) {
			if (read_joystick_dead_zone >= 1 && read_joystick_dead_zone <= 99) {
				joystick_dead_zone = read_joystick_dead_zone;
			} else {
				fprintf(stderr, "%s: joystick_dead_zone within rcfile is invalid: try 1 to 99\n",
					__func__);
			}
		}
		/* Key repeat delay */
		if (read_key_repeat.delay != UNDEFINED) {
			if (read_key_repeat.delay >= 80 && read_key_repeat.delay <= 520) {
				sdl_key_repeat.delay = read_key_repeat.delay;
			} else {
				fprintf(stderr, "%s: key_repeat.delay within rcfile is invalid: try 80 to 520\n",
					__func__);
			}
		}
		/* Key repeat interval */
		if (read_key_repeat.interval != UNDEFINED) {
			if (read_key_repeat.interval >= 80 && read_key_repeat.interval <= 520) {
				sdl_key_repeat.interval = read_key_repeat.interval;
			} else {
				fprintf(stderr, "%s: key_repeat.interval within rcfile is invalid: try 80 to 520\n",
					__func__);
			}
		}
		/* Sound volume */
		if (read_sound_volume != UNDEFINED) {
			if (read_sound_volume >= 0 && read_sound_volume <= 128) {
				sdl_sound.volume = read_sound_volume;
			} else {
				fprintf(stderr, "%s: sound.volume within rcfile is invalid: try 0 to 128\n",
					__func__);
			}
		}
		/* Frameskip*/
		if (read_emulator_frameskip != UNDEFINED) {
			if (read_emulator_frameskip >= 0 && read_emulator_frameskip <= MAX_FRAMESKIP) {
				sdl_emulator.frameskip = read_emulator_frameskip;
			} else {
				fprintf(stderr, "%s: emulator.frameskip within rcfile is invalid: try 0 to %i\n",
					__func__, MAX_FRAMESKIP);
			}
		}
		/* Machine model */
		if (read_emulator_model != UNDEFINED) {
			if (read_emulator_model >= 0 && read_emulator_model <= 1) {
				*sdl_emulator.model = read_emulator_model;
			} else {
				fprintf(stderr, "%s: emulator.model within rcfile is invalid: try 0 to 1\n",
					__func__);
			}
		}
		/* RAM size */
		if (read_emulator_ramsize != UNDEFINED) {
			if (read_emulator_ramsize == 1 || read_emulator_ramsize == 2 ||
				read_emulator_ramsize == 4 || read_emulator_ramsize == 8 ||
				read_emulator_ramsize == 16 || read_emulator_ramsize == 32 ||
				read_emulator_ramsize == 48 || read_emulator_ramsize == 56) {
				sdl_emulator.ramsize = read_emulator_ramsize;
			} else {
				fprintf(stderr, "%s: emulator.ramsize within rcfile is invalid: try 1, 2, 4, 8, 16, 32, 48 or 56\n",
					__func__);
			}
		}
		/* Invert screen */
		if (read_emulator_invert != UNDEFINED) {
			if (read_emulator_invert >= 0 && read_emulator_invert <= 1) {
				sdl_emulator.invert = read_emulator_invert;
			} else {
				fprintf(stderr, "%s: emulator.invert within rcfile is invalid: try 0 to 1\n",
					__func__);
			}
		}
		/* Vkeyb alpha */
		if (read_vkeyb_alpha != UNDEFINED) {
			if (read_vkeyb_alpha >= 0 && read_vkeyb_alpha <= SDL_ALPHA_OPAQUE) {
				vkeyb.alpha = read_vkeyb_alpha;
			} else {
				fprintf(stderr, "%s: vkeyb.alpha within rcfile is invalid: try 0 to %i\n",
					__func__, SDL_ALPHA_OPAQUE);
			}
		}
		/* Vkeyb autohide */
		if (read_vkeyb_autohide != UNDEFINED) {
			if (read_vkeyb_autohide >= 0 && read_vkeyb_autohide <= 1) {
				vkeyb.autohide = read_vkeyb_autohide;
			} else {
				fprintf(stderr, "%s: vkeyb.autohide within rcfile is invalid: try 0 to 1\n",
					__func__);
			}
		}
		/* Vkeyb toggle shift */
		if (read_vkeyb_toggle_shift != UNDEFINED) {
			if (read_vkeyb_toggle_shift >= 0 && read_vkeyb_toggle_shift <= 1) {
				vkeyb.toggle_shift = read_vkeyb_toggle_shift;
			} else {
				fprintf(stderr, "%s: vkeyb.toggle_shift within rcfile is invalid: try 0 to 1\n",
					__func__);
			}
		}
		/* Colours */
		if (read_colours.emu_fg != UNDEFINED) colours.emu_fg = read_colours.emu_fg;
		if (read_colours.emu_bg != UNDEFINED) colours.emu_bg = read_colours.emu_bg;
		if (read_colours.hs_load_selected != UNDEFINED) 
			colours.hs_load_selected = read_colours.hs_load_selected;
		if (read_colours.hs_load_pressed != UNDEFINED) 
			colours.hs_load_pressed = read_colours.hs_load_pressed;
		if (read_colours.hs_vkeyb_zx80_selected != UNDEFINED) 
			colours.hs_vkeyb_zx80_selected = read_colours.hs_vkeyb_zx80_selected;
		if (read_colours.hs_vkeyb_zx80_pressed != UNDEFINED) 
			colours.hs_vkeyb_zx80_pressed = read_colours.hs_vkeyb_zx80_pressed;
		if (read_colours.hs_vkeyb_zx80_toggle_pressed != UNDEFINED) 
			colours.hs_vkeyb_zx80_toggle_pressed = read_colours.hs_vkeyb_zx80_toggle_pressed;
		if (read_colours.hs_vkeyb_zx81_selected != UNDEFINED) 
			colours.hs_vkeyb_zx81_selected = read_colours.hs_vkeyb_zx81_selected;
		if (read_colours.hs_vkeyb_zx81_pressed != UNDEFINED) 
			colours.hs_vkeyb_zx81_pressed = read_colours.hs_vkeyb_zx81_pressed;
		if (read_colours.hs_vkeyb_zx81_toggle_pressed != UNDEFINED) 
			colours.hs_vkeyb_zx81_toggle_pressed = read_colours.hs_vkeyb_zx81_toggle_pressed;
		if (read_colours.hs_ctb_selected != UNDEFINED) 
			colours.hs_ctb_selected = read_colours.hs_ctb_selected;
		if (read_colours.hs_ctb_pressed != UNDEFINED) 
			colours.hs_ctb_pressed = read_colours.hs_ctb_pressed;
		if (read_colours.hs_options_selected != UNDEFINED) 
			colours.hs_options_selected = read_colours.hs_options_selected;
		if (read_colours.hs_options_pressed != UNDEFINED) 
			colours.hs_options_pressed = read_colours.hs_options_pressed;

		/* read_ctrl_remaps have pretty much validated themselves since if
		 * something was found to be invalid then they'll still be UNDEFINED	Redundant: I'm changing updating to completely
		 * and they won't overwrite/insert into ctrl_remaps.					replacing if at least one has been read.
		 * 
		 * Attempt to find a match and overwrite the existing default
		 * controls; insert new ones following the defaults
		for (index = 0; index < MAX_CTRL_REMAPS; index++) {
			for (count = 0; count < MAX_CTRL_REMAPS; count++) {
				if (read_ctrl_remaps[index].components > 0 &&
					read_ctrl_remaps[index].device != UNDEFINED &&
					read_ctrl_remaps[index].id != UNDEFINED) {
					if (ctrl_remaps[count].device != UNDEFINED &&
						read_ctrl_remaps[index].components == ctrl_remaps[count].components &&
						read_ctrl_remaps[index].device == ctrl_remaps[count].device &&
						read_ctrl_remaps[index].remap_device == ctrl_remaps[count].remap_device &&
						read_ctrl_remaps[index].remap_id == ctrl_remaps[count].remap_id &&
						read_ctrl_remaps[index].remap_mod_id == ctrl_remaps[count].remap_mod_id) {
						/$ Update existing $/
						#ifdef SDL_DEBUG_RCFILE
							printf("Overwriting ctrl_remaps[%i] with read_ctrl_remaps[%i]\n", count, index);
						#endif
						ctrl_remaps[count] = read_ctrl_remaps[index];
						break;
					} else if (ctrl_remaps[count].device == UNDEFINED) {
						/$ Insert new $/
						#ifdef SDL_DEBUG_RCFILE
							printf("Inserting read_ctrl_remaps[%i] into ctrl_remaps[%i]\n", index, count);
						#endif
						ctrl_remaps[count] = read_ctrl_remaps[index];
						break;
					}
				}
			}
		} */

		/* New improved system: If one or more ctrl_remaps were read then they will
		 * completely replace the existing default ctrl_remaps. If none were read
		 * then the defaults will remain. So after the first rcfile write the defaults
		 * will always be replaced until either the rcfile is deleted or the user deletes
		 * the ctrl_remaps manually */
		if (found)
			for (count = 0; count < MAX_CTRL_REMAPS; count++)
				ctrl_remaps[count] = read_ctrl_remaps[count];

	}
}

/***************************************************************************
 * Write Resource File                                                     *
 ***************************************************************************/

void rcfile_write(void) {
	char key[64], value[192];
	int count, found;
	FILE *fp;

	fprintf(stdout, "Writing to %s\n", rcfile.filename);
	if ((fp = fopen(rcfile.filename, "w")) == NULL) {
		fprintf(stderr, "Cannot write to %s\n", rcfile.filename);
		return;
	}

	fprintf(fp, "\n");
	fprintf(fp, "version=%s\n", VERSION);
	fprintf(fp, "joystick_dead_zone=%i\n", joystick_dead_zone);
	fprintf(fp, "key_repeat.delay=%i\n", sdl_key_repeat.delay);
	fprintf(fp, "key_repeat.interval=%i\n", sdl_key_repeat.interval);
	fprintf(fp, "sound.volume=%i\n", sdl_sound.volume);
	fprintf(fp, "emulator.frameskip=%i\n", sdl_emulator.frameskip);
	fprintf(fp, "emulator.model=%i\n", *sdl_emulator.model);
	fprintf(fp, "emulator.ramsize=%i\n", sdl_emulator.ramsize);
	fprintf(fp, "emulator.invert=%i\n", sdl_emulator.invert);
	fprintf(fp, "vkeyb.alpha=%i\n", vkeyb.alpha);
	fprintf(fp, "vkeyb.autohide=%i\n", vkeyb.autohide);
	fprintf(fp, "vkeyb.toggle_shift=%i\n", vkeyb.toggle_shift);
	fprintf(fp, "\n");

	fprintf(fp, "colour.emu_fg=%06x\n", colours.emu_fg);
	fprintf(fp, "colour.emu_bg=%06x\n", colours.emu_bg);
	fprintf(fp, "colour.hs_load_selected=%06x\n", colours.hs_load_selected);
	fprintf(fp, "colour.hs_load_pressed=%06x\n", colours.hs_load_pressed);
	fprintf(fp, "colour.hs_vkeyb_zx80_selected=%06x\n", colours.hs_vkeyb_zx80_selected);
	fprintf(fp, "colour.hs_vkeyb_zx80_pressed=%06x\n", colours.hs_vkeyb_zx80_pressed);
	fprintf(fp, "colour.hs_vkeyb_zx80_toggle_pressed=%06x\n", colours.hs_vkeyb_zx80_toggle_pressed);
	fprintf(fp, "colour.hs_vkeyb_zx81_selected=%06x\n", colours.hs_vkeyb_zx81_selected);
	fprintf(fp, "colour.hs_vkeyb_zx81_pressed=%06x\n", colours.hs_vkeyb_zx81_pressed);
	fprintf(fp, "colour.hs_vkeyb_zx81_toggle_pressed=%06x\n", colours.hs_vkeyb_zx81_toggle_pressed);
	fprintf(fp, "colour.hs_ctb_selected=%06x\n", colours.hs_ctb_selected);
	fprintf(fp, "colour.hs_ctb_pressed=%06x\n", colours.hs_ctb_pressed);
	fprintf(fp, "colour.hs_options_selected=%06x\n", colours.hs_options_selected);
	fprintf(fp, "colour.hs_options_pressed=%06x\n", colours.hs_options_pressed);
	fprintf(fp, "\n");
	
	for (count = 0; count < MAX_CTRL_REMAPS; count++) {
		if (ctrl_remaps[count].device != UNDEFINED) {
			/* .components */
			strcpy(key, "ctrl_remap.components");
			strcpy(value, "");
			found = FALSE;
			if ((ctrl_remaps[count].components & COMP_ALL) == COMP_ALL) {
				strcat(value, "COMP_ALL");
			} else {
				if (ctrl_remaps[count].components & COMP_EMU) {
					strcat(value, "COMP_EMU"); found = TRUE;
				}				
				if (ctrl_remaps[count].components & COMP_LOAD) {
					if (found) strcat(value, " | ");
					strcat(value, "COMP_LOAD"); found = TRUE;
				}				
				if (ctrl_remaps[count].components & COMP_VKEYB) {
					if (found) strcat(value, " | ");
					strcat(value, "COMP_VKEYB"); found = TRUE;
				}				
				if (ctrl_remaps[count].components & COMP_CTB) {
					if (found) strcat(value, " | ");
					strcat(value, "COMP_CTB"); found = TRUE;
				}				
				/* COMP_RUNOPTS_ALL isn't required to be broken down */
				if ((ctrl_remaps[count].components & COMP_RUNOPTS_ALL) == COMP_RUNOPTS_ALL) {
					if (found) strcat(value, " | ");
					strcat(value, "COMP_RUNOPTS_ALL"); found = TRUE;
				}				
			}				
			fprintf(fp, "%s=%s\n", key, value);
			/* .protected */
			strcpy(key, "ctrl_remap.protected");
			strcpy(value, "");
			if (ctrl_remaps[count].protected) {
				strcat(value, "TRUE");
			} else {
				strcat(value, "FALSE");
			}
			fprintf(fp, "%s=%s\n", key, value);
			/* .device */
			strcpy(key, "ctrl_remap.device");
			strcpy(value, "");
			if (ctrl_remaps[count].device == DEVICE_KEYBOARD) {
				strcat(value, "DEVICE_KEYBOARD");
			} else if (ctrl_remaps[count].device == DEVICE_JOYSTICK) {
				strcat(value, "DEVICE_JOYSTICK");
			} else if (ctrl_remaps[count].device == DEVICE_CURSOR) {
				strcat(value, "DEVICE_CURSOR");
			}
			fprintf(fp, "%s=%s\n", key, value);
			/* .id */
			strcpy(key, "ctrl_remap.id");
			if (ctrl_remaps[count].device == DEVICE_KEYBOARD ||
				ctrl_remaps[count].device == DEVICE_CURSOR) {
				strcpy(value, keycode_to_keysym(ctrl_remaps[count].id));
			} else {
				sprintf(value, "%i", ctrl_remaps[count].id);
			}
			fprintf(fp, "%s=%s\n", key, value);
			/* .remap_device */
			strcpy(key, "ctrl_remap.remap_device");
			strcpy(value, "");
			if (ctrl_remaps[count].remap_device == DEVICE_KEYBOARD) {
				strcat(value, "DEVICE_KEYBOARD");
			} else if (ctrl_remaps[count].remap_device == DEVICE_JOYSTICK) {
				strcat(value, "DEVICE_JOYSTICK");
			} else if (ctrl_remaps[count].remap_device == DEVICE_CURSOR) {
				strcat(value, "DEVICE_CURSOR");
			}
			fprintf(fp, "%s=%s\n", key, value);
			/* .remap_id */
			strcpy(key, "ctrl_remap.remap_id");
			if (ctrl_remaps[count].remap_device == DEVICE_KEYBOARD ||
				ctrl_remaps[count].remap_device == DEVICE_CURSOR) {
				strcpy(value, keycode_to_keysym(ctrl_remaps[count].remap_id));
			} else {
				sprintf(value, "%i", ctrl_remaps[count].remap_id);
			}
			fprintf(fp, "%s=%s\n", key, value);
			/* .remap_mod_id */
			strcpy(key, "ctrl_remap.remap_mod_id");
			if (ctrl_remaps[count].remap_device == DEVICE_KEYBOARD ||
				ctrl_remaps[count].remap_device == DEVICE_CURSOR) {
				strcpy(value, keycode_to_keysym(ctrl_remaps[count].remap_mod_id));
			} else {
				sprintf(value, "%i", ctrl_remaps[count].remap_mod_id);
			}
			fprintf(fp, "%s=%s\n", key, value);
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
	
	rcfile.rewrite = FALSE;
}

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
			strcpy(filename, PACKAGE_DATA_DIR "/");
			if (count == BMF_FONT_ZX80) {
				strcat(filename, IMG_ZX80_FONT);
			} else if (count == BMF_FONT_ZX81) {
				strcat(filename, IMG_ZX81_FONT);
			} else {
				strcat(filename, IMG_ZX82_FONT);
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
 * It now loads both vkeybs if they haven't already been loaded and then
 * creates the relevant scaled-up version depending on which machine is
 * currently being emulated.
 * 
 * On exit: returns TRUE on error
 *          else FALSE */

int vkeyb_init(void) {
	SDL_Surface *unconverted, *original;
	char filename[256];
	int count;

	/* Free any existing scaled surface first */
	if (vkeyb.scaled) SDL_FreeSurface(vkeyb.scaled);

	for (count = 0; count < 2; count++) {
		if ((count == 0 && vkeyb.zx80original == NULL) || 
			(count == 1 && vkeyb.zx81original == NULL)) {
			/* Prepare the relevant filename */
			if (count == 0) {
				strcpy(filename, PACKAGE_DATA_DIR "/" IMG_ZX80_KYBD);
			} else {
				strcpy(filename, PACKAGE_DATA_DIR "/" IMG_ZX81_KYBD);
			}
			/* Load the bitmap */
			if ((unconverted = SDL_LoadBMP(filename)) == NULL) {
				fprintf(stderr, "%s: Cannot load keyboard image %s: %s\n", 
					__func__, filename, SDL_GetError ());
				return TRUE;
			}
			/* Convert the keyboard surface to the same pixel
			 * format as the screen for fast blitting */
			if (count == 0) {
				vkeyb.zx80original = SDL_DisplayFormat(unconverted);
			} else {
				vkeyb.zx81original = SDL_DisplayFormat(unconverted);
			}
			SDL_FreeSurface(unconverted);	/* Free the redundant surface */
			/* Check the result of SDL_DisplayFormat */
			if ((count == 0 && vkeyb.zx80original == NULL) || 
				(count == 1 && vkeyb.zx81original == NULL)) {
				fprintf(stderr, "%s: Cannot create surface: %s\n", __func__,
					SDL_GetError ());
				return TRUE;
			}
		}
	}

	/* Initialise a pointer to the relevant original surface */
	if (*sdl_emulator.model) {
		original = vkeyb.zx80original;
	} else {
		original = vkeyb.zx81original;
	}

	/* Create an RGB surface to accommodate the scaled keyboard */
	vkeyb.scaled = SDL_CreateRGBSurface(SDL_SWSURFACE, 
		original->w * video.scale, original->h * video.scale, 
		video.screen->format->BitsPerPixel,
		video.screen->format->Rmask, video.screen->format->Gmask,
		video.screen->format->Bmask, video.screen->format->Amask);
	if (vkeyb.scaled == NULL) {
		fprintf(stderr, "%s: Cannot create RGB surface: %s\n", __func__,
			SDL_GetError ());
		return TRUE;
	}

	/* Now scale the keyboard into the new surface */
	scale_surface(original, vkeyb.scaled);

	/* Apply some alpha to the entire surface if required */
	if (vkeyb.alpha < SDL_ALPHA_OPAQUE)
		if (vkeyb_alpha_apply()) return TRUE;

	/* Set-up the virtual keyboard's screen offset */
	vkeyb.xoffset = (video.xres - vkeyb.scaled->w) / 2;
	vkeyb.yoffset = (video.yres - vkeyb.scaled->h) / 2;

	return FALSE;
}

/***************************************************************************
 * Virtual Keyboard Alpha Apply                                            *
 ***************************************************************************/
/* On exit: returns TRUE on error
 *          else FALSE */

int vkeyb_alpha_apply(void) {
	int retval = FALSE;
	
	if ((SDL_SetAlpha(vkeyb.scaled, SDL_SRCALPHA, vkeyb.alpha)) < 0) {
		fprintf(stderr, "%s: Cannot set surface alpha: %s\n", __func__, 
			SDL_GetError());
		retval = TRUE;
	}

	return retval;
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
		strcpy(filename, PACKAGE_DATA_DIR "/");
		strcat(filename, IMG_SZ81_ICONS);
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
	colour = 0x000000;
	dstrect.x = dstrect.y = 0; dstrect.w = dstrect.h = 18 * video.scale;
	for (count = 0; count < 4; count++) {
		/* 17 and not 18 is used below because the outlines overlap */
		if (count == 1) {
			dstrect.x += 17 * 2 * video.scale;
		} else if (count == 2) {
			dstrect.x += 17 * 2 * video.scale;
			dstrect.w = 17 * 5 * video.scale + video.scale;
		} else if (count == 3) {
			dstrect.x += 17 * 9 * video.scale;
			dstrect.w = 18 * video.scale;
		}
		if (SDL_FillRect(control_bar.scaled, &dstrect, SDL_MapRGB(video.screen->format,
			colour >> 16 & 0xff, colour >> 8 & 0xff, colour & 0xff)) < 0) {
			fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
			return TRUE;
		}
	}
	/* Blit the icons */
	srcrect.w = srcrect.h = dstrect.w = dstrect.h = 16 * video.scale;
	dstrect.y = video.scale;
	for (count = 0; count < 8; count++) {
		if (count == 0) {
			/* Exit */
			srcrect.x = ICON_EXIT_X * video.scale; srcrect.y = ICON_EXIT_Y * video.scale;
			dstrect.x = video.scale;
		} else if (count == 1) {
			/* Reset */
			srcrect.x = ICON_RESET_X * video.scale; srcrect.y = ICON_RESET_Y * video.scale;
			dstrect.x = video.scale + 17 * 2 * video.scale; 
		} else if (count == 2) {
			/* Autohide */
			if (vkeyb.autohide) {
				srcrect.x = ICON_AUTOHIDE_X * video.scale; srcrect.y = ICON_AUTOHIDE_Y * video.scale;
			} else {
				srcrect.x = ICON_DONTHIDE_X * video.scale; srcrect.y = ICON_DONTHIDE_Y * video.scale;
			}
			dstrect.x = video.scale + 17 * 4 * video.scale; 
		} else if (count == 3) {
			/* Shift type */
			if (vkeyb.toggle_shift) {
				srcrect.x = ICON_TOGGLESHIFT_X * video.scale; srcrect.y = ICON_TOGGLESHIFT_Y * video.scale;
			} else {
				srcrect.x = ICON_STICKYSHIFT_X * video.scale; srcrect.y = ICON_STICKYSHIFT_Y * video.scale;
			}
			dstrect.x = video.scale + 17 * 5 * video.scale; 
		} else if (count == 4) {
			/* Inverse screen */
			if (sdl_emulator.invert) {
				srcrect.x = ICON_INVERSE_X * video.scale; srcrect.y = ICON_INVERSE_Y * video.scale;
			} else {
				srcrect.x = ICON_NOTINVERSE_X * video.scale; srcrect.y = ICON_NOTINVERSE_Y * video.scale;
			}
			dstrect.x = video.scale + 17 * 6 * video.scale; 
		} else if (count == 5) {
			/* Alpha down */
			srcrect.x = ICON_ALPHA_DN_X * video.scale; srcrect.y = ICON_ALPHA_DN_Y * video.scale;
			dstrect.x = video.scale + 17 * 7 * video.scale; 
		} else if (count == 6) {
			/* Alpha up */
			srcrect.x = ICON_ALPHA_UP_X * video.scale; srcrect.y = ICON_ALPHA_UP_Y * video.scale;
			dstrect.x = video.scale + 17 * 8 * video.scale; 
		} else if (count == 7) {
			/* Runtime options */
			srcrect.x = ICON_RUNOPTS_X * video.scale; srcrect.y = ICON_RUNOPTS_Y * video.scale;
			dstrect.x = video.scale + 17 * 13 * video.scale; 
		}
		if (SDL_BlitSurface (sz81icons.scaled, &srcrect, control_bar.scaled, &dstrect) < 0) {
			fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
			return TRUE;
		}
	}

	/* Set-up the control bar's screen offset */
	control_bar.xoffset = (video.xres - control_bar.scaled->w) / 2;
	if (control_bar.xoffset < 0) control_bar.xoffset = 0;
	control_bar.yoffset = sdl_emulator.yoffset - 19 * video.scale;
	if (control_bar.yoffset < 0) control_bar.yoffset = 0;

	return FALSE;
}

/***************************************************************************
 * ZX ROMs Initialise                                                      *
 ***************************************************************************/
/* This loads the ROMs if they haven't already been loaded.
 * 
 * On exit: returns TRUE on error
 *          else FALSE */

int sdl_zxroms_init(void) {
	int retval = FALSE;
	char filename[256];
	int count;
	FILE *fp;

	for (count = 0; count < 2; count++) {
		if ((count == 0 && !sdl_zx80rom.state) || 
			(count == 1 && !sdl_zx81rom.state)) {
			/* Prepare the relevant filename */
			if (count == 0) {
				strcpy(filename, PACKAGE_DATA_DIR "/" ROM_ZX80);
			} else {
				strcpy(filename, PACKAGE_DATA_DIR "/" ROM_ZX81);
			}
			/* Open the ROM */
			if ((fp = fopen(filename, "rb")) == NULL) {
				fprintf(stderr, "Cannot read from %s\n", filename);
				retval = TRUE;
			} else {
				/* Read in the data */
				if (count == 0) {
					fread(sdl_zx80rom.data, 1, 4 * 1024, fp);
					sdl_zx80rom.state = TRUE;
				} else {
					fread(sdl_zx81rom.data, 1, 8 * 1024, fp);
					sdl_zx81rom.state = TRUE;
				}
				fclose(fp);
			}
		}
	}

	return retval;
}


