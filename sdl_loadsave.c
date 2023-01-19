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

/* Includes */
#include "sdl_engine.h"

/* Defines */

/* Variables */

/* Function prototypes */
char *strtoupper(char *original);
char *strzx81_to_ascii(int memaddr);
char *strzx80_to_ascii(int memaddr);
void fwrite_unsigned_short_little_endian(unsigned short *source, FILE *fp);
void fwrite_int_little_endian(int *source, FILE *fp);
void fwrite_unsigned_long_little_endian(unsigned long *source, FILE *fp);
void fread_unsigned_short_little_endian(unsigned short *target, FILE *fp);
void fread_int_little_endian(int *target, FILE *fp);
void fread_unsigned_long_little_endian(unsigned long *target, FILE *fp);


/***************************************************************************
 * Load File Dialog Directory List Populate                                *
 ***************************************************************************/
/* This is called from multiple places.
 * 
 * On entry: int refresh = TRUE if this is just a directory refresh
 *                         else FALSE */

void load_file_dialog_dirlist_populate(int refresh) {
	int filetypes;

	if (*sdl_emulator.model == MODEL_ZX80) {
		filetypes = DIRLIST_FILETYPE_ZX80;
	} else {
		filetypes = DIRLIST_FILETYPE_ZX81;
	}

	/* Repopulate the load file dialog's directory list */
	dirlist_populate(load_file_dialog.dir,
		&load_file_dialog.dirlist, &load_file_dialog.dirlist_sizeof,
		&load_file_dialog.dirlist_count, filetypes);

	/* If this is just a refresh then don't reset these */
	if (!refresh) {
		load_file_dialog.dirlist_top = 0;
		load_file_dialog.dirlist_selected = 0;
	}
}

/***************************************************************************
 * Save State Dialog Slots Populate                                        *
 ***************************************************************************/
/* This function will open the currently loaded program's save state folder
 * and create it if it's not found to exist. It will then scan it looking
 * for savsta1.ss{o|p} to savsta9.ss{o|p} */

int save_state_dialog_slots_populate(void) {
	struct dirent *direntry;
	char foldername[256];
	char parentname[256];
	int retval = FALSE;
	int count, index;
	DIR *dirstream;

	/* Wipe the slots ready for fresh data */
	for (count = 0; count < 9; count++)
		save_state_dialog.slots[count] = 0;

	/* Build a path to the currently loaded program's save state folder */
	#if defined(PLATFORM_GP2X) || defined(__amigaos4__) || defined(_WIN32)
		strcpy(foldername, LOCAL_DATA_DIR);
	#else
		strcpy(foldername, getenv ("HOME"));
		strcatdelimiter(foldername);
		strcat(foldername, LOCAL_DATA_DIR);
	#endif
	strcatdelimiter(foldername);
	strcat(foldername, LOCAL_SAVSTA_DIR);
	strcatdelimiter(foldername);

	/* Here I'm going to create a single letter parent directory which
	 * is the first letter of the program name. Due to some nonsense
	 * experienced with my GP2X FAT32 Sandisk SD card creating m instead
	 * of M, showing m in termula2x but M plugged into my card reader on
	 * my Linux desktop computer I'm going to force the parent to lower-
	 * case so that MazezaM.p will save as local/m/MazezaM.p and fingers-
	 * crossed it'll universally work */
	foldername[index = strlen(foldername)] = 
		tolower(file_dialog_basename(load_file_dialog.loaded)[0]);
	foldername[++index] = 0;
	strcpy(parentname, foldername);

	/* parentname will now be something like local/m and
	 * foldername will now be something like local/m/MazezaM.p */
	strcatdelimiter(foldername);
	strcat(foldername, file_dialog_basename(load_file_dialog.loaded));

	/* Firstly attempt to open the folder */
	if ((dirstream = opendir(foldername)) == NULL) {
		/* The path doesn't yet exist so attempt to create everything
		 * from parentname and onwards */
		mkdir(parentname, 0755);
		mkdir(foldername, 0755);
		/* Attempt to open the newly created folder */
		dirstream = opendir(foldername);
	}

	if (dirstream != NULL) {
		while ((direntry = readdir(dirstream))) {
			if (strlen(direntry->d_name) >= 5 &&
				direntry->d_name[strlen(direntry->d_name) - 5] >= '1' &&
				direntry->d_name[strlen(direntry->d_name) - 5] <= '9' &&
				((*sdl_emulator.model == MODEL_ZX80 &&
				sdl_filetype_casecmp(direntry->d_name, ".sso") == 0) ||
				(*sdl_emulator.model == MODEL_ZX81 &&
				sdl_filetype_casecmp(direntry->d_name, ".ssp") == 0))) {
				save_state_dialog.slots[direntry->d_name
					[strlen(direntry->d_name) - 5] - '1'] = TRUE;
			}
		}
		closedir(dirstream);
	} else {
		retval = TRUE;
		fprintf(stderr, "%s: Cannot read from directory %s\n", __func__,
			foldername);
	}

	return retval;
}

