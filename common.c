/* z81/xz81, Linux console and X ZX81/ZX80 emulators.
 * Copyright (C) 1994 Ian Collier. z81 changes (C) 1995-2004 Russell Marks.
 * sz81 Copyright (C) 2007-2011 Thunor <thunorsif@hotmail.com>
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
 *
 *
 * common.c - various routines/vars common to z81/xz81/sz81.
 */

#define Z81_VER		"2.1"

#include <string.h>

#ifndef SZ81	/* Added by Thunor */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#endif

#ifdef SZ81	/* Added by Thunor */
#include "sdl.h"
#include "sdl_sound.h"
extern void sdl_set_redraw_video();
#endif

#ifndef Win32
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include "common.h"
#include "sound.h"
#include "z80/z80.h"
#include "zx81.h"
#include "allmain.h"
#include "w5100.h"

extern ZX81 zx81;

#ifdef ZXMORE
unsigned char mem[0x100000];
#else
unsigned char mem[0x20000]; /* allows for shadow RAM */
#endif
unsigned char *helpscrn;
unsigned char keyports[9]={0xff,0xff,0xff,0xff, 0xff,0xff,0xff,0xff, 0xff};

#define SHMSIZ 0x4000
BYTE *sz81mem;
int rwsz81mem, fdsz81mem;

extern int zxmmod;

/* this two work on a per-k basis, so we can support 1k etc. properly */
unsigned char *memptr[64];
int memattr[64];

int help=0;
int sound=0;
int sound_vsync=0,sound_ay=0,sound_ay_type=AY_TYPE_NONE;
int load_hook=1,save_hook=1;
int vsync_visuals=0;
int invert_screen=0;

#ifdef SZ81	/* Added by Thunor */
int signal_int_flag=0;
#else
volatile int signal_int_flag=0;
#endif
volatile int exit_program_flag=0;
int interrupted=0;
int nmigen=0,hsyncgen=0,vsync=0;
int scrn_freq=2;
int unexpanded=0;
int taguladisp=0;
int fakedispx=0,fakedispy=0;	/* set by main.c/xmain.c */

extern int ulacharline;

/* for the printer */
#ifdef SZ81	/* Added by Thunor */
/* This solves an issue for us with zxpopen. Originally the printer file
 * (if requested via -p on the CLI) is created right at the start and
 * it could also remain empty if nothing was printed to it, but we want
 * to create a uniquely named file only when it is printed to. The issue
 * with the default behaviour is that zxpopen initialises some variables
 * that are used within printer_inout which is always called regardless
 * of an open file, so they need to be initialised here from the outset.
 * zxpopen is now called from within zxpout (for sz81) and therefore
 * printer_inout gets called first.
 * 
 * I don't know why static is being used here as it is protecting the
 * variables from being modified from outside this file, but they're
 * not being accessed from outside this file anyway. I think this code
 * may have been copied from xz80. Also zxpframes and zxpcycles aren't
 * being initialised so I'm setting them to zero too */

static unsigned long zxpframes=0, zxpcycles=0;
static int zxpspeed=0,zxpnewspeed=0;
static int zxpheight=0,zxppixel=-1,zxpstylus=0;
#else
static unsigned long zxpframes=0, zxpcycles=0; 
static intzxpspeed,zxpnewspeed;
static int zxpheight,zxppixel,zxpstylus;
#endif
static FILE *zxpfile=NULL;
char *zxpfilename=NULL;
static unsigned char zxpline[256];

#ifdef SZ81	/* Added by Thunor */
/* This is actually redundant as z81's load selector has been entirely
 * replaced, but a load selector component still exists within sz81 that
 * is currently always off and so I'll leave this here for the moment
 * and mark it temp temp */
int load_selector_state = 0;
#endif

int refresh_screen=1;

/* =1 if emulating ZX80 rather than ZX81 */
int zx80=0;

int ignore_esc=0;

int autolist=0;
int autoload=0;
char autoload_filename[1024];

int chromamode=0;
unsigned char bordercolour=0x0f;

/* not too many prototypes needed... :-) */
#ifndef SZ81	/* Added by Thunor */
char *load_selector(void);
#endif


#ifndef SZ81	/* Added by Thunor */
void sighandler(int a)
{
signal_int_flag=1;
}
#endif


#ifdef ZXMORE
char *libdir(char *file)
{
static char buf[1024];

#ifdef ZXMORE
#define LIBDIR PACKAGE_DATA_DIR
#else
#define LIBDIR ""
#endif
if(strlen(LIBDIR)+strlen(file)+2>sizeof(buf))
  strcpy(buf,file);	/* we know file is a short constant */
else
  sprintf(buf,"%s/%s",LIBDIR,file);
return(buf);
}
#endif

#ifndef SZ81	/* Added by Thunor */
void autoload_setup(char *filename)
{
if(zx80 || (filename && strlen(filename)+1>sizeof(autoload_filename)))
  return;
autoload=1;
if(filename)
  strcpy(autoload_filename,filename);
else
  *autoload_filename=0;
}
#endif


#ifndef SZ81	/* Added by Thunor */
static void exit_program_flag_set(int foo)
{
exit_program_flag=1;
}
#endif


#ifndef SZ81	/* Added by Thunor */
void startsigsandtimer()
{
int f,tmp=1000000/50;	/* default 50 ints/sec */ 
struct sigaction sa;
struct itimerval itv;

int badsig[]={SIGHUP,SIGINT,SIGQUIT,SIGILL,SIGSEGV,SIGTERM,SIGBUS};
int numsig=sizeof(badsig)/sizeof(badsig[0]);

sigemptyset(&sa.sa_mask);
sa.sa_handler=exit_program_flag_set;
/* it's a bit odd using SA_RESTART for one-off exit code,
 * but Solaris's SA_RESETHAND behaviour *as documented* is bogus
 * (even though it seems ok in practice), so we play it safe.
 */
sa.sa_flags=SA_RESTART;

/* XXX this extra-careful masking stuff may be superfluous
 * now it just sets an `exit soon' flag...
 *
 * handle all signals listed in badsig[], making sure that all
 * signals in the list are blocked whenever handling one of them.
 * There are two loops as sa_mask must be fully set before calling
 * sigaction().
 */
for(f=0;f<numsig;f++)
  sigaddset(&sa.sa_mask,badsig[f]);

for(f=0;f<numsig;f++)
  sigaction(badsig[f],&sa,NULL);

#ifdef OSS_SOUND_SUPPORT
/* no timer signals when sound is enabled */
if(sound_enabled)
  return;
#endif

sigemptyset(&sa.sa_mask);	/* SIGALRM implicit */
sa.sa_handler=sighandler;
sa.sa_flags=SA_RESTART;

sigaction(SIGALRM,&sa,NULL);

itv.it_interval.tv_sec=  tmp/1000000;
itv.it_interval.tv_usec=tmp-1000000*itv.it_interval.tv_sec;
itv.it_value.tv_sec=itv.it_interval.tv_sec;
itv.it_value.tv_usec=itv.it_interval.tv_usec;
setitimer(ITIMER_REAL,&itv,NULL);
}
#endif

