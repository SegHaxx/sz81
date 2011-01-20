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
char *strtoupper(char *original);
char *strzx81_to_ascii(int memaddr);
char *strzx80_to_ascii(int memaddr);

/***************************************************************************
 * Save File                                                               *
 ***************************************************************************/
/* This function replaces z81's save_p.
 * 
 * On entry: if method = SAVE_FILE_METHOD_NAMEDSAVE then prognameaddr holds
 *             the contents of the hl register pair which points to the area
 *             in the ZX81's memory that contains the program name.
 *           for all other methods prognameaddr is ignored.
 *  On exit: returns TRUE on error
 *           else FALSE */

int sdl_save_file(int prognameaddr, int method) {
	char fullpath[256], filename[256];
	struct MSG_Box msg_box;
	int retval = FALSE;
	FILE *fp;
	#ifndef __amigaos4__
		int index, idxend, vars;
		struct tm *timestruct;
		time_t rightnow;
	#endif

	if (method == SAVE_FILE_METHOD_NAMEDSAVE) {

		#ifdef __amigaos4__
			strcpy(fullpath, "");
		#else
			/* Build a path from the last entered directory */
			strcpy(fullpath, load_file_dialog.dir);
			/* Add a directory delimiter if required */
			strcatdelimiter(fullpath);
		#endif
		/* Add translated program name */
		strcat(fullpath, strzx81_to_ascii(prognameaddr));
		/* Add a file extension if one hasn't already been affixed */
		if (sdl_filetype_casecmp(fullpath, ".p") != 0 &&
			sdl_filetype_casecmp(fullpath, ".81") != 0)
			strcat(fullpath, ".p");

		/* Attempt to open the file */
		if ((fp = fopen(fullpath, "wb")) != NULL) {
			/* Write up to and including E_LINE */
			fwrite(mem + 0x4009, 1, (mem[0x4015] << 8 | mem[0x4014]) - 0x4009, fp);
			/* Close the file now as we've finished with it */
			fclose(fp);
		} else {
			retval = TRUE;
			/* Warn the user via the GUI that the save failed */
			strcpy(msg_box.title, "Save");
			strcpy(msg_box.text, "Failed");
			msg_box.timeout = MSG_BOX_TIMEOUT_SAVE_FAILED;
			message_box_manager(MSG_BOX_SHOW, &msg_box);
		}

	} else if (method == SAVE_FILE_METHOD_UNNAMEDSAVE) {

		#ifdef __amigaos4__
			/* This will return NULL if the user cancelled */
			if ((amiga_file_request_retval = amiga_file_request("")) != NULL) {
				strcpy(fullpath, amiga_file_request_retval);
			} else {
				retval = TRUE;
			}
		#else
			/* Build a path from the last entered directory */
			strcpy(fullpath, load_file_dialog.dir);
			/* Add a directory delimiter if required */
			strcatdelimiter(fullpath);

			/* At the moment I'm not looking to develop a Save As dialog
			 * just for ZX80 files, but I'm happy to implement some other
			 * methods to name the files:
			 * 1. Date and time stamped files e.g. zx80-20110115-234836.o
			 *    (the GP2X won't support this but everything else is OK)
			 * 2. Filenames embedded within a xxxx REM SAVE "progname"
			 *    (works on any platform and is actually quit cool :) ) */
			index = 0x4028;	/* Start of user program area */
			vars = mem[0x4009] << 8 | mem[0x4008];	/* VARS */
			while (index < vars) {
				if (mem[index] == 0xfe && mem[index + 1] == 0xea && 
					mem[index + 2] == 0x01) {	/* REM SAVE " */
					idxend = index = index + 3;	/* Position on first char */
					while (mem[idxend] != 0x01 && mem[idxend] < 0x80 &&
						mem[idxend] != 0x76 && idxend < vars) idxend++;
					if (index < idxend) {
						mem[--idxend] |= 0x80;	/* +80h marks last char */
						strcpy(filename, strzx80_to_ascii(index));
						mem[idxend] &= ~0x80;	/* Remove +80h */
						index = vars;			/* Exit loop */
					}
				}
				index++;
			}
			if (index == vars) {
				/* Create a unique filename using the date and time */
				rightnow = time(NULL);
				timestruct = localtime(&rightnow);
				strftime(filename, sizeof(filename), "zx80-%Y%m%d-%H%M%S.o", timestruct);
			}

			/* Add program name */
			strcat(fullpath, filename);
		#endif

		if (!retval) {

			/* Add a file extension if one hasn't already been affixed */
			if (sdl_filetype_casecmp(fullpath, ".o") != 0 &&
				sdl_filetype_casecmp(fullpath, ".80") != 0)
				strcat(fullpath, ".o");

			/* Attempt to open the file */
			if ((fp = fopen(fullpath, "wb")) != NULL) {
				/* Write up to and including E_LINE */
				fwrite(mem + 0x4000, 1, (mem[0x400b] << 8 | mem[0x400a]) - 0x4000, fp);
				/* Close the file now as we've finished with it */
				fclose(fp);
			} else {
				retval = TRUE;
				/* Warn the user via the GUI that the save failed */
				strcpy(msg_box.title, "Save");
				strcpy(msg_box.text, "Failed");
				msg_box.timeout = MSG_BOX_TIMEOUT_SAVE_FAILED;
				message_box_manager(MSG_BOX_SHOW, &msg_box);
			}

		}

	}

	if (!retval) {
		/* Reinitialise the load file dialog's directory list */
		load_file_dialog_dirlist_init();
	}

	return retval;
}