/***************************************************************************
 * Save File                                                               *
 ***************************************************************************/
/* This function replaces z81's save_p.
 * 
 * On entry: if method = SAVE_FILE_METHOD_NAMEDSAVE then parameter holds
 *               the contents of the hl register pair which points to the
 *               area in the ZX81's memory that contains the program name.
 *           if method = SAVE_FILE_METHOD_STATESAVE then parameter holds
 *               the slot number 0 to 8.
 *           for all other methods parameter is ignored.
 *  On exit: returns TRUE on error
 *           else FALSE */

int sdl_save_file(int parameter, int method) {
	char fullpath[256], filename[256];
	struct Notification notification;
	int retval = FALSE;
	int index;
	FILE *fp;
	#ifndef __amigaos4__
		/*struct tm *timestruct;	Redundant
		time_t rightnow;*/
		int idxend, vars;
	#endif

	if (method == SAVE_FILE_METHOD_NAMEDSAVE) {
		/* Build a path from the last entered directory */
		strcpy(fullpath, load_file_dialog.dir);
		/* Add a directory delimiter if required */
		strcatdelimiter(fullpath);
#if 0
		/* Add the local program dir.  This will only really work
		 * on platforms that aren't changing load_file_dialog.dir */
		#ifdef __amigaos4__
			strcat(fullpath, LOCAL_PROGRM_DIR);
			strcatdelimiter(fullpath);
		#endif
#endif
		/* Add translated program name */
		strcat(fullpath, strzx81_to_ascii(parameter));
		/* Add a file extension if one hasn't already been affixed */
		if (sdl_filetype_casecmp(fullpath, ".p") != 0 &&
			sdl_filetype_casecmp(fullpath, ".81") != 0)
			strcat(fullpath, ".p");
	} else if (method == SAVE_FILE_METHOD_UNNAMEDSAVE) {
		#ifdef __amigaos4__
			/* This will return NULL if the user cancelled */
			if ((amiga_file_request_retval = amiga_file_request("SZ81: Save file", TRUE)) != NULL) {
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
			 * 2. Incremented filenames e.g. zx80-0001.o, zx80-0002.o etc.
			 * 3. Filenames embedded within a xxxx REM SAVE "progname"
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
				/* Create a unique filename using the date and time		Redundant
				rightnow = time(NULL);
				timestruct = localtime(&rightnow);
				strftime(filename, sizeof(filename), "zx80-%Y%m%d-%H%M%S.o", timestruct);*/

				/* Create a unique filename using the next highest number
				 * (it'll return 0 if the directory couldn't be opened or 1 as
				 * the base number when no files exist that match the pattern) */
				index = get_filename_next_highest(fullpath, "zx80prog%4d");
				sprintf(filename, "zx80prog%04i.o", index);
			}

			/* Add program name */
			strcat(fullpath, filename);
		#endif
		if (!retval) {
			/* Add a file extension if one hasn't already been affixed */
			if (sdl_filetype_casecmp(fullpath, ".o") != 0 &&
				sdl_filetype_casecmp(fullpath, ".80") != 0)
				strcat(fullpath, ".o");
		}
	} else if (method == SAVE_FILE_METHOD_STATESAVE) {
		/* Build a path to the currently loaded program's save state folder */
		#if defined(PLATFORM_GP2X) || defined(__amigaos4__) || defined(_WIN32)
			strcpy(fullpath, LOCAL_DATA_DIR);
		#else
			strcpy(fullpath, getenv ("HOME"));
			strcatdelimiter(fullpath);
			strcat(fullpath, LOCAL_DATA_DIR);
		#endif
		strcatdelimiter(fullpath);
		strcat(fullpath, LOCAL_SAVSTA_DIR);
		strcatdelimiter(fullpath);
		fullpath[index = strlen(fullpath)] = 
			tolower(file_dialog_basename(load_file_dialog.loaded)[0]);
		fullpath[++index] = 0;
		strcatdelimiter(fullpath);
		strcat(fullpath, file_dialog_basename(load_file_dialog.loaded));
		strcatdelimiter(fullpath);
		/* Form an appropriate filename */
		if (*sdl_emulator.model == MODEL_ZX80) {
			sprintf(filename, "savsta%i.sso", parameter + 1);
		} else if (*sdl_emulator.model == MODEL_ZX81) {
			sprintf(filename, "savsta%i.ssp", parameter + 1);
		}
		/* Append filename to fullpath */
		strcat(fullpath, filename);
	}

	if (!retval) {
		/* Attempt to open the file */
		if ((fp = fopen(fullpath, "wb")) != NULL) {
			if (method == SAVE_FILE_METHOD_STATESAVE) {
				/* Printer variables are reinitialised when a new file
				 * is opened on output so saving them is futile.
				 * Saving keyports and sound variables is unneccessary.
				 * signal_int_flag is being updated by the SDL timer
				 * likely in another thread so no point in saving that.
				 * refresh_screen I'm forcing to 1 anyway.
				 * 
				 * To make these files platform independent I'm saving
				 * and restoring everything by the byte in little-endian
				 * format. These are the integer sizes on my development
				 * computer (GNU/Linux 32bit):
				 * 
				 * sizeof(long) = 4 bytes
				 * sizeof(int) = 4 bytes
				 * sizeof(short) = 2 bytes
				 * sizeof(char) = 1 byte */

				/* The entire contents of memory */
				fwrite(mem, 1, 64 * 1024, fp);	/* unsigned char */

				/* Variables from the top of z80.c */
				fwrite_unsigned_long_little_endian(&tstates, fp);
				fwrite_unsigned_long_little_endian(&frames, fp);
				fwrite_int_little_endian(&liney, fp);
				fwrite_int_little_endian(&vsy, fp);
				fwrite_unsigned_long_little_endian(&linestart, fp);
				fwrite_int_little_endian(&vsync_toggle, fp);
				fwrite_int_little_endian(&vsync_lasttoggle, fp);

				/* Variables liberated from the top of mainloop */
				fwrite(&a, 1, 1, fp);	/* unsigned char */
				fwrite(&f, 1, 1, fp);
				fwrite(&b, 1, 1, fp); 
				fwrite(&c, 1, 1, fp);
				fwrite(&d, 1, 1, fp); 
				fwrite(&e, 1, 1, fp);
				fwrite(&h, 1, 1, fp); 
				fwrite(&l, 1, 1, fp);
				fwrite(&r, 1, 1, fp);
				fwrite(&a1, 1, 1, fp); 
				fwrite(&f1, 1, 1, fp);
				fwrite(&b1, 1, 1, fp); 
				fwrite(&c1, 1, 1, fp);
				fwrite(&d1, 1, 1, fp); 
				fwrite(&e1, 1, 1, fp);
				fwrite(&h1, 1, 1, fp); 
				fwrite(&l1, 1, 1, fp);
				fwrite(&i, 1, 1, fp); 
				fwrite(&iff1, 1, 1, fp); 
				fwrite(&iff2, 1, 1, fp);
				fwrite(&im, 1, 1, fp); 
				fwrite_unsigned_short_little_endian(&pc, fp);
				fwrite_unsigned_short_little_endian(&ix, fp);
				fwrite_unsigned_short_little_endian(&iy, fp);
				fwrite_unsigned_short_little_endian(&sp, fp);
				fwrite(&radjust, 1, 1, fp);	/* unsigned char */
				fwrite_unsigned_long_little_endian(&nextlinetime, fp);
				fwrite_unsigned_long_little_endian(&linegap, fp);
				fwrite_unsigned_long_little_endian(&lastvsyncpend, fp);
				fwrite(&ixoriy, 1, 1, fp);	/* unsigned char */
				fwrite(&new_ixoriy, 1, 1, fp);
				fwrite(&intsample, 1, 1, fp);
				fwrite(&op, 1, 1, fp);
				fwrite_int_little_endian(&ulacharline, fp);
				fwrite_int_little_endian(&nmipend, fp);
				fwrite_int_little_endian(&intpend, fp);
				fwrite_int_little_endian(&vsyncpend, fp);
				fwrite_int_little_endian(&vsynclen, fp);
				fwrite_int_little_endian(&hsyncskip, fp);
				fwrite_int_little_endian(&framewait, fp);

				/* Variables from the top of common.c */
				fwrite_int_little_endian(&interrupted, fp);
				fwrite_int_little_endian(&nmigen, fp);
				fwrite_int_little_endian(&hsyncgen, fp);
				fwrite_int_little_endian(&vsync, fp);

				/* 65654/0x10076 bytes to here for 2.1.7 */

			} else {
				/* Write up to and including E_LINE */
				if (*sdl_emulator.model == MODEL_ZX80) {
					fwrite(mem + 0x4000, 1, (mem[0x400b] << 8 | mem[0x400a]) - 0x4000, fp);
				} else if (*sdl_emulator.model == MODEL_ZX81) {
					fwrite(mem + 0x4009, 1, (mem[0x4015] << 8 | mem[0x4014]) - 0x4009, fp);
				}
				/* Copy fullpath across to the load file dialog as
				 * then we have a record of what was last saved */
				strcpy(load_file_dialog.loaded, fullpath);
			}
			/* Close the file now as we've finished with it */
			fclose(fp);
		} else {
			retval = TRUE;
			/* Warn the user via the GUI that the save failed */
			if (method == SAVE_FILE_METHOD_STATESAVE) {
				strcpy(notification.title, "Save State");
			} else {
				strcpy(notification.title, "Save");
			}
			strcpy(notification.text, "Failed");
			notification.timeout = NOTIFICATION_TIMEOUT_1250;
			notification_show(NOTIFICATION_SHOW, &notification);
		}
	}

	if (!retval) {
		if (method != SAVE_FILE_METHOD_STATESAVE) {
			/* Refresh the load file dialog's directory list */
			load_file_dialog_dirlist_populate(TRUE);
		}
	}

	return retval;
}