void loadrombank(int offset)
{
  memcpy(mem,sdl_zx81rom.data+offset,8192);
}

void loadrom(void)
{
#ifdef ZXMORE
FILE *in;
char *fname=libdir("ZXM-DATA.BIN");

if((in=fopen(fname,"rb"))!=NULL)
  {
    fread(mem,1,0x80000,in);
  }
 else
   {
     fprintf(stderr,"couldn't load ROM file %s.\n", fname);
     exit(1);
   }
return;
#endif

#ifdef SZ81	/* Added by Thunor */
/* sz81 has already preloaded the ROMs so now this function
 * is simply copying the data into the ROM area afresh */
if(zx80)
  {
  memcpy(mem,sdl_zx80rom.data,sdl_zx80rom.state);
  }
else
  {
  memcpy(mem,sdl_zx81rom.data,sdl_zx81rom.state);
  }

/* fill first 16k with extra copies of it */

 if (zx81.shadowROM) {
  int siz=(zx80?4096:8192);
  memcpy(mem+siz,mem,siz);
  if(zx80)
    memcpy(mem+siz*2,mem,siz*2);
 }

#else
FILE *in;
char *fname=libdir(zx80?"zx80.rom":"zx81.rom");

if((in=fopen(fname,"rb"))!=NULL)
  {
  int siz=(zx80?4096:8192);
  
  fread(mem,1,siz,in);
  
  /* fill first 16k with extra copies of it
   * (not really needed given since improved memptr[] resolution,
   * but what the heck :-))
   */
  memcpy(mem+siz,mem,siz);
  if(zx80)
    memcpy(mem+siz*2,mem,siz*2);
  
  fclose(in);
  }
else
  {
  fprintf(stderr,
          "z81: couldn't load ROM file %s.\n"
          "     You *must* install this for z81 to work.\n",
          fname);
  exit(1);
  }
#endif
}

/* the ZX81 char is used to index into this, to give the ascii.
 * awkward chars are mapped to '_' (underscore), and the ZX81's
 * all-caps is converted to lowercase.
 * The mapping is also valid for the ZX80 for alphanumerics.
 * WARNING: this only covers 0<=char<=63!
 */
static char zx2ascii[64]={
/*  0- 9 */ ' ', '_', '_', '_', '_', '_', '_', '_', '_', '_', 
/* 10-19 */ '_', '\'','#', '$', ':', '?', '(', ')', '>', '<', 
/* 20-29 */ '=', '+', '-', '*', '/', ';', ',', '.', '0', '1', 
/* 30-39 */ '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 
/* 40-49 */ 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 
/* 50-59 */ 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
/* 60-63 */ 'w', 'x', 'y', 'z'
};

static char ascii2zx[96]=
  {
   0, 0,11,12,13, 0, 0,11,16,17,23,21,26,22,27,24,
  28,29,30,31,32,33,34,35,36,37,14,25,19,20,18,15,
  23,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,
  53,54,55,56,57,58,59,60,61,62,63,16,24,17,11,22,
  11,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,
  53,54,55,56,57,58,59,60,61,62,63,16,24,17,11, 0
  };


void kcomm(int a)
{
char buf[256], fname[256];
unsigned char *ptr=mem+a+2;
char *bptr=buf;
char *fptr=fname;
FILE *out;
int k=0;
int n=0;
unsigned int arg1, arg2;
int itxt=0;

  memset(buf,0,sizeof(buf));
  memset(fname,0,sizeof(fname));

  do {
    *bptr=zx2ascii[(*ptr)&63];
    switch (k) {
      case 0: if (*bptr=='\'') k++; break;
      case 1: if (*bptr!='\'') *fptr++ = *bptr; else k++; break;
      case 2: if (*bptr=='#') itxt=1; break;
      default :;
    }
    bptr++;
    n++;
  } while((*(++ptr))!=0x76 && bptr<buf+sizeof(buf)-3);
  
  printf("Writing files %s(.a/z)...\n",fname);
  strcat(fname,".a");

  if((out=fopen(fname,"wb"))==NULL) return;
  fwrite(buf,n,1,out);
  fclose(out);

  arg1 = mem[0x4001] | (mem[0x4002] << 8);
  arg2 = mem[0x4003] | (mem[0x4004] << 8);

  if (itxt) { arg1 -= 2; arg2 +=1; }

  n = arg2-arg1+1;
  fname[strlen(fname)-1] = 'z';
  if((out=fopen(fname,"wb"))==NULL) return;
  fwrite(mem+arg1,n,1, out);
  fclose(out);
}

unsigned int getarg1(char *buf)
{
int state=0;
int i;
int radix=10;
int n = 0;
int c;

    for (i=0; i<strlen(buf); i++) {
      switch (state) {
        case 0:
        case 1: if (buf[i]=='\'') state++; break;
	case 2: if (buf[i]==' ') state=3; break;
	case 3: if (buf[i]!=' ') state=4;
	case 4: if (buf[i]==' ') {
	          state = 5;
	        } else if (buf[i]==':') {
	          radix = 16;
		} else {
		  if (buf[i]>='a') c=buf[i]-'a'+10; else c=buf[i]-'0';
		  n = n*radix + c;
		}
		break;
	default:;
      }
    }
    return n;
}

static char lcomm_fname[256];
static FILE *lcomm_in;
static int lcomm_state=0;
static int lcomm_itxt=0;

unsigned char lcomm(int a1, int a2)
{
char buf[256];
unsigned char c=0;
unsigned char *ptr=mem+a2+1;
char *dptr=lcomm_fname;
unsigned int arg1;

switch (a1) {
case 0xf0:
  memset(lcomm_fname,0,sizeof(lcomm_fname));
  do
    *dptr++=zx2ascii[(*ptr)&63];
  while((*(++ptr))!=0x0b && dptr<lcomm_fname+sizeof(lcomm_fname)-3);
  printf("Reading files %s(.a/z)...\n",lcomm_fname);
  strcat(lcomm_fname,".a");
  if((lcomm_in=fopen(lcomm_fname,"rb"))==NULL) return 0xff;
  lcomm_state = 1;
  lcomm_itxt = 0;

case 0xf1:
  if (lcomm_state>2) return 0xff;
  if (fread(&c,1,1,lcomm_in)!=1) {
    fclose(lcomm_in);
    if (lcomm_state==1) {
      if (lcomm_itxt==1) {
        if((lcomm_in=fopen(lcomm_fname,"rb"))==NULL) return 0xff;
        c = fread(buf,sizeof(buf),1,lcomm_in);
        fclose(lcomm_in);
        lcomm_state++;
      }
      lcomm_fname[strlen(lcomm_fname)-1] = 'z';
      if((lcomm_in=fopen(lcomm_fname,"rb"))==NULL) return 0xff;
      if (lcomm_itxt==1) {
        arg1 = getarg1(buf);
	c = fread(mem+arg1,0x8000,1,lcomm_in);
	fclose(lcomm_in);
	lcomm_state++;
        return 0x76;
      }
    } else {
      return 0xff;
    }
    lcomm_state++;
  } else {
    if (lcomm_state==1) {
      if (c=='#') lcomm_itxt++;
      c = toupper(c);
      c = ascii2zx[c-32];
    }
  }
  break;
}

return c;

}

