/*----------------------------------------------------------------------------/
/  FatFs - FAT file system module  R0.07e                    (C)ChaN, 2009
/-----------------------------------------------------------------------------
/ Completely hacked to use a local subfolder for EightyOne
/----------------------------------------------------------------------------*/

#include "ff.h"         /* FatFs configurations and declarations */

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h> /* PATH_MAX */
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifdef Win32
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif

void loadFileSymbolsProxy(const char*) {}

struct stat stats;

// rootpath, curpath; always slash-terminated.
//
const char* rootPath = NULL;
char curPath[32768];
char tempPath[32768];
char statPath[32768];

extern "C" void strcatdelimiter(char *toappendto);

void AddSlash(char* p)
{
   size_t n = strlen(p);
   if (p[n-1] != '/' && p[n-1] != '\\')
   {
      p[n] = '/';
      p[n+1] = 0;
   }
}

void SetRoot(const char* root)
{
   int n;

   strcpy(tempPath, PACKAGE_DATA_DIR);
   n = strlen(tempPath);
   tempPath[n-4] = '\0'; /* remove 'data' ... */
   strcat(tempPath, root);
   strcatdelimiter(tempPath);

   char* t = tempPath;
   for (;*t; ++t)
   {
      if (*t =='\\')
      {
         *t = '/';
      }
   }

   rootPath = strdup(tempPath);
   strcpy(curPath, rootPath);
}

const char*createFullPath(const char*path)
{
   strcpy(tempPath, curPath);
   if (path) strcat(tempPath, path);
   return tempPath;
}

#if 0
char *realpath(const char *path, char *resolved_path) 
{
	char buf[32768];
   int len;
	size_t idx;
	
	_fullpath(buf,path,32768);
	
	len=strlen(buf);
	
	if (len == 0 || len > 32768 - 1)
		strcpy(resolved_path, path);
	else 
		strcpy(resolved_path, buf);
 
	// Replace backslashes with forward slashes so the
	// rest of the code behaves correctly.
	for (idx = 0; idx < strlen(resolved_path); idx++) 
	{
		if (resolved_path[idx] == '\\') 
			resolved_path[idx] = '/';
	}
  
	return resolved_path;
}
#endif

void mem_cpy (void* dst, const void* src, int cnt) {
   char *d = (char*)dst;
   const char *s = (const char *)src;
   while (cnt--) *d++ = *s++;
}

char chk_chr (const char* str, char chr) {
   while (*str && *str != chr) str++;
   return *str;
}


FRESULT cvtERRNO(void)
{
   switch(errno)
   {
   case EACCES: // Tried to open read-only file for writing, file's sharing mode does not allow specified operations, or given path is directory.
      return FR_DENIED;

   case EEXIST: // _O_CREAT and _O_EXCL flags specified, but filename already exists.
      return FR_EXIST;

   case EINVAL: // Invalid oflag or pmode argument.
      return FR_INVALID_OBJECT;

   case ENOENT: // File or path not found.
      return FR_NO_PATH;

   default:
      return FR_INT_ERR;
   }
}

void SetHandle(FIL* fp, int handle)
{
   fp->fs = handle;
}

int GetHandle(FIL* fp)
{
   return fp->fs;
}








/*-----------------------------------------------------------------------*/
/* Helper                                                                */
/*-----------------------------------------------------------------------*/

void get_fileinfo_special(struct _FILINFO_ *fi)
{
   fi->fsize = stats.st_size;
   if (!(stats.st_mode & S_IWRITE))
   {
      fi->fattrib = AM_RDO;
   }
   else
   {
      fi->fattrib = 0;
   }
}






/*-----------------------------------------------------------------------*/
/* Open or Create a File                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_open (
   FIL *fp,       /* Pointer to the blank file object */
   const XCHAR *path,   /* Pointer to the file name */
   BYTE mode         /* Access mode and file open mode flags */
)
{
   if (GetHandle(fp) != 0)
   {
      close(GetHandle(fp));
      SetHandle(fp, 0);
   }

   int flags = O_BINARY;
   if (mode & FA_WRITE)
   {
      flags |= O_CREAT | O_TRUNC | O_RDWR;
      if (mode & FA_CREATE_NEW)
      {
         // fail if exists
         flags |= O_EXCL;
      }
   }

   // use stat(name,...) instead of fstat(handle,...)
   
   const char* fullNme = createFullPath(path);
   int handle = open(fullNme, flags, 0644);
   if (-1 == handle)
   {
      return cvtERRNO();
   }

   if (-1 == stat(fullNme, &stats))
   {
      close(handle);
      return cvtERRNO();
   }

   fp->fsize = stats.st_size;
   fp->flag = 0;

   loadFileSymbolsProxy(fullNme);

   SetHandle(fp, handle);
   return FR_OK;
}


FRESULT f_lseek (FIL *fp, DWORD pos)
{
        if (lseek(GetHandle(fp), pos, SEEK_SET) == -1) {
                return cvtERRNO();
        }
        return FR_OK;
}