/***************************************************************************
 * Load File                                                               *
 ***************************************************************************/
/* This function replaces z81's load_p.
 * 
 * On entry: if method = LOAD_FILE_METHOD_NAMEDLOAD then parameter holds
 *               the contents of the hl register pair which points to the
 *               area in the ZX81's memory that contains the program name.
 *           if method = LOAD_FILE_METHOD_STATELOAD then parameter holds
 *               the slot number 0 to 8.
 *           for all other methods parameter is ignored.
 *  On exit: returns TRUE on error
 *           else FALSE */

int sdl_load_file(int parameter, int method) {
	char fullpath[256], filename[256];
	struct Notification notification;
	int retval = FALSE;
	int count, index;
	int ramsize;
	FILE *fp;
	char *scp = NULL;
	int addr;

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
			if ((amiga_file_request_retval = amiga_file_request("SZ81: Load file", FALSE)) != NULL)
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
	} else if (method == LOAD_FILE_METHOD_STATELOAD) {
		/* Build a path to the currently loaded program's save state folder */
		#if defined(PLATFORM_GP2X) || defined(__amigaos4__) || defined(_WIN32)
			strcpy(fullpath, LOCAL_DATA_DIR);
		#else
			strcpy(fullpath, getenv ("HOME"));
			strcatdelimiter(fullpath);
			strcat(fullpath, LOCAL_DATA_DIR);
		#endif
		strcatdelimiter(fullpath);
		strcat(fullpath, LOCAL_SAVSTA_DIR);
		strcatdelimiter(fullpath);
		fullpath[index = strlen(fullpath)] = 
			tolower(file_dialog_basename(load_file_dialog.loaded)[0]);
		fullpath[++index] = 0;
		strcatdelimiter(fullpath);
		strcat(fullpath, file_dialog_basename(load_file_dialog.loaded));
		strcatdelimiter(fullpath);
		/* Form an appropriate filename */
		if (*sdl_emulator.model == MODEL_ZX80) {
			sprintf(filename, "savsta%i.sso", parameter + 1);
		} else if (*sdl_emulator.model == MODEL_ZX81) {
			sprintf(filename, "savsta%i.ssp", parameter + 1);
		}
		/* Append filename to fullpath */
		strcat(fullpath, filename);
	}

	if (!retval) {
		for (count = 0; count < 2; count++) {
			if (method == LOAD_FILE_METHOD_NAMEDLOAD) {
				/* Attempt to open the file firstly in lowercase and then 
				 * in uppercase as program files are obtained in either */

				/* Get translated program name */
				strcpy(filename, strzx81_to_ascii(parameter));
				scp = strrchr(filename,';');
				if (scp) {
					sscanf(scp+1,"%d",&addr);
					if (!addr) scp = NULL; else strcpy(scp,"");
				}
				/* Add a file extension if one hasn't already been affixed */
				if (sdl_filetype_casecmp(filename, ".p") != 0 &&
					sdl_filetype_casecmp(filename, ".81") != 0 && !scp)
					strcat(filename, ".p");

				/* Convert filename to uppercase on second attempt */
				if (count == 1) strcpy(filename, strtoupper(filename));

				/* Build a path from the last entered directory */
				strcpy(fullpath, load_file_dialog.dir);
				/* Add a directory delimiter if required */
				strcatdelimiter(fullpath);
#if 0
				/* Add the local program dir.  This will only really work
				 * on platforms that aren't changing load_file_dialog.dir */
				#ifdef __amigaos4__
					strcat(fullpath, LOCAL_PROGRM_DIR);
					strcatdelimiter(fullpath);
				#endif
#endif
				/* Add translated program name */
				strcat(fullpath, filename);
			}


printf("Opening file %s...\n",fullpath);

			/* Attempt to open the file */
			if ((fp = fopen(fullpath, "rb")) != NULL) {
				if (method == LOAD_FILE_METHOD_STATELOAD) {
					/* Printer variables are reinitialised when a new file
					 * is opened on output so saving them is futile.
					 * Saving keyports and sound variables is unneccessary.
					 * signal_int_flag is being updated by the SDL timer
					 * likely in another thread so no point in saving that.
					 * refresh_screen I'm forcing to 1 anyway.
					 * 
					 * To make these files platform independent I'm saving
					 * and restoring everything by the byte in little-endian
					 * format. These are the integer sizes on my development
					 * computer (GNU/Linux 32bit):
					 * 
					 * sizeof(long) = 4 bytes
					 * sizeof(int) = 4 bytes
					 * sizeof(short) = 2 bytes
					 * sizeof(char) = 1 byte */

					/* The entire contents of memory */
					fread(mem, 1, 64 * 1024, fp);	/* unsigned char */

					/* Variables from the top of z80.c */
					fread_unsigned_long_little_endian(&tstates, fp);
					fread_unsigned_long_little_endian(&frames, fp);
					fread_int_little_endian(&liney, fp);
					fread_int_little_endian(&vsy, fp);
					fread_unsigned_long_little_endian(&linestart, fp);
					fread_int_little_endian(&vsync_toggle, fp);
					fread_int_little_endian(&vsync_lasttoggle, fp);

					/* Variables liberated from the top of mainloop */
					fread(&a, 1, 1, fp);	/* unsigned char */
					fread(&f, 1, 1, fp);
					fread(&b, 1, 1, fp); 
					fread(&c, 1, 1, fp);
					fread(&d, 1, 1, fp); 
					fread(&e, 1, 1, fp);
					fread(&h, 1, 1, fp); 
					fread(&l, 1, 1, fp);
					fread(&r, 1, 1, fp);
					fread(&a1, 1, 1, fp); 
					fread(&f1, 1, 1, fp);
					fread(&b1, 1, 1, fp); 
					fread(&c1, 1, 1, fp);
					fread(&d1, 1, 1, fp); 
					fread(&e1, 1, 1, fp);
					fread(&h1, 1, 1, fp); 
					fread(&l1, 1, 1, fp);
					fread(&i, 1, 1, fp); 
					fread(&iff1, 1, 1, fp); 
					fread(&iff2, 1, 1, fp);
					fread(&im, 1, 1, fp); 
					fread_unsigned_short_little_endian(&pc, fp);
					fread_unsigned_short_little_endian(&ix, fp);
					fread_unsigned_short_little_endian(&iy, fp);
					fread_unsigned_short_little_endian(&sp, fp);
					fread(&radjust, 1, 1, fp);	/* unsigned char */
					fread_unsigned_long_little_endian(&nextlinetime, fp);
					fread_unsigned_long_little_endian(&linegap, fp);
					fread_unsigned_long_little_endian(&lastvsyncpend, fp);
					fread(&ixoriy, 1, 1, fp);	/* unsigned char */
					fread(&new_ixoriy, 1, 1, fp);
					fread(&intsample, 1, 1, fp);
					fread(&op, 1, 1, fp);
					fread_int_little_endian(&ulacharline, fp);
					fread_int_little_endian(&nmipend, fp);
					fread_int_little_endian(&intpend, fp);
					fread_int_little_endian(&vsyncpend, fp);
					fread_int_little_endian(&vsynclen, fp);
					fread_int_little_endian(&hsyncskip, fp);
					fread_int_little_endian(&framewait, fp);

					/* Variables from the top of common.c */
					fread_int_little_endian(&interrupted, fp);
					fread_int_little_endian(&nmigen, fp);
					fread_int_little_endian(&hsyncgen, fp);
					fread_int_little_endian(&vsync, fp);

					/* 65654/0x10076 bytes to here for 2.1.7 */

				} else {
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
						} else if (*sdl_emulator.model == MODEL_ZX81 && !scp) {
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

					/* Read in up to 48K of data */
					if (sdl_emulator.ramsize > 48) {
						ramsize = 48;
					} else {
						ramsize = sdl_emulator.ramsize;
					}
					if (*sdl_emulator.model == MODEL_ZX80) {
						fread(mem + 0x4000, 1, ramsize * 1024, fp);
					} else if (*sdl_emulator.model == MODEL_ZX81) {
						if (scp)
							fread(mem + addr, 1, ramsize * 1024, fp);
						else
							fread(mem + 0x4009, 1, ramsize * 1024 - 9, fp);
					}
					/* Copy fullpath across to the load file dialog as
					 * then we have a record of what was last loaded */
					strcpy(load_file_dialog.loaded, fullpath);
				}
				/* Close the file now as we've finished with it */
				fclose(fp);
				break;
			} else {
				if (!(method == LOAD_FILE_METHOD_NAMEDLOAD && count == 0)) {
					retval = TRUE;
					break;
				}
			}
		}

		if (retval) {
			if (method == LOAD_FILE_METHOD_AUTOLOAD) {
				fprintf(stderr, "%s: Cannot read from %s\n", __func__, fullpath);
			} else if (method == LOAD_FILE_METHOD_NAMEDLOAD || 
				method == LOAD_FILE_METHOD_SELECTLOAD || 
				method == LOAD_FILE_METHOD_FORCEDLOAD || 
				method == LOAD_FILE_METHOD_STATELOAD) {
				/* Warn the user via the GUI that the load failed */
				if (method == LOAD_FILE_METHOD_STATELOAD) {
					strcpy(notification.title, "Load State");
				} else {
					strcpy(notification.title, "Load");
				}
				strcpy(notification.text, "Failed");
				notification.timeout = NOTIFICATION_TIMEOUT_1250;
				notification_show(NOTIFICATION_SHOW, &notification);
			}
		}
	}

	if (method != LOAD_FILE_METHOD_STATELOAD) {
		/* We've finished with the load file dialog now */
		load_file_dialog.method = LOAD_FILE_METHOD_NONE;
	}

	return retval;
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

	#if defined(__amigaos4__)
		if (toappendto[strlen(toappendto) - 1] != DIR_DELIMITER_CHAR &&
			toappendto[strlen(toappendto) - 1] != ':')
			strcat(toappendto, delimiter);
	#else
		if (toappendto[strlen(toappendto) - 1] != DIR_DELIMITER_CHAR)
			strcat(toappendto, delimiter);
	#endif
}