void aszmic4hacks()
{
	mem[0x0690]=0xe1;
	mem[0x0691]=0xef;
	mem[0x0692]=0xed;
	mem[0x0693]=0xfa;
	mem[0x0694]=0xc9;

	mem[0x06c2]=0xe1;
	mem[0x06c3]=0x3e;
	mem[0x06c4]=0xf0;
	mem[0x06c5]=0xed;
	mem[0x06c6]=0xfb;
	mem[0x06c7]=0xfe;
	mem[0x06c8]=0xff;
	mem[0x06c9]=0xc8;
	mem[0x06ca]=0xcd;
	mem[0x06cb]=0x92;
	mem[0x06cc]=0x04;
	mem[0x06cd]=0x3e;
	mem[0x06ce]=0xf1;
	mem[0x06cf]=0x18;
	mem[0x06d0]=0xf4;
}

void aszmic7hacks()
{
	mem[0x0691]=0xe1;
	mem[0x0692]=0xef;
	mem[0x0693]=0xed;
	mem[0x0694]=0xfa;
	mem[0x0695]=0xc9;

	mem[0x06c3]=0xe1;
	mem[0x06c4]=0x3e;
	mem[0x06c5]=0xf0;
	mem[0x06c6]=0xed;
	mem[0x06c7]=0xfb;
	mem[0x06c8]=0xfe;
	mem[0x06c9]=0xff;
	mem[0x06ca]=0xc8;
	mem[0x06cb]=0xcd;
	mem[0x06cc]=0x90;
	mem[0x06cd]=0x04;
	mem[0x06ce]=0x3e;
	mem[0x06cf]=0xf1;
	mem[0x06d0]=0x18;
	mem[0x06d1]=0xf4;
}


void lambdahacks()
{
/* patch save routine */
if(save_hook)
  {
  mem[0x0d0d]=0xed; mem[0x0d0e]=0xfd;
  mem[0x0d0f]=0xc3; mem[0x0d10]=0x03; mem[0x0d11]=0x02;
  }

/* patch load routine */
if(load_hook)
  {
  mem[0x19ef]=0xeb;
  mem[0x19f0]=0xed; mem[0x19f1]=0xfc;
  mem[0x19f2]=0xc3; mem[0x19f3]=0x03; mem[0x19f4]=0x02;
  }
}

void zx80hacks()
{
#ifndef ZXPAND
#ifndef ZXNU
#ifndef VDRIVE

/* patch save routine */
if(save_hook)
  {
  mem[0x1b6]=0xed; mem[0x1b7]=0xfd;
  mem[0x1b8]=0xc3; mem[0x1b9]=0x83; mem[0x1ba]=0x02;
  }

/* patch load routine */
if(load_hook)
  {
  mem[0x206]=0xed; mem[0x207]=0xfc;
  mem[0x208]=0xc3; mem[0x209]=0x83; mem[0x20a]=0x02;
  }
#endif
#endif
#endif
}

#ifdef ZXMORE

void zx81hacks()
{
  BYTE *memory;
  int i;

  memory = mem + 0x60000;

  for (i=1; i<=6; i++) {

/* patch save routine */
if(save_hook)
  {
  memory[0x2fc]=0xed; memory[0x2fd]=0xfd;
  memory[0x2fe]=0xc3; memory[0x2ff]=0x07; memory[0x300]=0x02;
  }

/* patch load routine */
if(load_hook)
  {
  memory[0x347]=0xeb;
  memory[0x348]=0xed; memory[0x349]=0xfc;
  memory[0x34a]=0xc3; memory[0x34b]=0x07; memory[0x34c]=0x02;
  }

  memory -= 0x10000;
  }

  /* The ZXmore has a ZX80 in the first page of memory */
  zx80hacks();
}

#else

void zx81hacks()
{
#ifndef ZXPAND
#ifndef ZXNU

/* patch save routine */
if(save_hook)
  {
  mem[0x2fc]=0xed; mem[0x2fd]=0xfd;
  mem[0x2fe]=0xc3; mem[0x2ff]=0x07; mem[0x300]=0x02;
  }

/* patch load routine */
if(load_hook)
  {
  mem[0x347]=0xeb;
  mem[0x348]=0xed; mem[0x349]=0xfc;
  mem[0x34a]=0xc3; mem[0x34b]=0x07; mem[0x34c]=0x02;
  }
#endif
#endif
}

#endif


