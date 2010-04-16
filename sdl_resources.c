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
#ifdef __amigaos4__
#include "amiga.h" /* Amiga-specifics */
#endif

/* Defines */
#if defined(PLATFORM_GP2X)
	#define RESOURCE_FILE "sz81rc"
#elif defined(PLATFORM_ZAURUS)
	#define RESOURCE_FILE ".sz81rc"
#elif defined(__amigaos4__)
	#define RESOURCE_FILE amiga_resource_file
#else
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

/* Function prototypes */


/***************************************************************************
 * Read Resource File                                                      *
 ***************************************************************************/

void sdl_rcfile_read(void) {
	char filename[256], line[256], key[64], value[192];
	struct ctrlremap read_ctrl_remaps[MAX_CTRL_REMAPS];
	struct keyrepeat read_key_repeat;
	struct colourtable read_colours;
	int read_joystick_dead_zone;
	int read_sound_volume;
	int count, index, line_count;
	FILE *fp;
	
	#if defined(PLATFORM_GP2X)
		/* If used, the load selector can change the working directory
		 * and this affects the relatively referenced GP2X rcfile. In
		 * fact any platform that uses the built-in load selector and
		 * references the rcfile relatively should be aware of this issue */
		strcpy(rcfile.workdir, "");
		getcwd(rcfile.workdir, 256);
		strcpy(filename, rcfile.workdir);
		strcat(filename, "/");
	#elif defined(PLATFORM_ZAURUS)
		strcpy(filename, getenv ("HOME"));
		strcat(filename, "/");
	#elif defined(__amigaos4__)
		filename[0] = '\0';
	#else
		strcpy(filename, getenv ("HOME"));
		strcat(filename, "/");
	#endif
	strcat(filename, RESOURCE_FILE);

	fprintf(stdout, "Reading from %s\n", filename);
	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Cannot read from %s\n", filename);
		/* Schedule a new rcfile */
		rcfile.rewrite = TRUE;
		return;
	}

	read_joystick_dead_zone = UNDEFINED;
	read_key_repeat.delay = UNDEFINED;
	read_key_repeat.interval = UNDEFINED;
	read_sound_volume = UNDEFINED;
	
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
	while ((fgets(line, 256, fp)) != NULL) {
		line_count++;
		/* Remove the trailing [CR]LFs */
		for (count = strlen(line) - 1; count >= 0; count--)
			if (line[count] == 13 || line[count] == 10) line[count] = 0;
		/* Process the line ignoring comments */
		if (strlen(line) && line[0] != '#') {
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
				if (strstr(value, "COMP_RUNOPTS0") != NULL)
					read_ctrl_remaps[index].components |= COMP_RUNOPTS0;
				if (strstr(value, "COMP_RUNOPTS1") != NULL)
					read_ctrl_remaps[index].components |= COMP_RUNOPTS1;
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
			}
		}
	}
	fclose(fp);

	#ifdef SDL_DEBUG_RCFILE
		printf("read_joystick_dead_zone=%i\n", read_joystick_dead_zone);
		printf("read_key_repeat.delay=%i\n", read_key_repeat.delay);
		printf("read_key_repeat.interval=%i\n", read_key_repeat.interval);
		printf("read_sound_volume=%i\n", read_sound_volume);
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
	 * something was found to be invalid then they'll still be UNDEFINED
	 * and they won't overwrite/insert into ctrl_remaps.
	 * Attempt to find a match on device and id and overwrite the existing
	 * default ctrl_remaps; insert new ones following the defaults */
	for (index = 0; index < MAX_CTRL_REMAPS; index++) {
		for (count = 0; count < MAX_CTRL_REMAPS; count++) {
			if (read_ctrl_remaps[index].components > 0 &&
				read_ctrl_remaps[index].device != UNDEFINED &&
				read_ctrl_remaps[index].id != UNDEFINED) {
				if (ctrl_remaps[count].device != UNDEFINED &&
					read_ctrl_remaps[index].components == ctrl_remaps[count].components &&
					read_ctrl_remaps[index].device == ctrl_remaps[count].device &&
					read_ctrl_remaps[index].id == ctrl_remaps[count].id) {
					/* Overwrite existing */
					#ifdef SDL_DEBUG_RCFILE
						printf("Overwriting ctrl_remaps[%i] with read_ctrl_remaps[%i]\n", count, index);
					#endif
					ctrl_remaps[count] = read_ctrl_remaps[index];
					break;
				} else if (ctrl_remaps[count].device == UNDEFINED) {
					/* Insert new */
					#ifdef SDL_DEBUG_RCFILE
						printf("Inserting read_ctrl_remaps[%i] into ctrl_remaps[%i]\n", index, count);
					#endif
					ctrl_remaps[count] = read_ctrl_remaps[index];
					break;
				}
			}
		}
	}
}

/***************************************************************************
 * Write Resource File                                                     *
 ***************************************************************************/

void rcfile_write(void) {
	char filename[256], key[64], value[192];
	int count, found;
	FILE *fp;

	#if defined(PLATFORM_GP2X)
		/* If used, the load selector can change the working directory
		 * and this affects the relatively referenced GP2X rcfile. In
		 * fact any platform that uses the built-in load selector and
		 * references the rcfile relatively should be aware of this issue */
		strcpy(filename, rcfile.workdir);
		strcat(filename, "/");
	#elif defined(PLATFORM_ZAURUS)
		strcpy(filename, getenv ("HOME"));
		strcat(filename, "/");
	#elif defined(__amigaos4__)
		filename[0] = '\0';
	#else
		strcpy(filename, getenv ("HOME"));
		strcat(filename, "/");
	#endif
	strcat(filename, RESOURCE_FILE);

	fprintf(stdout, "Writing to %s\n", filename);
	if ((fp = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "Cannot write to %s\n", filename);
		return;
	}

	fprintf(fp, "\n");
	fprintf(fp, "joystick_dead_zone=%i\n", joystick_dead_zone);
	fprintf(fp, "\n");
	fprintf(fp, "key_repeat.delay=%i\n", sdl_key_repeat.delay);
	fprintf(fp, "key_repeat.interval=%i\n", sdl_key_repeat.interval);
	fprintf(fp, "\n");
	fprintf(fp, "sound.volume=%i\n", sdl_sound.volume);
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
				if (ctrl_remaps[count].components & COMP_RUNOPTS0) {
					if (found) strcat(value, " | ");
					strcat(value, "COMP_RUNOPTS0"); found = TRUE;
				}				
				if (ctrl_remaps[count].components & COMP_RUNOPTS1) {
					if (found) strcat(value, " | ");
					strcat(value, "COMP_RUNOPTS1"); found = TRUE;
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
 * On exit: returns TRUE on error
 *          else FALSE */

int vkeyb_init(void) {
	SDL_Surface *unconverted;
	char filename[256];

	/* Free any existing scaled surface first */
	if (vkeyb.scaled) SDL_FreeSurface(vkeyb.scaled);

	/* Load the appropriate original scale keyboard image once */
	if (vkeyb.original == NULL) {
		strcpy(filename, PACKAGE_DATA_DIR "/");
		if (zx80) {
			strcat(filename, IMG_ZX80_KYBD);
		} else {
			strcat(filename, IMG_ZX81_KYBD);
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
			if (invert_screen) {
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
	control_bar.yoffset = emulator.yoffset - 19 * video.scale;
	if (control_bar.yoffset < 0) control_bar.yoffset = 0;

	return FALSE;
}