/***************************************************************************
 * Load File                                                               *
 ***************************************************************************/
/* This function replaces z81's load_p.
 * 
 * On entry: if method = LOAD_FILE_METHOD_NAMEDLOAD then prognameaddr holds
 *             the contents of the hl register pair which points to the area
 *             in the ZX81's memory that contains the program name.
 *           for all other methods prognameaddr is ignored.
 *  On exit: returns TRUE on error
 *           else FALSE */

int sdl_load_file(int prognameaddr, int method) {
	char fullpath[256], filename[256];
	struct MSG_Box msg_box;
	int retval = FALSE;
	int count;
	FILE *fp;

	/* If requested, read and set the preset method instead */
	if (method == LOAD_FILE_METHOD_DETECT) {
		method = load_file_dialog.method;
		load_file_dialog.method = LOAD_FILE_METHOD_NONE;
	}

	if (method == LOAD_FILE_METHOD_AUTOLOAD) {
		/* Check that the file type is compatible with the machine model */
		if ((*sdl_emulator.model == MODEL_ZX80 &&
			(sdl_filetype_casecmp(sdl_com_line.filename, ".o") == 0 ||
			sdl_filetype_casecmp(sdl_com_line.filename, ".80") == 0)) ||
			(*sdl_emulator.model == MODEL_ZX81 &&
			(sdl_filetype_casecmp(sdl_com_line.filename, ".p") == 0 ||
			sdl_filetype_casecmp(sdl_com_line.filename, ".81") == 0))) {
			/* Copy the filename to fullpath which we'll be using below */
			strcpy(fullpath, sdl_com_line.filename);
		} else {
			fprintf(stderr, "%s: File type is incompatible with machine model.\n",
				__func__);
			retval = TRUE;
		}
	} else if (method == LOAD_FILE_METHOD_FORCEDLOAD) {
		#ifdef __amigaos4__
			strcpy(fullpath, amiga_file_request_retval);
		#else
			/* Build a path from the last entered directory */
			strcpy(fullpath, load_file_dialog.dir);
			/* Add a directory delimiter if required */
			strcatdelimiter(fullpath);
			/* Add load_file_dialog.selected item */
			strcat(fullpath, load_file_dialog.dirlist +
				load_file_dialog.dirlist_selected * load_file_dialog.dirlist_sizeof);
		#endif
	} else if (method == LOAD_FILE_METHOD_SELECTLOAD) {
		/* Let the load file dialog know what we are doing */
		load_file_dialog.method = method;

		#ifdef __amigaos4__
			/* This will return NULL if the user cancelled */
			if ((amiga_file_request_retval = amiga_file_request("")) != NULL)
				load_file_dialog.method = LOAD_FILE_METHOD_SELECTLOADOK;
		#else
			/* Show the load file dialog */
			toggle_ldfile_state();

			/* Wait for the user to select Load or Exit
			 * which will close the dialog */
			emulator_hold(&load_file_dialog.state);
		#endif

		/* If Load was selected then the method will have been updated */
		if (load_file_dialog.method == LOAD_FILE_METHOD_SELECTLOADOK) {
			#ifdef __amigaos4__
				strcpy(fullpath, amiga_file_request_retval);
			#else
				/* Build a path from the last entered directory */
				strcpy(fullpath, load_file_dialog.dir);
				/* Add a directory delimiter if required */
				strcatdelimiter(fullpath);
				/* Add load_file_dialog.selected item */
				strcat(fullpath, load_file_dialog.dirlist +
					load_file_dialog.dirlist_selected * load_file_dialog.dirlist_sizeof);
			#endif
		} else {
			retval = TRUE;
		}
	}

	if (!retval) {
		for (count = 0; count < 2; count++) {
			if (method == LOAD_FILE_METHOD_NAMEDLOAD) {
				/* Attempt to open the file firstly in lowercase and then 
				 * in uppercase as program files are obtained in either */

				/* Get translated program name */
				strcpy(filename, strzx81_to_ascii(prognameaddr));
				/* Add a file extension if one hasn't already been affixed */
				if (sdl_filetype_casecmp(filename, ".p") != 0 &&
					sdl_filetype_casecmp(filename, ".81") != 0)
					strcat(filename, ".p");

				/* Convert filename to uppercase on second attempt */
				if (count == 1) strcpy(filename, strtoupper(filename));

				#ifdef __amigaos4__
					strcpy(fullpath, "");
				#else
					/* Build a path from the last entered directory */
					strcpy(fullpath, load_file_dialog.dir);
					/* Add a directory delimiter if required */
					strcatdelimiter(fullpath);
				#endif
				/* Add translated program name */
				strcat(fullpath, filename);
			}

			/* Attempt to open the file */
			if ((fp = fopen(fullpath, "rb")) != NULL) {

				if (method == LOAD_FILE_METHOD_AUTOLOAD || 
					method == LOAD_FILE_METHOD_FORCEDLOAD) {
					/* To duplicate these values: in mainloop in z80.c around
					 * line 189, change #if 0 to #if 1 and recompile. Run
					 * the emulator, load a suitably sized program by typing
					 * LOAD or LOAD "something" and view the console output.
					 * 
					 * It's likely I've been a bit too thorough recording these
					 * values because I know from looking at the ZX81 ROM that
					 * DE points to the program name in memory which would get
					 * overwritten on LOAD and make it redundant, and HL and A
					 * are modified soon after anyway, but there's no harm done.
					 * 
					 * Note that the ZX81's RAMTOP won't be greater than 0x8000
					 * unless POKEd by the user.
					 */
					if (*sdl_emulator.model == MODEL_ZX80) {
						/* Registers (common values) */
						a = 0x00; f = 0x44; b = 0x00; c = 0x00;
						d = 0x07; e = 0xae; h = 0x40; l = 0x2a;
						pc = 0x0283;
						ix = 0x0000; iy = 0x4000; i = 0x0e; r = 0xdd;
						a1 = 0x00; f1 = 0x00; b1 = 0x00; c1 = 0x21;
						d1 = 0xd8; e1 = 0xf0; h1 = 0xd8; l1 = 0xf0;
						iff1 = 0x00; iff2 = 0x00; im = 0x02;
						radjust = 0x6a;
						/* Machine Stack (common values) */
						if (sdl_emulator.ramsize >= 16) {
							sp = 0x8000 - 4;
						} else {
							sp = 0x4000 - 4 + sdl_emulator.ramsize * 1024;
						}
						mem[sp + 0] = 0x47;
						mem[sp + 1] = 0x04;
						mem[sp + 2] = 0xba;
						mem[sp + 3] = 0x3f;
						/* Now override if RAM configuration changes things
						 * (there's a possibility these changes are unimportant) */
						if (sdl_emulator.ramsize == 16) {
							mem[sp + 2] = 0x22;
						}
					} else if (*sdl_emulator.model == MODEL_ZX81) {
						/* Registers (common values) */
						a = 0x0b; f = 0x00; b = 0x00; c = 0x02;
						d = 0x40; e = 0x9b; h = 0x40; l = 0x99;
						pc = 0x0207;
						ix = 0x0281; iy = 0x4000; i = 0x1e; r = 0xdd;
						a1 = 0xf8; f1 = 0xa9; b1 = 0x00; c1 = 0x00;
						d1 = 0x00; e1 = 0x2b; h1 = 0x00; l1 = 0x00;
						iff1 = 0; iff2 = 0; im = 2;
						radjust = 0xa4;
						/* GOSUB Stack (common values) */
						if (sdl_emulator.ramsize >= 16) {
							sp = 0x8000 - 4;
						} else {
							sp = 0x4000 - 4 + sdl_emulator.ramsize * 1024;
						}
						mem[sp + 0] = 0x76;
						mem[sp + 1] = 0x06;
						mem[sp + 2] = 0x00;
						mem[sp + 3] = 0x3e;
						/* Now override if RAM configuration changes things
						 * (there's a possibility these changes are unimportant) */
						if (sdl_emulator.ramsize >= 4) {
							d = 0x43; h = 0x43;
							a1 = 0xec; b1 = 0x81; c1 = 0x02;
							radjust = 0xa9;
						}
						/* System variables */
						mem[0x4000] = 0xff;				/* ERR_NR */
						mem[0x4001] = 0x80;				/* FLAGS */
						mem[0x4002] = sp & 0xff;		/* ERR_SP lo */
						mem[0x4003] = sp >> 8;			/* ERR_SP hi */
						mem[0x4004] = (sp + 4) & 0xff;	/* RAMTOP lo */
						mem[0x4005] = (sp + 4) >> 8;	/* RAMTOP hi */
						mem[0x4006] = 0x00;				/* MODE */
						mem[0x4007] = 0xfe;				/* PPC lo */
						mem[0x4008] = 0xff;				/* PPC hi */
					}
				}

				/* Read in up to sdl_emulator.ramsize K of data */
				if (*sdl_emulator.model == MODEL_ZX80) {
					fread(mem + 0x4000, 1, sdl_emulator.ramsize * 1024, fp);
				} else if (*sdl_emulator.model == MODEL_ZX81) {
					fread(mem + 0x4009, 1, sdl_emulator.ramsize * 1024 - 9, fp);
				}

				/* Close the file now as we've finished with it */
				fclose(fp);

				/* Copy the filename across to the load file dialog as
				 * then we have a record of what was last/is now loaded */
				strcpy(load_file_dialog.filename, fullpath);

				break;

			} else {
				if ((method == LOAD_FILE_METHOD_NAMEDLOAD && count == 1) ||
					method != LOAD_FILE_METHOD_NAMEDLOAD) {
					retval = TRUE;
					break;
				}
			}
		}

		if (retval) {
			if (method == LOAD_FILE_METHOD_AUTOLOAD || 
				method == LOAD_FILE_METHOD_FORCEDLOAD) {
				fprintf(stderr, "%s: Cannot read from %s\n", __func__, fullpath);
			} else if (method == LOAD_FILE_METHOD_NAMEDLOAD || 
				method == LOAD_FILE_METHOD_SELECTLOAD) {
				/* Warn the user via the GUI that the load failed */
				strcpy(msg_box.title, "Load");
				strcpy(msg_box.text, "Failed");
				msg_box.timeout = MSG_BOX_TIMEOUT_LOAD_FAILED;
				message_box_manager(MSG_BOX_SHOW, &msg_box);
			}
		}
	}

	/* We've finished with the load file dialog now */
	load_file_dialog.method = LOAD_FILE_METHOD_NONE;

	/* printf("%s: Was/is loaded: %s\n", __func__, load_file_dialog.filename);	temp temp */

	return retval;
}

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
 * Append Directory Delimiter to String                                    *
 ***************************************************************************/