void initmem()
{
int f;
int ramsize;
int count;

loadrom();

#ifdef ZXMORE
 memset(mem+0x80000,0xaa,0x80000); /* "clear" RAM */
#endif

#ifdef SZ81NO	/* Added by Thunor */ /* Would not give shadow ROMs... */
if(zx80)
  {
  memset(mem+0x1000,0,0xf000);
  }
else
  {
  memset(mem+0x2000,0,0xe000);
  }
#else
memset(mem+0x4000,0,0xc000);
#endif

/* ROM setup */
count=0;
for(f=0;f<16;f++)
  {
  memattr[f]=memattr[32+f]=0;
  memptr[f]=memptr[32+f]=mem+1024*count;
  count++;
  if(count>=(zx80?4:8)) count=0;
  }

/* RAM setup */
#ifdef SZ81	/* Added by Thunor */
ramsize=sdl_emulator.ramsize;
#else
ramsize=16;
if(unexpanded)
  ramsize=1;
#endif
count=0;
for(f=16;f<32;f++)
  {
  memattr[f]=memattr[32+f]=1;
  memptr[f]=memptr[32+f]=mem+1024*(16+count);
  count++;
  if(count>=ramsize) count=0;
  }

#ifdef SZ81	/* Added by Thunor */
/* z81's ROM and RAM initialisation code is OK for <= 16K RAM but beyond
 * that it requires a little tweaking.
 * 
 * The following diagram shows the ZX81 + 8K ROM. The ZX80 version is
 * the same except that each 8K ROM region will contain two copies of
 * the 4K ROM.
 * 
 * RAM less than 16K is mirrored throughout the 16K region.
 * 
 * The ROM will only detect up to 8000h when setting RAMTOP, therefore
 * having more than 16K RAM will require RAMTOP to be set by the user
 * (or user program) to either 49152 for 32K or 65535 for 48/56K.
 * 
 *           1K to 16K       32K           48K           56K      Extra Info.
 * 
 *  65535  +----------+  +----------+  +----------+  +----------+ 
 * (FFFFh) | 16K RAM  |  | 16K RAM  |  | 16K RAM  |  | 16K RAM  | DFILE can be
 *         | mirrored |  | mirrored |  |          |  |          | wholly here.
 *         |          |  |          |  |          |  |          | 
 *         |          |  |          |  |          |  |          | BASIC variables
 *         |          |  |          |  |          |  |          | can go here.
 *  49152  +----------+  +----------+  +----------+  +----------+ 
 * (C000h) | 8K ROM   |  | 16K RAM  |  | 16K RAM  |  | 16K RAM  | BASIC program
 *         | mirrored |  |          |  |          |  |          | is restricted
 *  40960  +----------+  |          |  |          |  |          | to here.
 * (A000h) | 8K ROM   |  |          |  |          |  |          | 
 *         | mirrored |  |          |  |          |  |          | 
 *  32768  +----------+  +----------+  +----------+  +----------+ 
 * (8000h) | 16K RAM  |  | 16K RAM  |  | 16K RAM  |  | 16K RAM  | No machine code
 *         |          |  |          |  |          |  |          | beyond here.
 *         |          |  |          |  |          |  |          | 
 *         |          |  |          |  |          |  |          | DFILE can be
 *         |          |  |          |  |          |  |          | wholly here.
 *  16384  +----------+  +----------+  +----------+  +----------+ 
 * (4000h) | 8K ROM   |  | 8K ROM   |  | 8K ROM   |  | 8K RAM   | 
 *         | mirrored |  | mirrored |  | mirrored |  |          | 
 *   8192  +----------+  +----------+  +----------+  +----------+ 
 * (2000h) | 8K ROM   |  | 8K ROM   |  | 8K ROM   |  | 8K ROM   | 
 *         |          |  |          |  |          |  |          | 
 *      0  +----------+  +----------+  +----------+  +----------+ 

 * Added: 24K option which is 16K plus 8K in 8192-16383
 */

switch(ramsize)
  {
  case 56:
    for(f=8;f<16;f++)
      {
      memattr[f]=1;					/* It's now writable */
      memptr[f]=mem+1024*f;
      }
  case 48:
    for(f=48;f<64;f++)
      {
      memattr[f]=1;
      memptr[f]=mem+1024*f;
      }
  case 32:
    for(f=32;f<48;f++)
      {
      memattr[f]=1;
      memptr[f]=mem+1024*f;
      }
    break;
  }
#endif

/* changed with 2.2.0 */

if(zx81.machine==MACHINEZX80)
  zx80hacks();

if(zx81.machine==MACHINEZX81)
  zx81hacks();

if(zx81.machine==MACHINELAMBDA)
  lambdahacks();

 if (sdl_emulator.networking) w_init();

/* initialise shared memory */

#ifndef Win32
 if (rwsz81mem==1) {
	 fdsz81mem = shm_open("/sz81mem", O_RDONLY, S_IRUSR | S_IWUSR);
	 if (fdsz81mem < 0) {
		 perror("open");
		 exit(1);
	 } else {
		 sz81mem = mmap(NULL, SHMSIZ, PROT_READ, MAP_SHARED, fdsz81mem, 0);
		 if (sz81mem == MAP_FAILED) { perror("map"); exit(1); }
	 }
 } else if (rwsz81mem==2) {
	 fdsz81mem = shm_open("/sz81mem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	 if (fdsz81mem < 0) {
		 perror("open");
		 exit(1);
	 } else {
		 if (ftruncate(fdsz81mem, SHMSIZ) < 0) { perror("ftruncate"); exit(1); }
		 sz81mem = mmap(NULL, SHMSIZ, PROT_WRITE, MAP_SHARED, fdsz81mem, 0);
		 if (sz81mem == MAP_FAILED) { perror("map"); exit(1); }
	}
}
#endif

}

void exitmem()
{
#ifndef Win32
	if (rwsz81mem) {
		if (munmap(sz81mem, SHMSIZ) < 0) perror("munmap");
		if (close(fdsz81mem < 0)) perror("close");
		if (rwsz81mem==2) { if (shm_unlink("/sz81mem") < 0) perror("shm_unlink"); }
	}
#endif
#ifdef ZXMORE
	if (zxmmod) {
		FILE *out;
		char *fname=libdir("ZXM-DATA.BIN");

		if((out=fopen(fname,"wb"))!=NULL)
			{
				fwrite(mem,0x80000,1,out);
				fclose(out);
			}
	}
#endif
}

#ifndef SZ81	/* Added by Thunor */
void loadhelp(void)
{
FILE *in;
char buf[128];

/* but first, set up location of help/selector */
fakedispx=(ZX_VID_HMARGIN-FUDGE_FACTOR)/8;
fakedispy=ZX_VID_MARGIN;

/* Thunor: Note that helpscrn below is never freed */
if((in=fopen(libdir(zx80?"zx80kybd.pbm":"zx81kybd.pbm"),"rb"))!=NULL)
  {
  /* ditch header lines */
  fgets(buf,sizeof(buf),in);
  fgets(buf,sizeof(buf),in);
  if((helpscrn=calloc(256*192/8,1))==NULL)
    {
    fprintf(stderr,"z81: couldn't allocate memory for help screen.\n");
    exit(1);
    }
  fread(helpscrn,1,256*192/8,in);
  fclose(in);
  }
else
  {
  fprintf(stderr,"z81: couldn't load help screen.\n");
  exit(1);
  }
}
#endif


// #define DEBUGPRTR

/* versus description of static values... */
void zxpinit()
{
	zxpframes=0,zxpcycles=0,zxpspeed=0,zxpnewspeed=0;
	zxpheight=0,zxppixel=-1,zxpstylus=0;
	zxpfile=NULL;
	zxpfilename=NULL;
}

void zxpopen(void)
{
#ifdef SZ81	/* Added by Thunor */
static int failcnt = 0;
#endif

if(!zxpfilename)
  return;

fprintf(stdout,"Creating zxprinter file %s...\n", zxpfilename);

if((zxpfile=fopen(zxpfilename,"wb"))==NULL)
  {
#ifdef SZ81	/* Added by Thunor */
  if (failcnt++ < 10) 
#endif
  fprintf(stderr,"z81: couldn't open printer file, printing disabled\n");
  return;
  }

/* we reserve 10 chars for height */
fprintf(zxpfile,"P4\n256 %10d\n",0);
}


void zxpupdateheader(void)
{
long pos;

if(!zxpfile || !zxpheight) return;

pos=ftell(zxpfile);

/* seek back to write the image height */
if(fseek(zxpfile,strlen("P4\n256 "),SEEK_SET)!=0)
  fprintf(stderr,"z81: warning: couldn't seek to write image height\n");
else
  {
  /* I originally had spaces after the image height, but that actually
   * breaks the format as defined in pbm(5) (not to mention breaking
   * when read by zgv :-)). So they're now before the height.
   */
  fprintf(zxpfile,"%10d",zxpheight);
  }

if(fseek(zxpfile,pos,SEEK_SET)!=0)
  {
  fprintf(stderr,"z81: error: printer file re-seek failed, printout disabled!\n");
  fclose(zxpfile);
  zxpfile=NULL;
  }
}


void zxpclose(void)
{
int f;

/* stop the printer */
zx81_writeport(0xfb,4,0);

if(!zxpfile)
  return;

/* a blank line if we haven't actually printed anything :-) */
if(!zxpheight)
  {
  zxpheight++;
  for(f=0;f<32;f++)
    fputc(0,zxpfile);
  }

/* write header */
zxpupdateheader();

fclose(zxpfile);
zxpfile=NULL;
printf("Closing printer file...\n");
}


void zxpout(void)
{
int i,j,d;

if(!zxpfile) 
#ifdef SZ81	/* Added by Chris */
	zxpopen();
    if(!zxpfile) return;
#else
	return;
#endif

zxpheight++;
for(i=0;i<32;i++)
  {
  for(d=j=0;j<8;j++)
    d=(d<<1)+(zxpline[i*8+j]?1:0);
  fputc(d,zxpfile);
  }
}


