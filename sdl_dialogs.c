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

/* Variables */

/* Function prototypes */


/***************************************************************************
 * Directory List Initialise                                               *
 ***************************************************************************/
/* This function will record the current directory, change to the requested
 * directory, create a sorted listing of its contents and then change back
 * to the original directory.
 * 
 * Directory names are wrapped within brackets and a parent directory
 * "(..)" is forced to always be present.
 * 
 * dirlist will return a pointer to the dynamically allocated memory
 * containing the list and should be freed before program exit.
 * When calling this function, any existing list created from a previous
 * call will be freed automatically so the caller need not do this.
 * 
 * On entry: char *dir = a string containing the directory to list
 *           int filetypes = an OR'd combination of file types to list
 *  On exit: char **dirlist will point to the dynamically allocated
 *               directory listing
 *           int *dirlist_sizeof will contain the size of each item
 *           int *dirlist_count will contain the number of items found
 *           int *dirlist_selected will equal zero */

void dirlist_init(char *dir, char **dirlist, int *dirlist_sizeof,
	int *dirlist_count, int *dirlist_selected, int filetypes) {
	char cwd[256], swap[256], *realloclist;
	int count, found, lendirentry, swapped;
	int parentfound = 0, offset = 0;
	struct dirent *direntry;
	struct stat filestatus;
	DIR *dirstream;

	/* [Re]initialise the list */
	if (*dirlist) {
		free(*dirlist); *dirlist = NULL;
	}
	*dirlist_sizeof = 0; *dirlist_count = 0; *dirlist_selected = 0;

	/* Record the current working directory before changing it to dir
	 * (I've found that stat doesn't work unless the dir is changed) */
	strcpy(cwd, ""); getcwd(cwd, 256); cwd[255] = 0; chdir(dir);

	if ((dirstream = opendir(dir))) {

		while ((direntry = readdir(dirstream))) {

			/* Store the size of the list element once */
			if (*dirlist_sizeof == 0)
				*dirlist_sizeof = sizeof(direntry->d_name) + 2;

			/* Get directory entry status information */
			if (stat(direntry->d_name, &filestatus)) {
				fprintf(stderr, "%s: Cannot stat %s\n", __func__, direntry->d_name);
			} else {
				/* Get directory entry string length */
				lendirentry = strlen(direntry->d_name);

				found = FALSE;

				if (strcmp(direntry->d_name, "..") == 0) {
					found = TRUE;
				} else if (strcmp(direntry->d_name, ".") == 0) {
					/* Discard */
				} else if (!(filetypes & DIRLIST_FILETYPE_HIDDEN) &&
					direntry->d_name[0] == '.') {
					/* Discard */
				} else if (S_ISDIR(filestatus.st_mode)) {
					found = TRUE;
				} else if (filetypes & DIRLIST_FILETYPE_ZX80 &&
					lendirentry > 2 && 
					strcasecmp(direntry->d_name + lendirentry - 2, ".o") == 0) {
					found = TRUE;
				} else if (filetypes & DIRLIST_FILETYPE_ZX80 &&
					lendirentry > 3 && 
					strcmp(direntry->d_name + lendirentry - 3, ".80") == 0) {
					found = TRUE;
				} else if (filetypes & DIRLIST_FILETYPE_ZX81 &&
					lendirentry > 2 && 
					strcasecmp(direntry->d_name + lendirentry - 2, ".p") == 0) {
					found = TRUE;
				} else if (filetypes & DIRLIST_FILETYPE_ZX81 &&
					lendirentry > 3 && 
					strcmp(direntry->d_name + lendirentry - 3, ".81") == 0) {
					found = TRUE;
				} else if (filetypes & DIRLIST_FILETYPE_TXT &&
					lendirentry > 4 && 
					strcasecmp(direntry->d_name + lendirentry - 4, ".txt") == 0) {
					found = TRUE;
				}

				/*if ((strcmp(direntry->d_name, "..") == 0) ||	Redundant
					((direntry->d_name[0] != '.') &&
					((S_ISDIR(filestatus.st_mode)) ||
					(lendirentry > 2 && strcasecmp(direntry->d_name + 
					lendirentry - 2, ".p") == 0) ||
					(lendirentry > 3 && strcasecmp(direntry->d_name + 
					lendirentry - 3, ".81") == 0)))) {*/

				if (found) {

					/* Record that the parent directory was found because it's
					 * not always the case that it is (the Zaurus for example) */
					if (strcmp(direntry->d_name, "..") == 0) parentfound = TRUE;

					/* Expand the list by the necessary size (if the list
					 * is currently NULL then realloc acts like malloc) */
					if ((realloclist = realloc(*dirlist, 
						*dirlist_sizeof * (offset + 1))) == NULL) {
						fprintf(stderr, "%s: Cannot expand list\n", __func__);
					} else {
						/* Update list pointer */
						*dirlist = realloclist;

						/* Store the filename and wrap within brackets if dir */
						if (S_ISDIR(filestatus.st_mode)) {
							*(*dirlist + offset) = '(';
							strcpy(*dirlist + offset + 1, direntry->d_name);
							strcat(*dirlist + offset + 1, ")");
						} else {
							strcpy(*dirlist + offset, direntry->d_name);
						}

						(*dirlist_count)++;
						offset += *dirlist_sizeof;
					}
				}
			}
		}
		closedir(dirstream);

		/* If ".." wasn't found then add it to the end of the list */
		if (!parentfound) {
			if ((realloclist = realloc(*dirlist, 
				*dirlist_sizeof * (offset + 1))) == NULL) {
				fprintf(stderr, "%s: Cannot expand list\n", __func__);
			} else {
				/* Update list pointer */
				*dirlist = realloclist;
				/* Store the filename wrapped within brackets */
				strcpy(*dirlist + offset, "(..)");
				(*dirlist_count)++;
			}
		}

		/* Bubble sort the directory list */
		do {
			swapped = FALSE;
			for (count = 0; count < *dirlist_count - 1; count++) {
				if (strcmp(*dirlist + *dirlist_sizeof * count,
					*dirlist + *dirlist_sizeof * (count + 1)) > 0) {
					swapped = TRUE;
					strcpy(swap, *dirlist + *dirlist_sizeof * (count + 1));
					strcpy(*dirlist + *dirlist_sizeof * (count + 1),
						*dirlist + *dirlist_sizeof * count);
					strcpy(*dirlist + *dirlist_sizeof * count, swap);
				}
			}
		}
		while (swapped);

	} else {
		fprintf(stderr, "%s: Cannot read from directory %s\n", __func__,
			dir);
	}

	/* Restore the current working directory */
	chdir(cwd);
}

