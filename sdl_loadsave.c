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
	int parentfound = FALSE, offset = 0;
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

		/* Force the parent directory "(..)" to always be the topmost item */
		for (count = 0; count < *dirlist_count; count++) {
			if (strcmp(*dirlist + *dirlist_sizeof * count, "(..)") == 0) {
				strcpy(swap, *dirlist);
				strcpy(*dirlist, *dirlist + *dirlist_sizeof * count);
				strcpy(*dirlist + *dirlist_sizeof * count, swap);
				break;
			}
		}

		/* Bubble sort the directory listing following element zero */
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

	} else {
		fprintf(stderr, "%s: Cannot read from directory %s\n", __func__,
			dir);
	}

	/* Restore the current working directory */
	chdir(cwd);
}