/* From http://problemkaputt.de/zxdocs.htm

Port FBh Read - Printer Status

  Bit  Expl.
  0    Data Request   (0=Busy, 1=Ready/DRQ)
  1-5  Not used
  6    Printer Detect (0=Okay, 1=None)
  7    Newline        (0=Nope, 1=Begin of new line)

Port FBh Write - Printer Output

  Bit  Expl.
  0    Not used
  1    Undoc/Speed?   (0=Normal, 1=used to slow-down last 2 scanlines)
  2    Motor          (0=Start, 1=Stop)
  3-6  Not used
  7    Pixel Output   (0=White/Silver, 1=Black)
*/

/* ZX Printer support, transliterated from IMC's xz80 by a bear of
 * very little brain. :-) Or at least, I don't grok it that well.
 * It works wonderfully though.
 */
int printer_inout(int is_out,int val)
{
/* Note that we go through the motions even if printer support isn't
 * enabled, as the alternative would seem to be to crash... :-)
 */

if(!is_out)
  {
  /* input */
  
  if(!zxpspeed)
	  return 0x3e; /* no newline, printer okay, busy */
  else                 /* after init, zxpspeed is 0 */
    {
    int frame=frames-zxpframes;
    int cycles=tstates-zxpcycles;
    int pix=zxppixel;
    int sp=zxpnewspeed;
    int x,ans;
    int cpp=440/zxpspeed;

    if(frame>400) frame=400;
    cycles+=frame*tsmax;
    x=cycles/cpp-64;        /* x-coordinate reached */
      
    while(x>320)
      {                    /* if we are on another line, */
      pix=-1;              /* find out where we are */
      x-=384;
      if(sp)
        {
        x=(x+64)*cpp;
        cpp=440/sp;
        x=x/cpp-64;
        sp=0;
        }
      }

    if((x>-10 && x<0) | zxpstylus)
	    ans=0xbe; /* newline, printer okay, busy */
    else
	    ans=0x3e; /* no newline */
    if(x>pix) ans|=1; /* ready */

#ifdef DEBUGPRTR
    printf("printer_inout %ld %ld %d 0x%02x %d\n", frames, tstates, is_out, val, zxpspeed);
#endif

    return ans;
    }
  }


/* output */

#ifdef DEBUGPRTR
 printf("printer_inout %ld %ld %d 0x%02x %d\n", frames, tstates, is_out, val, zxpspeed);
#endif

if(!zxpspeed)          /* after init, zxpspeed is 0 */
  {
	  if(!(val&4)) /* motor start */
		  {
			  zxpspeed=(val&2)?1:2;
			  zxpframes=frames;
			  zxpcycles=tstates;
			  zxpstylus=val&128;
			  zxppixel=-1;
		  }
  }
 else                  /* after init, zxpspeed is 0 */
  {
  int frame=frames-zxpframes;
  int cycles=tstates-zxpcycles;
  int i,x;
  int cpp=440/zxpspeed;
      
  if(frame>400) frame=400; /* limit height of blank paper */
  cycles+=frame*tsmax;
  x=cycles/cpp-64;         /* x-coordinate reached */
  for(i=zxppixel;i<x && i<256;i++)
    if(i>=0)		   /* should be, but just in case */
      zxpline[i]=zxpstylus;
  if(x>=256 && zxppixel<256) {
    zxpout();
#ifdef DEBUGPRTR
    printf("zxpout A\n");
#endif
  }

  while(x>=320)
    {          /* move to next line */

#ifdef DEBUGPRTR
	    printf("%d %d %d %d %d %d %d %d\n", tstates, zxpframes, zxpcycles, zxpnewspeed, zxpspeed, x, cpp, zxpstylus);
#endif

    zxpcycles+=cpp*384;
    if(zxpcycles>=tsmax) zxpcycles-=tsmax,zxpframes++;
    x-=384;
    if(zxpnewspeed)
      {
      zxpspeed=zxpnewspeed;
      zxpnewspeed=0;
      x=(x+64)*cpp;
      cpp=440/zxpspeed;
      x=x/cpp-64;
      }
    for(i=0;i<x && i<256;i++)
      zxpline[i]=zxpstylus;
    if(x>=256) {
      zxpout();
#ifdef DEBUGPRTR
      printf("zxpout B\n");
#endif
    }
    } /* end move to next line */

  if(x<0) x=-1;

  if(val&4) /* motor stop */
    {
    if(x>=0 && x<256)
      {
      for(i=x;i<256;i++)
        zxpline[i]=zxpstylus;
      zxpout();
#ifdef DEBUGPRTR
      printf("zxpout C\n");
#endif
      }
    zxpspeed=zxpstylus=0;
    
    /* this is pretty frequent (on a per-char-line basis!),
     * but it's the only time we can really do it automagically.
     */
    zxpupdateheader();
    }
  else /* motor start */
    {
    zxppixel=x;
    zxpstylus=val&128;
    if(x<0)
      zxpspeed=(val&2)?1:2;
    else
      {
      zxpnewspeed=(val&2)?1:2;
      if(zxpnewspeed==zxpspeed) zxpnewspeed=0;
      }
    }
  } 

return(0);
}


#ifndef SZ81	/* Added by Thunor */
void save_p(int a)
{
static unsigned char fname[256];
unsigned char *ptr=mem+a,*dptr=fname;
FILE *out;

if(zx80)
  {
    strcpy(fname,"zx80prog.p");
  }
else
  {
  /* so the filename is at ptr, in the ZX81 char set, with the last char
   * having bit 7 set. First, get that name in ASCII.
   */
  
  memset(fname,0,sizeof(fname));
  do
    *dptr++=zx2ascii[(*ptr)&63];
  while((*ptr++)<128 && dptr<fname+sizeof(fname)-3);
  
  /* add '.p' */
  strcat(fname,".p");
  }

/* now open the file */
if((out=fopen(fname,"wb"))==NULL)
  return;

/* work out how much to write, and write it.
 * we need to write from 0x4009 (0x4000 on ZX80) to E_LINE inclusive.
 */
if(zx80)
  fwrite(mem+0x4000,1,fetch2(16394)-0x4000+1,out);
else
  fwrite(mem+0x4009,1,fetch2(16404)-0x4009+1,out);
/* Thunor: The +1 is unnecessary as it adds junk after the trailing 0x80 */

fclose(out);
}
#endif