/* This will append a directory delimiter to a string if one doesn't already
 * exist.
 * The point of this function is to make it easier to change the delimiter
 * character for other platforms. See also #define DIR_DELIMITER_CHAR.
 * 
 * On entry: char *toappendto = the string to append to
 *  On exit: toappendto will have had a delimiter appended if required  */

void strcatdelimiter(char *toappendto) {
	static char delimiter[2] = {DIR_DELIMITER_CHAR, 0};

	if (toappendto[strlen(toappendto) - 1] != DIR_DELIMITER_CHAR)
		strcat(toappendto, delimiter);
}

/***************************************************************************
 * String to Uppercase                                                     *
 ***************************************************************************/
/* This will convert a string into a new uppercase string (the original
 * remains unchanged).
 * 
 * On entry: char *original = the string to convert
 *  On exit: returns a pointer to the converted string  */

char *strtoupper(char *original) {
	static char converted[256];
	int index = 0;

	do {
		converted[index] = toupper(original[index]);
	} while (original[index++] != 0);

	return converted;
}

/***************************************************************************
 * Sinclair ZX81 String to ASCII String                                    *
 ***************************************************************************/
/* This will translate a ZX81 string of characters into an ASCII equivalent.
 * All alphabetical characters are converted to lowercase.
 * Those characters that won't translate are replaced with underscores.
 * 
 * On entry: int memaddr = the string's address within mem[]
 *  On exit: returns a pointer to the translated string */

