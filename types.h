#ifndef __MITYPES__
#define __MITYPES__

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

/* Maximum length of a filename (inc. path, filename and ext.) */

#ifndef _MAX_PATH
#define _MAX_PATH	260			/* Max length of filename, inc. path. */
#endif

#ifndef Z80_CONFIG
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;
#endif

#endif		/* __MITYPES__ */