#ifndef SZ81	/* Added by Thunor */
void load_p(int a)
{
static unsigned char fname[sizeof(autoload_filename)];
unsigned char *ptr=mem+(a&32767),*dptr=fname;
FILE *in;
int got_ascii_already=0;

if(zx80)
  {
    strcpy(fname,"zx80prog.p");
  }
else
  {
  if(a>=32768) 	/* they did LOAD "" */
    {
    got_ascii_already=1;
    
    if(autoload && !autolist)	/* autoload stuff did it */
      {
      refresh_screen=1;		/* make sure we redraw screen soon */
      strcpy(fname,autoload_filename);	/* guaranteed to be ok */
      
      /* add .p if needed */
      if(strlen(fname)<sizeof(fname)-3 &&
         (strlen(fname)<=2 || strcasecmp(fname+strlen(fname)-2,".p")!=0))
        strcat(fname,".p");
      }
    else /* user did it (or `-l') */
      {
      char *ret=load_selector();

      if(ret==NULL || strlen(ret)+1>=sizeof(fname))
        {
        /* if autolist is aborted or goes wrong, we exit completely */
        if(autolist)
          exit_program();
        return;
        }

      autoload=autolist=0;	/* in case it was `-l' */

      strcpy(fname,ret);
      }
    }
  
  /* so the filename is at ptr, in the ZX81 char set, with the last char
   * having bit 7 set. First, get that name in ASCII.
   */
  if(!got_ascii_already)
    {
    /* test for Xtender-style LOAD " STOP " to quit */
    if(*ptr==227) exit_program();
    
    memset(fname,0,sizeof(fname));
    do
      *dptr++=zx2ascii[(*ptr)&63];
    while((*ptr++)<128 && dptr<fname+sizeof(fname)-3);
    
    /* add '.p' */
    strcat(fname,".p");
    }
  }

/* now open the file */
if((in=fopen(fname,"rb"))==NULL)
  {
  /* the partial snap will crash without a file, so reset */
  if(autoload)
    reset81(),autoload=0;
  return;
  }

autoload=0;

/* just read it all */
fread(mem+(zx80?0x4000:0x4009),1,16384,in);

fclose(in);

/* XXX is this still valid given proper video? */
/* don't ask me why, but the ZX80 ROM load routine does this if it
 * works...
 */
if(zx80)
  store(0x400b,fetch(0x400b)+1);
/* Thunor: This must be a bug!? I can see it in the ROM 021A: INC (IY+$0B)
 * but it's part of the ZX80 LOAD routine which is being jumped over.
 * All I've found it to do is corrupt E_LINE, cause numbers to appear
 * after the last program line with the only solution being a reset */
}
#endif


#ifndef SZ81	/* Added by Thunor */
void overlay_help(void)
{
unsigned char *ptr,*optr;
int y;

ptr=helpscrn;
optr=scrnbmp+fakedispy*ZX_VID_FULLWIDTH/8+fakedispx;

for(y=0;y<192;y++)
  {
  memcpy(optr,ptr,32);
  ptr+=32;
  optr+=ZX_VID_FULLWIDTH/8;
  }
}
#endif


void do_interrupt()
{
static int count=0;

#ifndef SZ81	/* Added by Thunor */
if(exit_program_flag)
  exit_program();
#endif

/* being careful here not to screw up any pending reset... */
if(interrupted==1)
  interrupted=0;

/* only do screen update every 1/Nth */
count++;
#ifdef SZ81	/* Added by Thunor */
if(count>sdl_emulator.frameskip)
#else
if(count>=scrn_freq)
#endif
  {
  count=0;
  #ifndef SZ81	/* Added by Thunor */
  if(help) overlay_help();
  #endif
  update_scrn();
  }
check_events();	/* on X checks events, on VGA scans kybd */
}


#ifndef SZ81	/* Added by Thunor */
/* despite the name, this also works for the ZX80 :-) */
void reset81()
{
interrupted=2;	/* will cause a reset */
memset(mem+16384,0,49152);
refresh_screen=1;
#ifdef OSS_SOUND_SUPPORT	/* Thunor: this was missing */
if(sound_ay)
  sound_ay_reset();
#endif
}
#endif


#ifndef SZ81	/* Added by Thunor */
void usage_help(char *cmd)
{
printf("z81 " Z81_VER
	" - copyright (C) 1994-2004 Ian Collier and Russell Marks.\n\n");
printf("usage: %s [-hilLosSTuV] [-a sound_addon_type] [-p printout.pbm]\n"
       "\t\t[-r refresh_rate] [filename.p]\n",
       cmd);
puts("\n"
"	-a	emulate AY-chip-based sound addon chip, with\n"
"		`sound_addon_type' specifying the addon from the types\n"
"		below:\n"
"\n"
"		q	Quicksilva sound board\n"
"		z	Bi-Pak Zon X-81\n"
"\n"
"		You can also enable ACB stereo, by adding `s' (e.g. `-a qs').\n"
"	-h	this usage help.\n"
"	-i	invert screen, giving a white-on-black display.\n"
"	-l	boot directly to the `LOAD \"\"' file selector. This only\n"
"		works when starting with `filename.p' would (see note below).\n"
"	-L	disable LOAD hook. (Not terribly useful.)\n"
"	-o	emulate `old ROM' machine, i.e. emulate a ZX80.\n"
"	-p	emulate ZX Printer, with PBM output in specified file.\n"
"		(Most picture viewers/converters can handle PBM files.)\n"
"	-r	set how often the screen is redrawn in 1/50ths of a second.\n"
"	-s	emulate VSYNC-based sound support (the usual kind).\n"
"	-S	disable SAVE hook. Useful if you want to hear what SAVE\n"
"		does when sound is enabled. :-) Use `-V' to see it too.\n"
"	-T	toggle top bit of ULA-displayed bytes (mainly for debugging,\n"
"		but also useful for checking the display's `shape').\n"
"	-u	emulate unexpanded machine, e.g. a 1k ZX81.\n"
"	-V	emulate VSYNC visuals, e.g. the patterns you get when you\n"
"		LOAD or SAVE on the real machine, and the little flicker you\n"
"		get when typing in a program line. (This tends to be a bit\n"
"		annoying when typing in a program with the default frame-skip,\n"
"		so it's disabled by default.)\n"
"\n"
"	filename.p\n"
"		load the specified program instantly on startup;\n"
"		this doesn't work when emulating a ZX80, or 1k ZX81.");
}
#endif