char *strzx81_to_ascii(int memaddr) {
	static unsigned char zx81table[17] = {'\"', '_', '$', ':', '?', '(',
		')', '>', '<', '=', '+', '-', '*', '/', ';', ',', '.'};
	static char translated[256];
	unsigned char sinchar;
	char asciichar;
	int index = 0;

	do {
		sinchar = mem[memaddr] & 0x7f;
		if (sinchar == 0x00) {
			asciichar = ' ';
		} else if (sinchar >= 0x0b && sinchar <= 0x1b) {
			asciichar = zx81table[sinchar - 0x0b];
		} else if (sinchar >= 0x1c && sinchar <= 0x25) {
			asciichar = '0' + sinchar - 0x1c;
		} else if (sinchar >= 0x26 && sinchar <= 0x3f) {
			asciichar = 'a' + sinchar - 0x26;
		} else {
			asciichar = '_';
		}
		translated[index] = asciichar;
		translated[index++ + 1] = 0;
	} while (mem[memaddr++] < 0x80);

	return translated;
}

/***************************************************************************
 * Sinclair ZX80 String to ASCII String                                    *
 ***************************************************************************/
/* This will translate a ZX80 string of characters into an ASCII equivalent.
 * All alphabetical characters are converted to lowercase.
 * Those characters that won't translate are replaced with underscores.
 * 
 * On entry: int memaddr = the string's address within mem[]
 *  On exit: returns a pointer to the translated string */