/*-----------------------------------------------------------------------*/
/* Read File                                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_read (
   FIL *fp,       /* Pointer to the file object */
   void *buff,    /* Pointer to data buffer */
   UINT btr,      /* Number of bytes to read */
   UINT *br    /* Pointer to number of bytes read */
)
{
   int n = read(GetHandle(fp), buff, btr);
   if (n < 0)
   {
      return cvtERRNO();
   }

   *br = (UINT)n;
   return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Write File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_write (
   FIL *fp,       /* Pointer to the file object */
   const void *buff, /* Pointer to the data to be written */
   UINT btw,         /* Number of bytes to write */
   UINT *bw       /* Pointer to number of bytes written */
)
{
   int n = write(GetHandle(fp), buff, btw);
   if (n < 0)
   {
      return cvtERRNO();
   }

   *bw = (UINT)n;
   return FR_OK;
}



/*-----------------------------------------------------------------------*/
/* Close File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_close (
   FIL *fp     /* Pointer to the file object to be closed */
)
{
   close(GetHandle(fp));
   SetHandle(fp, 0);
   return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Change Current Drive/Directory                                        */
/*-----------------------------------------------------------------------*/

FRESULT f_chdrive (
   BYTE drv    /* Drive number */
)
{
   return FR_OK;
}



FRESULT f_chdir (
   const XCHAR *path /* Pointer to the directory path */
)
{
   char p[32768];

   // need to turn the relative path spec into a real path
   FRESULT	result = FR_NO_PATH;

   // add new directory to current
	sprintf(tempPath,"%s%s",curPath,path);
	
	// Resolve the newpath 
	if(NULL!=realpath(tempPath,p))
	{
      AddSlash(p);

		// Check that the new path is BELOW the base mmcpath
//		if(0==strncmp(rootPath,p,strlen(rootPath)))
		if(strlen(p) >= strlen(rootPath))
		{
			// And that it exists
			if(0==access(p,0))
			{
				strcpy(curPath, p);
				result=FR_OK;
			}
		}
	}

   return result;
}







/*-----------------------------------------------------------------------*/
/* Open Directory ready for reading                                      */
/*-----------------------------------------------------------------------*/

FRESULT __attribute__((optimize("O0"))) f_opendir (
   DIR **dj,         /* Pointer to directory object to create */
   const XCHAR *path /* Pointer to the directory path */
)
{
   if (!(*dj)) {
	   closedir(*dj);
	   *dj = NULL;
   }
   
   *dj = opendir(createFullPath(path));
   if (!(dj)) return FR_NO_PATH;
   return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Read Directory Entry in Sequense                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_readdir (
   DIR *dj,            /* Pointer to the open directory object */
   FILINFO *fileinfo   /* Pointer to file information to return */
)
{
   struct dirent *d;

   memset(&fileinfo->fname[0], 0, 13);

   if (dj) if ((d = readdir(dj)))
   {
      BYTE attrib = 0;

      strcpy(statPath, tempPath);
      strcat(statPath, d->d_name);
      
      stat(statPath, &stats);

      if (!(stats.st_mode & S_IRUSR))
      {
         attrib |= AM_HID;
      }
      if (!(stats.st_mode & S_IWUSR))
      {
         attrib |= AM_RDO;
      }
      if (stats.st_mode & S_IFDIR)
      {
         attrib |= AM_DIR;
      }

      fileinfo->fsize = stats.st_size;
      fileinfo->fattrib = attrib;
      strcpy(&fileinfo->fname[0], &d->d_name[0]);
   }

   return FR_OK;
}




FRESULT f_mkdir (const XCHAR* dirname)
{
   FRESULT ret = FR_OK;
#ifndef Win32
   if (mkdir(createFullPath(dirname), 0755) == -1)
#else
   if (mkdir(createFullPath(dirname)) == -1)
#endif
   {
      ret = cvtERRNO();
   }

   return ret;
}




/*-----------------------------------------------------------------------*/
/* Delete a File or Directory                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_rename (
   const XCHAR *path1,    /* Pointer to the file or directory path */
   const XCHAR *path2    /* Pointer to the file or directory path */
)
{
   FRESULT ret = FR_OK;

   char tmpp[32768];
   createFullPath(path1);
   strcpy(tmpp, tempPath);

   if (!stat(createFullPath(path2), &stats))
   {
      return cvtERRNO();
   }

   // a concatenation of the mmc dir & the requested path
   if (rename(tmpp, tempPath)) /* just created above */
   {
      ret = cvtERRNO();
   }

   return ret;
}



/*-----------------------------------------------------------------------*/
/* Delete a File or Directory                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_unlink (
   const XCHAR *path    /* Pointer to the file or directory path */
)
{
   FRESULT ret = FR_OK;
   const char* target= createFullPath(path);

   if (-1 == stat(target, &stats))
   {
      return cvtERRNO();
   }

   if (stats.st_mode & S_IFDIR)
   {
      if (rmdir(target) == -1)
      {
         ret = FR_DENIED;
      }
   }
   else
   {
      // a concatenation of the mmc dir & the requested path
      if (unlink(target) == -1)
      {
         ret = cvtERRNO();
      }
   }

   return ret;
}