#ifndef SZ81	/* Added by Thunor */
void parseoptions(int argc,char *argv[])
{
int done=0;

opterr=0;

do
  switch(getopt(argc,argv,"a:hilLop:r:sSTuV"))
    {
    #ifdef OSS_SOUND_SUPPORT	/* Thunor: this was missing */
    case 'a':
      sound=1;
      sound_ay=1;
      switch(*optarg)
        {
        case 'q':
          sound_ay_type=AY_TYPE_QUICKSILVA;
          break;
        case 'z':
          sound_ay_type=AY_TYPE_ZONX;
          break;
        default:
          fprintf(stderr,"z81: unrecognised sound addon type `%s'.\n",optarg);
          exit(1);
        }
      if(*optarg && optarg[1]=='s')
        sound_stereo=1,sound_stereo_acb=1;
      break;
    #endif
    case 'h':
      usage_help(argv[0]);
      exit(1);
    case 'i':	/* invert display */
      invert_screen=1;
      break;
    case 'l':	/* autoload, but to file selector */
      autolist=1;
      break;
    case 'L':	/* no LOAD hook */
      load_hook=0;
      break;
    case 'o':	/* old rom machine, i.e. ZX80 */
      zx80=1;
      break;
    case 'p':	/* printer support (and output filename) */
      zxpfilename=optarg;
      break;
    case 'r':	/* refresh rate */
      scrn_freq=atoi(optarg);
      if(scrn_freq<1) scrn_freq=1;
      if(scrn_freq>50) scrn_freq=50;
      break;
    #ifdef OSS_SOUND_SUPPORT	/* Thunor: this was missing */
    case 's':	/* sound (!) - specifically, VSYNC-based sound */
      sound=1;
      sound_vsync=1;
      break;
    #endif
    case 'S':	/* no SAVE hook */
      save_hook=0;
      break;
    case 'T':	/* tag ULA bytes when displayed */
      taguladisp=1;
      break;
    case 'u':	/* unexpanded (no RAM pack) */
      unexpanded=1;
      break;
    case 'V':	/* emulate VSYNC visuals */
      vsync_visuals=1;
      break;
    case '?':
      switch(optopt)
        {
        case 'a':
          fprintf(stderr,"z81: "
                  "the -a option needs a sound addon type as argument "
                  "(see man page).\n");
          break;
        case 'p':
          fprintf(stderr,"z81: "
                  "the -p option needs a PBM output filename as argument.\n");
          break;
        case 'r':
          fprintf(stderr,"z81: "
                  "the -r option needs a refresh rate as argument.\n");
          break;
        default:
          fprintf(stderr,"z81: option `%c' not recognised.\n",optopt);
        }
      exit(1);
    case -1:
      done=1;
    }
while(!done);

if(optind==argc-1 || autolist)	/* if filename or `-l' given... */
  {
  if(zx80 || unexpanded)
    {
    autolist=0;
    fprintf(stderr,"z81: auto-loading not supported for ZX80 or 1k ZX81.\n");
    }
  else
    {
    if(!load_hook)
      {
      load_hook=1;
      fprintf(stderr,"z81: warning: auto-load is re-enabling LOAD hook.\n");
      }
    autoload_setup(autolist?NULL:argv[optind]);
    }
  }
}
#endif


void frame_pause(void)
{
#ifndef SZ81	/* Added by Thunor */
static int first=1;
static sigset_t mask,oldmask;
#endif

#ifdef OSS_SOUND_SUPPORT
if(sound_enabled)
  {
  /* we block on the sound instead. It's a bit unpleasant,
   * but it's the best way.
   */
  sound_frame();
  
#ifndef SZ81	/* Added by Thunor. We don't block on sound but continue */
  if(interrupted<2)
    interrupted=1;
  return;
#endif
  }
#endif

#ifdef SZ81	/* Added by Thunor */
while(!signal_int_flag) SDL_Delay(10);
#else
/* we leave it blocked most of the time, only unblocking
 * temporarily with sigsuspend().
 */
if(first)
  {
  first=0;

  sigemptyset(&mask);
  sigaddset(&mask,SIGALRM);
  sigprocmask(SIG_BLOCK,&mask,&oldmask);
  }

/* the procmask stuff is to avoid a race condition */
while(!signal_int_flag)
  sigsuspend(&oldmask);
#endif

signal_int_flag=0;

if(interrupted<2)
  interrupted=1;
}


#ifndef SZ81	/* Added by Thunor */
static char ascii2zx[96]=
  {
   0, 0,11,12,13, 0, 0,11,16,17,23,21,26,22,27,24,
  28,29,30,31,32,33,34,35,36,37,14,25,19,20,18,15,
  23,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,
  53,54,55,56,57,58,59,60,61,62,63,16,24,17,11,22,
  11,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,
  53,54,55,56,57,58,59,60,61,62,63,16,24,17,11, 0
  };
#endif


#ifndef SZ81	/* Added by Thunor */
void draw_files(unsigned char *scrn,int top,int cursel,int numfiles,
	char *filearr,int elmtsiz)
{
int x,y,n,xor,c,len;
unsigned char *sptr;

/* since this is quick and easy (and incrementally updated for us :-)),
 * we redraw each time no matter what.
 */

/* clear filenames area */
for(y=3;y<21;y++)
  memset(scrn+1+y*33+1,0,30);

n=top;
for(y=3;y<21;y++,n++)
  {
  if(n>=numfiles) break;
  xor=128*(n==cursel);
  sptr=scrn+1+y*33+1;
  len=strlen(filearr+elmtsiz*n);
  if(len>30) len=30;
  for(x=0;x<len;x++)
    {
    c=(filearr+elmtsiz*n)[x]-32;
    if(c<0 || c>95) c=0;
    c=ascii2zx[c];
    *sptr++=(c^xor);
    }
  }
}
#endif


#ifndef SZ81	/* Added by Thunor */
void draw_load_frame(unsigned char *scrn)
{
int x,y;
unsigned char *sptr=scrn+1;
/* these must be exactly 32 chars long */
/*           01234567890123456789012345678901 */
char *text1="    choose a program to load    ";
char *text2=" q=up a=dn enter=load spc=abort ";

memset(sptr,131,32);
sptr[33*2]=7;
memset(sptr+33*2+1,3,30);
sptr[33*2+31]=132;
for(y=3;y<21;y++)
  sptr[33*y]=5,sptr[33*y+31]=133;
sptr[33*21]=130;
memset(sptr+33*21+1,131,30);
sptr[33*21+31]=129;
for(x=0;x<32;x++)
  {
  sptr[33   +x]=128+ascii2zx[text1[x]-32];
  sptr[33*22+x]=128+ascii2zx[text2[x]-32];
  }
memset(sptr+33*23,3,32);
}
#endif


#ifndef SZ81	/* Added by Thunor */
/* convert our text display to a bitmap in scrnbmp */
void selscrn_to_scrnbmp(unsigned char *scrn)
{
unsigned char *cptr=mem+0x1e00;
unsigned char *ptr,*optr,*optrsav;
int x,y,b,c,d,inv;

memset(scrnbmp,0,ZX_VID_FULLHEIGHT*ZX_VID_FULLWIDTH/8);

ptr=scrn+1;
optr=scrnbmp+fakedispy*ZX_VID_FULLWIDTH/8+fakedispx;

for(y=0;y<24;y++,ptr++)
  {
  optrsav=optr;
  for(x=0;x<32;x++,ptr++,optr++)
    {
    c=*ptr;
    inv=(c&128); c&=63;
    
    for(b=0;b<8;b++,optr+=ZX_VID_FULLWIDTH/8)
      {
      d=cptr[c*8+b];
      if(inv) d^=255;
      *optr=d;
      }
    optr-=ZX_VID_FULLWIDTH;
    }
  
  optr=optrsav+ZX_VID_FULLWIDTH;
  }
}
#endif


#ifndef SZ81	/* Added by Thunor */
/* simulate lastk generation from keyports[] */
int make_lastk(void)
{
int y,b;
int lastk0=0,lastk1=0;

for(y=0;y<8;y++)		/* 8 half-rows */
  {
  b=(keyports[y]|0xe0);
  
  /* contribute to it if key was pressed */
  if((b&31)!=31)
    {
    /* set y bit in lastk0 if not shift */
    if(!(y==0 && ((b&31)|1)==31)) lastk0|=(1<<y);
    
    /* now set pos-in-half-row bit(s) in lastk1 */
    b=(((~b)&31)<<1);
    /* move bit 1 of b back down to bit 0 if it's shift bit */
    if(y==0) b=((b&0xfc)|((b&2)>>1));
  
    lastk1|=b;
    }
  }

return(0xffff^(lastk0|(lastk1<<8)));
}
#endif