char *strzx80_to_ascii(int memaddr) {
	static unsigned char zx81table[16] = {'_', '$', ':', '?', '(', ')',
		'-', '+', '*', '/', '=', '>', '<', ';', ',', '.'};
	static char translated[256];
	unsigned char sinchar;
	char asciichar;
	int index = 0;

	do {
		sinchar = mem[memaddr] & 0x7f;
		if (sinchar == 0x00) {
			asciichar = ' ';
		} else if (sinchar == 0x01) {
			asciichar = '\"';
		} else if (sinchar >= 0x0c && sinchar <= 0x1b) {
			asciichar = zx81table[sinchar - 0x0c];
		} else if (sinchar >= 0x1c && sinchar <= 0x25) {
			asciichar = '0' + sinchar - 0x1c;
		} else if (sinchar >= 0x26 && sinchar <= 0x3f) {
			asciichar = 'a' + sinchar - 0x26;
		} else {
			asciichar = '_';
		}
		translated[index] = asciichar;
		translated[index++ + 1] = 0;
	} while (mem[memaddr++] < 0x80);

	return translated;
}

/***************************************************************************
 * File Dialog Basename                                                    *
 ***************************************************************************/
/* This will return the basename of a path e.g. "/moo/bah/" returns "bah".
 * It works exactly the same way as the *nix basename command and supports
 * multiple directory delimiters such as "/moo//bah//".
 * 
 * On entry: char *dir = the path to extract the basename from
 *  On exit: returns a pointer to a string containing the extracted basename
 *               (the root directory is returned as "/" not "") */

char *file_dialog_basename(char *dir) {
	static char basename[256];
	int index;

	strcpy(basename, "");
	
	if ((index = strlen(dir))) {
		/* Move leftwards past trailing delimiters */
		while (index > 0 && dir[index - 1] == DIR_DELIMITER_CHAR) index--;
		/* Move leftwards up to a delimiter or zero */
		while (index > 0 && dir[index - 1] != DIR_DELIMITER_CHAR) index--;
		/* Copy from this point */
		strcpy(basename, dir + index);
	}

	/* Cut trailing delimiters from our copy */
	if ((index = strlen(basename) - 1))
		while (index && basename[index] == DIR_DELIMITER_CHAR)
			basename[index--] = 0;

	return basename;
}

/***************************************************************************
 * File Dialog Change Directory                                            *
 ***************************************************************************/
/* This function will update a target directory string with another source
 * string which could be a relative subdirectory or parent directory.
 * The source directory can be wrapped within brackets which will be removed
 * if found. Multiple directory delimiters are supported e.g. "/moo//bah".
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
			if (dir[index] == DIR_DELIMITER_CHAR) {
				while (index > 0 && dir[index] == DIR_DELIMITER_CHAR) {
					dir[index--] = 0;
				}
				break;
			}
			dir[index--] = 0;
		}
	} else {
		/* It's a subdirectory */
		/* Add a directory delimiter if required */
		strcatdelimiter(dir);
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


