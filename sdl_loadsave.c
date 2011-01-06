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
 * Load File Dialog Directory List Initialise                              *
 ***************************************************************************/
/* This is called from multiple places */

void load_file_dialog_dirlist_init(void) {
	int filetypes;

	if (*sdl_emulator.model == MODEL_ZX80) {
		filetypes = DIRLIST_FILETYPE_ZX80;
	} else {
		filetypes = DIRLIST_FILETYPE_ZX81;
	}

	dirlist_populate(load_file_dialog.dir,
		&load_file_dialog.dirlist, &load_file_dialog.dirlist_sizeof,
		&load_file_dialog.dirlist_count, filetypes);

	load_file_dialog.dirlist_top = 0;
	load_file_dialog.dirlist_selected = 0;
}

/***************************************************************************
 * File Dialog Basename                                                    *
 ***************************************************************************/
/* This will return the basename of a path e.g. "/moo/bah/" returns "bah".
 * It works exactly the same way as the *nix basename command and supports
 * multiple directory delimeters such as "/moo//bah//".
 * 
 * On entry: char *dir = the path to extract the basename from
 *  On exit: returns a pointer to a string containing the extracted basename
 *               (the root directory is returned as "/" not "") */

char *file_dialog_basename(char *dir) {
	static char basename[256];
	int index;

	strcpy(basename, "");
	
	if ((index = strlen(dir))) {
		/* Move leftwards past trailing delimeters */
		while (index > 0 && dir[index - 1] == '/') index--;
		/* Move leftwards up to a delimeter or zero */
		while (index > 0 && dir[index - 1] != '/') index--;
		/* Copy from this point */
		strcpy(basename, dir + index);
	}

	/* Cut trailing delimeters from our copy */
	if ((index = strlen(basename) - 1))
		while (index && basename[index] == '/') basename[index--] = 0;

	return basename;
}

/***************************************************************************
 * File Dialog Change Directory                                            *
 ***************************************************************************/
/* This function will update a target directory string with another source
 * string which could be a relative subdirectory or parent directory.
 * The source directory can be wrapped within brackets which will be removed
 * if found. Multiple directory delimeters are supported e.g. "/moo//bah".
 * 
 * On entry: char *dir = the directory string to update
 *           char *direntry = the relative directory to change to,
 *               examples "(home)", "(..)", "home", ".."
 *  On exit: char *dir will be updated */

void file_dialog_cd(char *dir, char *direntry) {
	char filename[256];
	int index;

	/* Copy the direntry and strip the surrounding brackets if found */
	if (*direntry == '(') {
		strcpy(filename, direntry + 1);
		filename[strlen(filename) - 1] = 0;
	} else {
		strcpy(filename, direntry);
	}

	/* Is this the parent directory? */
	if (strcmp(filename, "..") == 0) {
		/* Go back to the parent directory */
		index = strlen(dir) - 1;
		while (index > 0) {
			if (dir[index] == '/') {
				while (index > 0 && dir[index] == '/') {
					dir[index--] = 0;
				}
				break;
			}
			dir[index--] = 0;
		}
	} else {
		/* It's a subdirectory */
		/* Add a directory delimeter if required */
		if (dir[strlen(dir) - 1] != '/') strcat(dir, "/");
		strcat(dir, filename);
	}
}

/***************************************************************************
 * Directory List Populate                                                 *
 ***************************************************************************/
/* This function will record the current directory, change to the requested
 * directory, create a sorted list of its contents and then change back to
 * the original directory.
 * 
 * Directory names are wrapped within brackets and a parent directory
 * "(..)" is forced to always be present (dirlist will never return NULL).
 * 
 * dirlist will return a pointer to the dynamically allocated memory
 * containing the list and should be freed before program exit.
 * When calling this function, any existing list created from a previous
 * call will be freed automatically so the caller need not do this.
 * 
 * On entry: char *dir = a string containing the directory to list
 *           int filetypes = an OR'd combination of file types to list
 *  On exit: char **dirlist will point to the dynamically allocated
 *               directory list
 *           int *dirlist_sizeof will contain the size of each item
 *           int *dirlist_count will contain the number of items found */

void dirlist_populate(char *dir, char **dirlist, int *dirlist_sizeof,
	int *dirlist_count, int filetypes) {
	char cwd[256], swap[256], *realloclist;
	int parentfound = FALSE, offset = 0;
	int count, found, swapped;
	struct dirent *direntry;
	struct stat filestatus;
	DIR *dirstream;

	/* [Re]initialise the list */
	if (*dirlist) {
		free(*dirlist); *dirlist = NULL;
	}
	*dirlist_sizeof = 0; *dirlist_count = 0;

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
					sdl_filetype_casecmp(direntry->d_name, ".o") == 0) {
					found = TRUE;
				} else if (filetypes & DIRLIST_FILETYPE_ZX80 &&
					sdl_filetype_casecmp(direntry->d_name, ".80") == 0) {
					found = TRUE;
				} else if (filetypes & DIRLIST_FILETYPE_ZX81 &&
					sdl_filetype_casecmp(direntry->d_name, ".p") == 0) {
					found = TRUE;
				} else if (filetypes & DIRLIST_FILETYPE_ZX81 &&
					sdl_filetype_casecmp(direntry->d_name, ".81") == 0) {
					found = TRUE;
				} else if (filetypes & DIRLIST_FILETYPE_TXT &&
					sdl_filetype_casecmp(direntry->d_name, ".txt") == 0) {
					found = TRUE;
				}

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

	} else {
		fprintf(stderr, "%s: Cannot read from directory %s\n", __func__,
			dir);
	}

	/* If opening the dir failed then manually set an element size */
	if (*dirlist_sizeof == 0) *dirlist_sizeof = 256 + 2;

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

	/* Force the parent directory "(..)" to always be the topmost item */
	for (count = 0; count < *dirlist_count; count++) {
		if (strcmp(*dirlist + *dirlist_sizeof * count, "(..)") == 0) {
			strcpy(swap, *dirlist);
			strcpy(*dirlist, *dirlist + *dirlist_sizeof * count);
			strcpy(*dirlist + *dirlist_sizeof * count, swap);
			break;
		}
	}

	/* Bubble sort the directory list following element zero */
	do {
		swapped = FALSE;
		for (count = 1; count < *dirlist_count - 1; count++) {
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

	/* Restore the current working directory */
	chdir(cwd);
}

/***************************************************************************
 * File Type Case Insensitive Compare                                      *
 ***************************************************************************/
/* On exit: returns TRUE on error
 *          else FALSE */

int sdl_filetype_casecmp(char *filename, char *filetype) {
	int retval = FALSE;

	if (filename == NULL || filetype == NULL || strlen(filename) < strlen(filetype) || 
		strcasecmp(filename + strlen(filename) - strlen(filetype), filetype) != 0)
		retval = TRUE;

	return retval;
}


