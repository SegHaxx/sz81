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

/* Defines */
#define DIR_DELIMITER_CHAR '/'

/* Directory List file types */
#define DIRLIST_FILETYPE_HIDDEN 1
#define DIRLIST_FILETYPE_ZX80 2
#define DIRLIST_FILETYPE_ZX81 4
#define DIRLIST_FILETYPE_TXT 8

/* Variables */
struct MSG_Box {
	char title[33];		/* The title bar text */
	char text[33];		/* A single line message */
	int timeout;		/* In ms */
};

struct {
	int state;
	int xoffset;
	int yoffset;
	char dir[256];
	char *dirlist;
	int dirlist_sizeof;
	int dirlist_count;
	int dirlist_top;
	int dirlist_selected;
	char filename[256];
	int method;
} load_file_dialog;

/* Function prototypes */
void load_file_dialog_dirlist_init(void);
void strcatdelimiter(char *toappendto);
char *file_dialog_basename(char *dir);
void file_dialog_cd(char *dir, char *direntry);
void dirlist_populate(char *dir, char **dirlist, int *dirlist_sizeof,
	int *dirlist_count, int filetypes);