#ifndef SZ81	/* Added by Thunor */
/* wait for them to let go of all keys */
void sel_waitnokeys(void)
{
while(make_lastk()!=0xffff)
  {
  frame_pause();
  do_interrupt();
  }
}
#endif


#ifndef SZ81	/* Added by Thunor */
char *load_selector()
{
static char returned_filename[256];
static unsigned char selscrn[33*24+1];	/* pseudo-DFILE */
int f,height=18;
char *files=NULL;
int files_size,files_incr=0x2000;
DIR *dirfile;
struct dirent *entry;
struct stat sbuf;
int files_ofs,numfiles,len,maxlen;
int top,cursel;
int quit,got_one,isdir;
char *filearr;
char *ptr;
int krwait=25,krwrep=3;	/* wait before key rpt and wait before next rpt */
int krheld=0,krsubh=0;
int oldkey,key,virtkey;

/* should never get here if emulating ZX80, but FWIW... */
if(zx80) return(NULL);

/* usually won't need this on a 16k, but a 1k responds to LOAD ""
 * much more quickly... :-)
 */
sel_waitnokeys();

ignore_esc=1;		/* avoid possibility of esc screwing things up */

oldkey=-1;

do
  {
  if((dirfile=opendir("."))==NULL)
    {
    fprintf(stderr,"couldn't read current directory!\n");
    ignore_esc=0;
    return(NULL);
    }

  files_size=0x4000;
  files_ofs=0;
  if((files=malloc(files_size))==NULL)
    {
    fprintf(stderr,"not enough memory for file selector!\n");
    ignore_esc=0;
    return(NULL);
    }

  memset(selscrn,0,sizeof(selscrn));	/* blank our screen */
  draw_load_frame(selscrn);	/* draw everything apart from files list */
  /* force an update */
  selscrn_to_scrnbmp(selscrn);
  refresh_screen=1;
  update_scrn();
  
  numfiles=0;
  maxlen=-1;
  top=cursel=0;
  quit=got_one=0;

  /* read list of .p files */
  while((entry=readdir(dirfile))!=NULL)
    {
    isdir=0;

    /* must be non-hidden dir (and not `.') or .p file */
    len=strlen(entry->d_name);
    if((stat(entry->d_name,&sbuf)!=-1 && (isdir=S_ISDIR(sbuf.st_mode)) &&
        (strcmp(entry->d_name,"..")==0 || entry->d_name[0]!='.')) ||
       (len>2 && (strcasecmp(entry->d_name+len-2,".p")==0)))
      {
      /* make array of filenames bigger if needed */
      if(files_ofs+len+(isdir?3:1)>=files_size)
        {
        files_size+=files_incr;
        if((files=realloc(files,files_size))==NULL)
          {
          fprintf(stderr,"not enough memory for file selector!\n");
          ignore_esc=0;
          closedir(dirfile);
          return(NULL);
          }
        }
    
      /* copy filename */
      if(isdir)
        {
        files[files_ofs]='(';
        strcpy(files+files_ofs+1,entry->d_name);
        strcat(files+files_ofs+1,")");
        len+=2;
        }
      else
        strcpy(files+files_ofs,entry->d_name);
    
      files_ofs+=len+1;
      if(len+1>maxlen) maxlen=len+1;
      numfiles++;
      }
    }

  closedir(dirfile);

  /* have to put them into something more like a normal array to
   * use qsort...
   * (I later realised I could just have sorted an array of pointers,
   * but never mind, I've got the code to do it this way now. :-/)
   */
  if(numfiles==0 || (filearr=malloc(maxlen*numfiles))==NULL)
    {
    free(files);
    if(numfiles!=0)
      fprintf(stderr,"not enough memory for file selector!\n");
    ignore_esc=0;
    return(NULL);
    }

  /* copy filenames */
  ptr=files;
  for(f=0;f<numfiles;f++)
    {
    strcpy(filearr+maxlen*f,ptr);
    ptr+=strlen(ptr)+1;
    }

  free(files);

  /* and sort */
  qsort(filearr,numfiles,maxlen,(int (*)(const void *,const void *))strcmp);


  /* now do the file selector stuff */

  while(!quit && !got_one)
    {
    /* draw file list */
    draw_files(selscrn,top,cursel,numfiles,filearr,maxlen);
    /* convert to graphics, in scrnbmp */
    selscrn_to_scrnbmp(selscrn);

    /* hang around, update real display, read keys */
    frame_pause();
    do_interrupt();
    
    key=make_lastk();
  
    /* auto-repeat stuff */
    virtkey=key;
    if(key!=oldkey)
      krheld=0,krsubh=0;
    else
      {
      krheld++;
      if(krheld<krwait)
        virtkey=-1;
      else
        if(krheld>krwait)
          {
          krsubh++;
          if(krsubh<krwrep)
            virtkey=-1;
          else
            krsubh=0;
          }
      }
  
    switch(virtkey)
      {
      case 0xfdfb:	/* q */
        cursel--; break;
      case 0xfdfd:	/* a */
        cursel++; break;
      case 0xfcfb:	/* shift(alt)-q */
        cursel-=17; break;
      case 0xfcfd:	/* shift(alt)-a */
        cursel+=17; break;
      case 0xfdbf:	/* enter */
        got_one=1; break;
      case 0xfd7f:	/* space */
        quit=1; break;
      }
    if(cursel<0) cursel=0;
    if(cursel>=numfiles) cursel=numfiles-1;
    if(cursel<top) top--;
    if(cursel>=top+height) top++;
  
    oldkey=key;
    }

  isdir=0;
  if(got_one)
    {
    strcpy(returned_filename,ptr=filearr+cursel*maxlen);
    if(*ptr=='(')
      {
      ptr[strlen(ptr)-1]=0;
      if(stat(ptr+1,&sbuf)!=-1 && S_ISDIR(sbuf.st_mode))
        {
        isdir=1;
        /* copy dir name instead */
        strcpy(returned_filename,ptr+1);
        }
      }
    }
  free(filearr);

  sel_waitnokeys();
  
  if(quit)
    {
    ignore_esc=0;
    return(NULL);
    }
  }
while(got_one && isdir && chdir(returned_filename)==0);

/* if not a dir, return file to load */
ignore_esc=0;

/* or error, if the chdir() failed */
if(got_one && isdir) return(NULL);

return(returned_filename);
}
#endif


#ifdef SZ81	/* Added by Thunor */
void common_reset(void)
{
int count;

/* Reinitialise variables at the top of common.c that aren't unused
 * options, being managed by sz81 or in init functions elsewhere */
for(count=0;count<9;count++)
  keyports[count]=0xff;
signal_int_flag=0;
nmigen=hsyncgen=vsync=0;
zxpframes=zxpcycles=zxpspeed=zxpnewspeed=0;
zxpheight=0;
zxppixel=-1;
zxpstylus=0;
refresh_screen=1;
}
#endif




