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

/* Defines */
#if defined(_WIN32)
	#define DIR_DELIMITER_CHAR '\\'
#else
	#define DIR_DELIMITER_CHAR '/'
#endif

#define LDFILE_LIST_H 20
#define LDFILE_SBPGSCRUNIT 19
#define LDFILE_SBXOFFSET (31 * 8 * video.scale)
#define LDFILE_SBUPYOFFSET (2 * 8 * video.scale)
#define LDFILE_SBHDLEYOFFSET (3 * 8 * video.scale)
#define LDFILE_SBPGUPYOFFSET (3 * 8 * video.scale)
#define LDFILE_SBDOWNYOFFSET (21 * 8 * video.scale)

/* Directory List file types */
#define DIRLIST_FILETYPE_HIDDEN 1
#define DIRLIST_FILETYPE_ZX80 2
#define DIRLIST_FILETYPE_ZX81 4
#define DIRLIST_FILETYPE_TXT 8

/* Save state dialog operating modes */
#define SSTATE_MODE_SAVE 0
#define SSTATE_MODE_LOAD 1

/* Variables */
typedef struct {
	int state;
	int xoffset;
	int yoffset;
	char dir[256];			/* The directory that files are loaded from and saved to */
	char *dirlist;			/* A list containing compatible entries from the directory */
	int dirlist_sizeof;		/* The size of each element within the list */
	int dirlist_count;		/* The count of files within the list */
	int dirlist_top;		/* The GUI list top as an index into the list */
	int dirlist_selected;	/* The selected item as an index into the list */
	char loaded[256];		/* The fullpath of the most recently loaded/saved file */
	int method;				/* The loading method to be implemented for certain methods */
	int sbpgscrunit;
} load_file_dialog_;

typedef struct {
	int state;
	int xoffset;
	int yoffset;
	int slots[9];		/* The slots currently saved to (existing state files) */
	int mode;			/* Are we loading or saving */
} save_state_dialog_;

extern load_file_dialog_ load_file_dialog;
extern save_state_dialog_ save_state_dialog;

/* Function prototypes */
void load_file_dialog_dirlist_populate(int refresh);
int save_state_dialog_slots_populate(void);
void strcatdelimiter(char *toappendto);
char *file_dialog_basename(char *dir);
void file_dialog_cd(char *dir, char *direntry);
void dirlist_populate(char *dir, char **dirlist, int *dirlist_sizeof,
	int *dirlist_count, int filetypes);
int get_filename_next_highest(char *dir, char *format);