/*void load_file_dialog_list_fill(void) {	Redundant
	struct dirent *direntry;
	struct stat filestatus;
	int parentfound = 0;
	char *realloclist;
	int lendirentry;
	DIR *dirstream;
	int offset = 0;

	/$ [Re]initialise the list $/
	if (load_file_dialog.list) {
		free(load_file_dialog.list);
		load_file_dialog.list = NULL;
	}
	load_file_dialog.list_element_sizeof = 0;
	load_file_dialog.list_element_count = 0;
	load_file_dialog.list_element_selected = 0;

	if ((dirstream = opendir(load_file_dialog.cwd))) {

		while ((direntry = readdir(dirstream))) {
			/$ Store the size of the list element once $/
			if (load_file_dialog.list_element_sizeof == 0)
				load_file_dialog.list_element_sizeof = sizeof(direntry->d_name) + 2;

			/$ Get directory entry status information $/
			if (stat(direntry->d_name, &filestatus)) {
				fprintf(stderr, "%s: Cannot stat %s\n", __func__, direntry->d_name);
			} else {
				/$ Get directory entry string length $/
				lendirentry = strlen(direntry->d_name);

				if ((strcmp(direntry->d_name, "..") == 0) ||
					((direntry->d_name[0] != '.') &&
					((S_ISDIR(filestatus.st_mode)) ||
					(lendirentry > 2 && strcasecmp(direntry->d_name + 
					lendirentry - 2, ".p") == 0) ||
					(lendirentry > 3 && strcasecmp(direntry->d_name + 
					lendirentry - 3, ".81") == 0)))) {

					/$ Record that the parent directory was found because it's
					 * not always the case that it is (the Zaurus for example) $/
					if (strcmp(direntry->d_name, "..") == 0) parentfound = TRUE;

					/$ Expand the list by the necessary size (if the list
					 * is currently NULL then realloc acts like malloc) $/
					if ((realloclist = realloc(load_file_dialog.list, 
						load_file_dialog.list_element_sizeof * (offset + 1))) == NULL) {
						fprintf(stderr, "%s: Cannot expand list\n", __func__);
					} else {
						/$ Update list pointer $/
						load_file_dialog.list = realloclist;

						/$ Store the filename and wrap within brackets if dir $/
						if (S_ISDIR(filestatus.st_mode)) {
							*(load_file_dialog.list + offset) = '(';
							strcpy(load_file_dialog.list + offset + 1, direntry->d_name);
							strcat(load_file_dialog.list + offset + 1, ")");
						} else {
							strcpy(load_file_dialog.list + offset, direntry->d_name);
						}

						load_file_dialog.list_element_count++;
						offset += load_file_dialog.list_element_sizeof;
					}
				}
			}
		}
		closedir(dirstream);

		/$ If ".." wasn't found then add it to the end of the list $/
		if (!parentfound) {
			if ((realloclist = realloc(load_file_dialog.list, 
				load_file_dialog.list_element_sizeof * (offset + 1))) == NULL) {
				fprintf(stderr, "%s: Cannot expand list\n", __func__);
			} else {
				/$ Update list pointer $/
				load_file_dialog.list = realloclist;
				/$ Store the filename wrapped within brackets $/
				strcpy(load_file_dialog.list + offset, "(..)");
				load_file_dialog.list_element_count++;
			}
		}

		for (offset = 0; offset < load_file_dialog.list_element_count; offset++) {	//temp temp
			printf("%s\n", load_file_dialog.list + load_file_dialog.list_element_sizeof * offset);
		}

	} else {
		fprintf(stderr, "%s: Cannot read from directory %s\n", __func__,
			load_file_dialog.cwd);
	}
}*/