/***************************************************************************
 * File Dialog Basename                                                    *
 ***************************************************************************/
/* This will return the basename of a path e.g. "/moo/bah" returns "bah".
 * The paths being passed are coming from getcwd or something other than
 * user written so it is not expecting a trailing directory delimiter.
 * getcwd for example will not add a trailing delimiter unless the path
 * is at the root, therefore that's how it will be interpreted here.
 * 
 * On entry: char *dir = the path to extract the basename from
 *  On exit: returns a pointer to a string containing the extracted basename */

char *file_dialog_basename(char *dir) {
	static char basename[256];
	int index;

	strcpy(basename, dir);	/* We'll work with a copy */

	if ((index = strlen(dir))) {
		/* Move leftwards up to a delimiter, root or nothing */
		while (index >= 1 && 
			basename[index - 1] != DIR_DELIMITER_CHAR
		#if defined(__amigaos4__)
			&& basename[index - 1] != ':'
		#endif
			) index--;

		/* Do we need to return root? */
		if (index >= 1 && basename[index] == 0 &&
		#if defined(__amigaos4__)
			basename[index - 1] == ':'
		#else
			basename[index - 1] == DIR_DELIMITER_CHAR
		#endif
			) index--;
	}

	return basename + index;
}

/***************************************************************************
 * File Dialog Change Directory                                            *
 ***************************************************************************/
