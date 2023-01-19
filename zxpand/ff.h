/*---------------------------------------------------------------------------/
/  FatFs - FAT file system module include file  R0.07e       (C)ChaN, 2009
/----------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following trems.
/
/  Copyright (C) 2009, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial product UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/ This version contains only the essential declarations for the ZXpand
/ emulation for sz81.
/
/----------------------------------------------------------------------------*/

#ifndef _FATFS
#define _FATFS 0x007E

#include <sys/types.h>
#include <dirent.h>

#include "integer.h" /* Basic integer types */

typedef char XCHAR;     /* SBCS, DBCS */


/* File object structure */

typedef struct _FIL_ {
   int   fs;         /* Pointer to the owner file system object */
   BYTE  flag;       /* File status flags */
   DWORD fsize;      /* File size */
} FIL;


/* File status structure */

typedef struct _FILINFO_ {
   DWORD fsize;      /* File size */
   BYTE  fattrib; /* Attribute */
   char  fname[13];  /* Short file name (8.3 format) */
} FILINFO;


/* File function return code (FRESULT) */

typedef enum {
   FR_OK = 0,        /* 0 */
   FR_DISK_ERR,      /* 1 */
   FR_INT_ERR,       /* 2 */
   FR_NOT_READY,     /* 3 */
   FR_NO_FILE,       /* 4 */
   FR_NO_PATH,       /* 5 */
   FR_INVALID_NAME,  /* 6 */
   FR_DENIED,        /* 7 */
   FR_EXIST,         /* 8 */
   FR_INVALID_OBJECT,   /* 9 */
   FR_WRITE_PROTECTED,  /* 10 */
   FR_INVALID_DRIVE, /* 11 */
   FR_NOT_ENABLED,      /* 12 */
   FR_NO_FILESYSTEM, /* 13 */
   FR_MKFS_ABORTED,  /* 14 */
   FR_TIMEOUT        /* 15 */
} FRESULT;


/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

FRESULT f_open (FIL*, const XCHAR*, BYTE);         /* Open or create a file */
FRESULT f_read (FIL*, void*, UINT, UINT*);         /* Read data from a file */
FRESULT f_write (FIL*, const void*, UINT, UINT*);  /* Write data to a file */
FRESULT f_lseek (FIL*, DWORD);                     /* Move file pointer of a file object */
FRESULT f_close (FIL*);                            /* Close an open file object */
FRESULT f_opendir (DIR**, const XCHAR*);           /* Open an existing directory */
FRESULT f_readdir (DIR*, FILINFO*);                /* Read a directory item */
FRESULT f_stat (const XCHAR*, FILINFO*);           /* Get file status */
FRESULT f_truncate (FIL*);                         /* Truncate file */
FRESULT f_sync (FIL*);                             /* Flush cached data of a writing file */
FRESULT f_unlink (const XCHAR*);                   /* Delete an existing file or directory */
FRESULT f_mkdir (const XCHAR*);                    /* Create a new directory */
FRESULT f_chmod (const XCHAR*, BYTE, BYTE);        /* Change attriburte of the file/dir */
FRESULT f_utime (const XCHAR*, const FILINFO*);    /* Change timestamp of the file/dir */
FRESULT f_rename (const XCHAR*, const XCHAR*);     /* Rename/Move a file or directory */
FRESULT f_forward (FIL*, UINT(*)(const BYTE*,UINT), UINT, UINT*);
						   /* Forward data to the stream */
FRESULT f_mkfs (BYTE, BYTE, WORD);                 /* Create a file system on the drive */
FRESULT f_chdir (const XCHAR*);                    /* Change current directory */
FRESULT f_chdrive (BYTE);                          /* Change current drive */

/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access control and file status flags (FIL.flag) */

#define FA_OPEN_EXISTING  0x00
#define FA_READ           0x01
#define FA_WRITE          0x02
#define FA_CREATE_NEW     0x04
#define FA_CREATE_ALWAYS  0x08
#define FA_OPEN_ALWAYS    0x10
#define FA__WRITTEN       0x20
#define FA__DIRTY         0x40
#define FA__ERROR         0x80


/* File attribute bits for directory entry */

#define AM_RDO  0x01  /* Read only */
#define AM_HID  0x02  /* Hidden */
#define AM_SYS  0x04  /* System */
#define AM_VOL  0x08  /* Volume label */
#define AM_LFN  0x0F  /* LFN entry */
#define AM_DIR  0x10  /* Directory */
#define AM_ARC  0x20  /* Archive */
#define AM_MASK 0x3F  /* Mask of defined bits */


#endif /* _FATFS */