/***************************************************************************
 * Message Box Manager                                                     *
 ***************************************************************************/
/* This recreates and draws an existing message box in the middle of the
 * screen every frame. It is recreated because the fg/bg colours or the
 * resolution could have changed in the meantime - not that it's a problem.
 * 
 * struct MSG_Box {
 *     char title[33];    The title bar text
 *     char text[33];     A single line message
 *     int timeout;       In ms
 * };
 * 
 * On entry: funcid = MSG_BOX_SHOW with
 *                    msg_box pointing to the msg_box to show
 *           funcid = MSG_BOX_SHOW with
 *                    msg_box = NULL to show an existing msgbox
 *           funcid = MSG_BOX_KILL to kill an existing msgbox */

void message_box_manager(int funcid, struct MSG_Box *msg_box) {
	static struct MSG_Box the_box;
	static Uint32 last_time;
	static int init = TRUE;
	int window_x, window_y, window_w, window_h;
	Uint32 colour, fg_colour, bg_colour;
	SDL_Surface *renderedtext, *shadow;
	SDL_Rect dstrect;
	int count;

	if (init) {
		init = FALSE;
		the_box.title[0] = 0; the_box.text[0] = 0; the_box.timeout = 0;
	}

	if (funcid == MSG_BOX_SHOW && msg_box != NULL) {
		/* Initialise a new msgbox / overwrite an existing msgbox */
		the_box = *msg_box;
		last_time = SDL_GetTicks();
	} else if (funcid == MSG_BOX_SHOW && msg_box == NULL) {
		/* Show an existing msgbox */
		if (the_box.timeout > 0) {
			/* Prepare the colours we shall be using */
			if (!sdl_emulator.invert) {
				fg_colour = colours.emu_fg; bg_colour = colours.emu_bg;
			} else {
				fg_colour = colours.emu_bg; bg_colour = colours.emu_fg;
			}
			/* window_x/y/w/h will be the reference coords of the window
			 * that everything else will be relative to */
			if (strlen(the_box.title) > strlen(the_box.text)) {
				window_w = (strlen(the_box.title) + 2) * 8 * video.scale;
			} else {
				window_w = (strlen(the_box.text) + 2) * 8 * video.scale;
			}
			window_h = 4.5 * 8 * video.scale;
			window_x = video.screen->w / 2 - (window_w + 8 * video.scale) / 2;
			window_y = video.screen->h / 2 - (window_h + 8 * video.scale) / 2;
			/* Draw the window background */
			dstrect.x = window_x; dstrect.y = window_y; 
			dstrect.w = window_w; dstrect.h = window_h; 
			if (SDL_FillRect(video.screen, &dstrect, SDL_MapRGB(video.screen->format,
				fg_colour >> 16 & 0xff, fg_colour >> 8 & 0xff, fg_colour & 0xff)) < 0) {
				fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			/* Invert the colours */
			colour = fg_colour; fg_colour = bg_colour; bg_colour = colour;
			/* Write the title */
			renderedtext = BMF_RenderText(BMF_FONT_ZX82, the_box.title, fg_colour, bg_colour);
			dstrect.x = window_x + 8 * video.scale; dstrect.y = window_y;
			dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
			if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			SDL_FreeSurface(renderedtext);
			/* Draw the window interior */
			dstrect.x = window_x + 4 * video.scale; dstrect.y = window_y + 8 * video.scale;
			dstrect.w = window_w - 8 * video.scale; dstrect.h = window_h - 1.5 * 8 * video.scale; 
			if (SDL_FillRect(video.screen, &dstrect, SDL_MapRGB(video.screen->format,
				fg_colour >> 16 & 0xff, fg_colour >> 8 & 0xff, fg_colour & 0xff)) < 0) {
				fprintf(stderr, "%s: FillRect error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			/* Invert the colours */
			colour = fg_colour; fg_colour = bg_colour; bg_colour = colour;
			/* Write the text */
			renderedtext = BMF_RenderText(BMF_FONT_ZX82, the_box.text, fg_colour, bg_colour);
			dstrect.x = window_x + 8 * video.scale; dstrect.y = window_y + 2 * 8 * video.scale;
			dstrect.w = renderedtext->w; dstrect.h = renderedtext->h;
			if (SDL_BlitSurface (renderedtext, NULL, video.screen, &dstrect) < 0) {
				fprintf(stderr, "%s: BlitSurface error: %s\n", __func__, SDL_GetError ());
				exit(1);
			}
			SDL_FreeSurface(renderedtext);
			/* Draw the right-hand and bottom shadows */
			for (count = 0; count < 2; count++) {

				/* I want to document some odd behaviour that I experienced here
				 * on my GP2X using the GPH SDK, but not anything else:
				 * Originally window_x/y/w/h was an SDL_Rect winrect and I found 
				 * that at this point winrect.y was 98 and after this 'if' 
				 * statement it would become 0! It doesn't make any sense at all 
				 * as only dstrect is being written to and I found a way to fix 
				 * this issue by moving SDL_Rect winrect out of this function to 
				 * the top of this file. I decided to simply not use an SDL_Rect
				 * to store data and replaced it with four integers instead.
				 * Since I've only experienced this with the GPH SDK I'm expecting
				 * that there's an issue with that and not anything else */

				if (count == 0) {
					dstrect.w = 8 * video.scale; dstrect.h = window_h;
				} else {
					dstrect.w = window_w - 8 * video.scale; dstrect.h = 8 * video.scale;
				}
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
				if (SDL_SetAlpha(shadow, SDL_SRCALPHA, 64) < 0) {	/* 25% */
					fprintf(stderr, "%s: Cannot set surface alpha: %s\n", __func__,
						SDL_GetError());
					exit(1);
				}
				/* Blit it to the screen */
				if (count == 0) {
					dstrect.x = window_x + window_w; dstrect.y = window_y + 8 * video.scale;
				} else {
					dstrect.x = window_x + 8 * video.scale; dstrect.y = window_y + window_h;
				}
				dstrect.w = shadow->w; dstrect.h = shadow->h;
				if (SDL_BlitSurface (shadow, NULL, video.screen, &dstrect) < 0) {
					fprintf(stderr, "%s: BlitSurface error: %s\n", __func__,
						SDL_GetError ());
					exit(1);
				}
				SDL_FreeSurface(shadow);
			}
			/* Decrement the timeout */			
			the_box.timeout -= SDL_GetTicks() - last_time;
			last_time = SDL_GetTicks();
		}
	} else if (funcid == MSG_BOX_KILL) {
		/* Kill an existing msgbox */
		the_box.timeout = 0;
	}
}