/* This function will update a target directory string with another source
 * string which could be a relative subdirectory or parent directory "..".
 * The source directory can be wrapped within brackets which will be removed
 * if found e.g. "(sz81)", "sz81", "(..)", "..".
 * 
 * This function is not expecting dir to have a trailing directory delimiter
 * unless it represents *nix root. C functions don't add these and neither
 * do sz81 functions so I'm choosing not to handle them.
 * 
 * On entry: char *dir = the directory string to update
 *           char *direntry = the relative directory to change to
 *  On exit: char *dir will be updated */

void file_dialog_cd(char *dir, char *direntry) {
	char foldername[256];
	int index;

	/* Copy the direntry and strip the surrounding brackets if found */
	if (*direntry == '(') {
		strcpy(foldername, direntry + 1);
		foldername[strlen(foldername) - 1] = 0;
	} else {
		strcpy(foldername, direntry);
	}

	/* Is this the parent directory? */
	if (strcmp(foldername, "..") == 0) {
		/* Go back to the parent directory */
		if ((index = strlen(dir))) {
			/* Truncate leftwards up to a delimiter, root or nothing */
			while (index >= 1 && 
				dir[index - 1] != DIR_DELIMITER_CHAR
			#if defined(__amigaos4__) || defined(_WIN32)
				&& dir[index - 1] != ':'
			#endif
				) dir[--index] = 0;

			/* Do we need to cut the directory delimiter? */
			if (index >= 2 && 
				dir[index - 1] == DIR_DELIMITER_CHAR
			#if defined(_WIN32)
				&& dir[index - 2] != ':'
			#endif
				) dir[--index] = 0;
		}
	} else {
		/* It's a subdirectory */
		/* Add a directory delimiter if required */
		strcatdelimiter(dir);
		strcat(dir, foldername);
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

	/* NOTE TO PORTERS: eventually you'll hit root ('/' on *nix, ':' on
	 * __amigaos4__, '\' on _WIN32 and as I'm developing this on Linux I
	 * can't populate the list with drive specifiers etc., so possibly
	 * you could have a virtual area above root that you fill yourself,
	 * or you could populate the root list with "(c:)", "(d:)" etc. and
	 * when the user selects one, you could change to that drive and set
	 * dir equal to root. Other functions you should take a look at are
	 * file_dialog_cd, the code within sdl_input.c:2555 that extracts the
	 * directory from the list to change to and file_dialog_basename. And
	 * DIR_DELIMITER_CHAR and strcatdelimiter are useful. That's it :) */

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
 * File Write Unsigned Short Little Endian                                 *
 ***************************************************************************/

void fwrite_unsigned_short_little_endian(unsigned short *source, FILE *fp) {
	unsigned char byteval;

	byteval = *source & 0xff;			fwrite(&byteval, 1, 1, fp);
	byteval = (*source >> 8) & 0xff;	fwrite(&byteval, 1, 1, fp);
}

/***************************************************************************
 * File Write Int Little Endian                                            *
 ***************************************************************************/

void fwrite_int_little_endian(int *source, FILE *fp) {
	unsigned char byteval;

	byteval = *source & 0xff;			fwrite(&byteval, 1, 1, fp);
	byteval = (*source >> 8) & 0xff;	fwrite(&byteval, 1, 1, fp);
	byteval = (*source >> 16) & 0xff;	fwrite(&byteval, 1, 1, fp);
	byteval = (*source >> 24) & 0xff;	fwrite(&byteval, 1, 1, fp);
}

/***************************************************************************
 * File Write Unsigned Long Little Endian                                  *
 ***************************************************************************/

void fwrite_unsigned_long_little_endian(unsigned long *source, FILE *fp) {
	unsigned char byteval;

	byteval = *source & 0xff;			fwrite(&byteval, 1, 1, fp);
	byteval = (*source >> 8) & 0xff;	fwrite(&byteval, 1, 1, fp);
	byteval = (*source >> 16) & 0xff;	fwrite(&byteval, 1, 1, fp);
	byteval = (*source >> 24) & 0xff;	fwrite(&byteval, 1, 1, fp);
}

/***************************************************************************
 * File Read Unsigned Short Little Endian                                  *
 ***************************************************************************/

void fread_unsigned_short_little_endian(unsigned short *target, FILE *fp) {
	unsigned char byteval;

	*target = 0;
	fread(&byteval, 1, 1, fp); *target |= byteval;
	fread(&byteval, 1, 1, fp); *target |= (byteval << 8);
}

/***************************************************************************
 * File Read Int Little Endian                                             *
 ***************************************************************************/

void fread_int_little_endian(int *target, FILE *fp) {
	unsigned char byteval;

	*target = 0;
	fread(&byteval, 1, 1, fp); *target |= byteval;
	fread(&byteval, 1, 1, fp); *target |= (byteval << 8);
	fread(&byteval, 1, 1, fp); *target |= (byteval << 16);
	fread(&byteval, 1, 1, fp); *target |= (byteval << 24);
}

/***************************************************************************
 * File Read Unsigned Long Little Endian                                   *
 ***************************************************************************/

void fread_unsigned_long_little_endian(unsigned long *target, FILE *fp) {
	unsigned char byteval;

	*target = 0;
	fread(&byteval, 1, 1, fp); *target |= byteval;
	fread(&byteval, 1, 1, fp); *target |= (byteval << 8);
	fread(&byteval, 1, 1, fp); *target |= (byteval << 16);
	fread(&byteval, 1, 1, fp); *target |= (byteval << 24);
}

/***************************************************************************
 * Get Filename Next Highest                                               *
 ***************************************************************************/
/* This will scan a directory for files matching the pattern in format.
 * 
 * Examples would be "prtout%4d" to match "prtout0001.pbm" and 
 * "zx80prog%4d" to match "zx80prog0001.o".
 * 
 * WARNING: Don't do what I initially did and use %i instead of %d else 
 * you won't get past 0008 and it'll take you a while to understand why!
 * 
 * Search out the sscanf or scanf documentation to fully understand format
 * and PLEASE NOTE that sscanf here is expecting max one value to read and
 * store, so passing "%c%i" will segfault (use "%*c%i" - see scanf docs).
 * 
 * On entry: char *dir points to the directory to scan
 *           char *format points to the pattern to match
 *  On exit: returns 1 to 2147483647 on success
 *           or 0 on opendir error */

int get_filename_next_highest(char *dir, char *format) {
	struct dirent *direntry;
	int retval = 1, value;
	DIR *dirstream;

	if ((dirstream = opendir(dir)) != NULL) {
		while ((direntry = readdir(dirstream))) {
			if (sscanf(direntry->d_name, format, &value) == 1 &&
				retval <= value) retval = value + 1;
		}
		closedir(dirstream);
	} else {
		retval = 0;
		fprintf(stderr, "%s: Cannot read from directory %s\n", __func__,
			dir);
	}

	return retval;
}


