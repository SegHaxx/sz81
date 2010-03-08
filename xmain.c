/* z81/xz81, Linux console and X ZX81/ZX80 emulators.
 * Copyright (C) 1994 Ian Collier. xz81 changes (C) 1995-2001 Russell Marks.
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

/* most of this file is heavily based on xz80's xspectrum.c,
 * to the extent that some of the indentation is a crazy
 * mix-and-match affair. :-)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>

#ifdef MITSHM
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif

#ifdef HAS_UNAME
#include <sys/utsname.h>
#endif

#include "common.h"
#include "sound.h"
#include "z80.h"
#include "allmain.h"

#define MAX_DISP_LEN 256

#include "xz81.icon"

int mitshm=1;

int hsize=ZX_VID_X_WIDTH*SCALE,vsize=ZX_VID_X_HEIGHT*SCALE;

/* remember, this table is ignoring shifts... */
static struct {unsigned char port,mask;} keytable[]={
/*  SP      !        "        #        $        %        &        '  */
{7,0xfe},{8,0xff},{3,0xfd},{8,0xff},{5,0xf7},{8,0xff},{8,0xff},{8,0xff},
/*  (       )        *        +        ,        -        .        /  */
{5,0xfb},{5,0xfd},{4,0xfb},{6,0xfb},{7,0xfd},{6,0xf7},{7,0xfd},{0,0xef},
/*  0       1        2        3        4        5        6        7  */
{4,0xfe},{3,0xfe},{3,0xfd},{3,0xfb},{3,0xf7},{3,0xef},{4,0xef},{4,0xf7},
/*  8       9        :        ;        <        =        >        ?  */
{4,0xfb},{4,0xfd},{0,0xfd},{0,0xfb},{7,0xf7},{6,0xfb},{7,0xfb},{0,0xef},
/*  @       A        B        C        D        E        F        G  */
{8,0xff},{1,0xfe},{7,0xef},{0,0xf7},{1,0xfb},{2,0xfb},{1,0xf7},{1,0xef},
/*  H       I        J        K        L        M        N        O  */
{6,0xef},{5,0xfb},{6,0xf7},{6,0xfb},{6,0xfd},{7,0xfb},{7,0xf7},{5,0xfd},
/*  P       Q        R        S        T        U        V        W  */
{5,0xfe},{2,0xfe},{2,0xf7},{1,0xfd},{2,0xef},{5,0xf7},{0,0xef},{2,0xfd},
/*  X       Y        Z        [        \        ]        ^        _  */
{0,0xfb},{5,0xef},{0,0xfd},{8,0xff},{8,0xff},{8,0xff},{8,0xff},{8,0xff},
/*  `       a        b        c        d        e        f        g  */
{8,0xff},{1,0xfe},{7,0xef},{0,0xf7},{1,0xfb},{2,0xfb},{1,0xf7},{1,0xef},
/*  h       i        j        k        l        m        n        o  */
{6,0xef},{5,0xfb},{6,0xf7},{6,0xfb},{6,0xfd},{7,0xfb},{7,0xf7},{5,0xfd},
/*  p       q        r        s        t        u        v        w  */
{5,0xfe},{2,0xfe},{2,0xf7},{1,0xfd},{2,0xef},{5,0xf7},{0,0xef},{2,0xfd},
/*  x       y        z        {        |        }        ~       DEL */
{0,0xfb},{5,0xef},{0,0xfd},{8,0xff},{8,0xff},{8,0xff},{8,0xff},{8,0xff}
};

/* for 1-bit displays with LSBFirst bit order.
 * This lookup table reverses the bit order, and flips the bits.
 */
static unsigned char revbitorder[256]=
  {
  0xff,0x7f,0xbf,0x3f,0xdf,0x5f,0x9f,0x1f,
  0xef,0x6f,0xaf,0x2f,0xcf,0x4f,0x8f,0x0f,
  0xf7,0x77,0xb7,0x37,0xd7,0x57,0x97,0x17,
  0xe7,0x67,0xa7,0x27,0xc7,0x47,0x87,0x07,
  0xfb,0x7b,0xbb,0x3b,0xdb,0x5b,0x9b,0x1b,
  0xeb,0x6b,0xab,0x2b,0xcb,0x4b,0x8b,0x0b,
  0xf3,0x73,0xb3,0x33,0xd3,0x53,0x93,0x13,
  0xe3,0x63,0xa3,0x23,0xc3,0x43,0x83,0x03,
  0xfd,0x7d,0xbd,0x3d,0xdd,0x5d,0x9d,0x1d,
  0xed,0x6d,0xad,0x2d,0xcd,0x4d,0x8d,0x0d,
  0xf5,0x75,0xb5,0x35,0xd5,0x55,0x95,0x15,
  0xe5,0x65,0xa5,0x25,0xc5,0x45,0x85,0x05,
  0xf9,0x79,0xb9,0x39,0xd9,0x59,0x99,0x19,
  0xe9,0x69,0xa9,0x29,0xc9,0x49,0x89,0x09,
  0xf1,0x71,0xb1,0x31,0xd1,0x51,0x91,0x11,
  0xe1,0x61,0xa1,0x21,0xc1,0x41,0x81,0x01,
  0xfe,0x7e,0xbe,0x3e,0xde,0x5e,0x9e,0x1e,
  0xee,0x6e,0xae,0x2e,0xce,0x4e,0x8e,0x0e,
  0xf6,0x76,0xb6,0x36,0xd6,0x56,0x96,0x16,
  0xe6,0x66,0xa6,0x26,0xc6,0x46,0x86,0x06,
  0xfa,0x7a,0xba,0x3a,0xda,0x5a,0x9a,0x1a,
  0xea,0x6a,0xaa,0x2a,0xca,0x4a,0x8a,0x0a,
  0xf2,0x72,0xb2,0x32,0xd2,0x52,0x92,0x12,
  0xe2,0x62,0xa2,0x22,0xc2,0x42,0x82,0x02,
  0xfc,0x7c,0xbc,0x3c,0xdc,0x5c,0x9c,0x1c,
  0xec,0x6c,0xac,0x2c,0xcc,0x4c,0x8c,0x0c,
  0xf4,0x74,0xb4,0x34,0xd4,0x54,0x94,0x14,
  0xe4,0x64,0xa4,0x24,0xc4,0x44,0x84,0x04,
  0xf8,0x78,0xb8,0x38,0xd8,0x58,0x98,0x18,
  0xe8,0x68,0xa8,0x28,0xc8,0x48,0x88,0x08,
  0xf0,0x70,0xb0,0x30,0xd0,0x50,0x90,0x10,
  0xe0,0x60,0xa0,0x20,0xc0,0x40,0x80,0x00
  };


static Display *display;
static Screen *scrptr;
static int screen;
static Window root;
static GC maingc;
static Window mainwin;
static XImage *ximage;
static unsigned char *image;
static int linelen,imagebpp,byteorder,bitorder;
static int black,white;
static unsigned char black_bits0to7,black_bits8to15;
static unsigned char black_bits16to23,black_bits24to31;
static unsigned char white_bits0to7,white_bits8to15;
static unsigned char white_bits16to23,white_bits24to31;
#ifdef MITSHM
static XShmSegmentInfo xshminfo;
#endif

/* needed to catch wm delete-window message */
static Atom proto_atom=None,delete_atom=None;


void closedown(){
#ifdef OSS_SOUND_SUPPORT
   sound_end();
#endif
   zxpclose();
#ifdef MITSHM
   if(mitshm){
      XShmDetach(display,&xshminfo);
      XDestroyImage(ximage);
      shmdt(xshminfo.shmaddr);
      shmctl(xshminfo.shmid,IPC_RMID,0);
   }
#endif
   XAutoRepeatOn(display);
   XCloseDisplay(display);
}


void exit_program(void)
{
closedown();
exit(0);
}



static int is_local_server(name)
char *name;
{
#ifdef HAS_UNAME
   struct utsname un;
#else
   char sysname[MAX_DISP_LEN];
#endif

   if(name[0]==':')return 1;
   if(!strncmp(name,"unix",4))return 1;
   if(!strncmp(name,"localhost",9))return 1;

#ifdef HAS_UNAME
   uname(&un);
   if(!strncmp(name,un.sysname,strlen(un.sysname)))return 1;
   if(!strncmp(name,un.nodename,strlen(un.nodename)))return 1;
#else
   gethostname(sysname,MAX_DISP_LEN);
   if(!strncmp(name,sysname,strlen(sysname)))return 1;
#endif
   return 0;
}


static Display *open_display()
{
   char *ptr;
   char dispname[MAX_DISP_LEN];
   Display *display;

   if((ptr=getenv("DISPLAY")))
     strcpy(dispname,ptr);
   else
     strcpy(dispname,":0.0");
   
   if(!(display=XOpenDisplay(dispname))){
      fprintf(stderr,"Unable to open display %s\n",dispname);
      exit(1);
   }

#ifdef MITSHM   
   mitshm=1;
#else
   mitshm=0;
#endif
   
   if(mitshm && !is_local_server(dispname)){
      fputs("Disabling MIT-SHM on remote X server\n",stderr);
      mitshm=0;
   }
   return display;
}


static int image_init()
{
#ifdef MITSHM
   if(mitshm){
      ximage=XShmCreateImage(display,DefaultVisual(display,screen),
             DefaultDepth(display,screen),ZPixmap,NULL,&xshminfo,
             hsize,vsize);
      if(!ximage){
         fputs("Couldn't create X image\n",stderr);
         return 1;
      }
      xshminfo.shmid=shmget(IPC_PRIVATE,
               ximage->bytes_per_line*(ximage->height+1),IPC_CREAT|0777);
      if(xshminfo.shmid == -1){
         perror("Couldn't perform shmget");
         return 1;
      }
      xshminfo.shmaddr=ximage->data=shmat(xshminfo.shmid,0,0);
      if(!xshminfo.shmaddr){
         perror("Couldn't perform shmat");
         return 1;
      }
      xshminfo.readOnly=0;
      if(!XShmAttach(display,&xshminfo)){
         perror("Couldn't perform XShmAttach");
         return 1;
      }
   } else
#endif
   {
      ximage=XCreateImage(display,DefaultVisual(display,screen),
             DefaultDepth(display,screen),ZPixmap,0,NULL,hsize,vsize,
             8,0);
      if(!ximage){
         perror("XCreateImage failed");
         return 1;
      }
      ximage->data=malloc(ximage->bytes_per_line*(ximage->height+1));
      if(!ximage->data){
         perror("Couldn't get memory for XImage data");
         return 1;
      }
   }
   linelen=ximage->bytes_per_line/SCALE;
   if(linelen!=ZX_VID_X_WIDTH/8 &&	/* 1-bit */
      linelen!=ZX_VID_X_WIDTH/4 &&	/* 4-bit */
      linelen!=ZX_VID_X_WIDTH &&	/* 8-bit */
      linelen!=ZX_VID_X_WIDTH*2 &&	/* 15/16-bit */
      linelen!=ZX_VID_X_WIDTH*3 &&	/* 24-bit */
      linelen!=ZX_VID_X_WIDTH*4)	/* 32-bit */
      fprintf(stderr,"Line length=%d; expect strange results!\n",linelen);
   ximage->data+=linelen;
   image=ximage->data;
   
   imagebpp=DefaultDepth(display,screen);
   /* if 24-bit but padded to 32-bit, use the 32-bit code. */
   if(imagebpp==24 && linelen==ZX_VID_X_WIDTH*4)
     imagebpp=32;
   /* if 4-bit but padded to 8-bit, use 8-bit code. */
   if(imagebpp==4 && linelen==ZX_VID_X_WIDTH)
     imagebpp=8;
   
   /* byte order is important for 15/16/24-bit */
   byteorder=ImageByteOrder(display);
   
   /* and bit order is important for 1-bit (and perhaps 4-bit?) */
   bitorder=BitmapBitOrder(display);
   
   /* get bytes which make up black/white to save time later */
   black_bits0to7=(black&255);
   black_bits8to15=((black>>8)&255);
   black_bits16to23=((black>>16)&255);
   black_bits24to31=((black>>24)&255);
   white_bits0to7=(white&255);
   white_bits8to15=((white>>8)&255);
   white_bits16to23=((white>>16)&255);
   white_bits24to31=((white>>24)&255);

#if SCALE>1
   if(imagebpp==1)
     fprintf(stderr,
       "Warning: xz81 doesn't support SCALE>1 in mono, expect oddities!\n");
#endif
   
   return 0;
}


static void notify()
{
   Pixmap icon;
   XWMHints xwmh;
   XSizeHints xsh;
   XClassHint xch;
   XTextProperty appname, iconname;
   char *apptext;
   char *icontext="xz81";

   icon=XCreatePixmapFromBitmapData(display,root,icon_bits,
        icon_width,icon_height,black,white,DefaultDepth(display,screen));
   apptext="xz81";
   xsh.flags=PSize|PMinSize|PMaxSize;
   xsh.min_width=hsize;
   xsh.min_height=vsize;
   xsh.max_width=hsize;
   xsh.max_height=vsize;
   if(!XStringListToTextProperty(&apptext,1,&appname)){
      fputs("Can't create a TextProperty!",stderr);
      return;
   }
   if(!XStringListToTextProperty(&icontext,1,&iconname)){
      fputs("Can't create a TextProperty!",stderr);
      return;
   }
   xwmh.initial_state=NormalState;
   xwmh.input=1;
   xwmh.icon_pixmap=icon;
   xwmh.flags=StateHint|IconPixmapHint|InputHint;
   xch.res_name="xz81";
   xch.res_class="Xz81";
   XSetWMProperties(display,mainwin,&appname,&iconname,NULL,0,
      &xsh,&xwmh,&xch);

   /* delete-window message stuff, from xloadimage via dosemu. */
   proto_atom =XInternAtom(display,"WM_PROTOCOLS",False);
   delete_atom=XInternAtom(display,"WM_DELETE_WINDOW",False);
   if(proto_atom!=None && delete_atom!=None)
   XChangeProperty(display,mainwin,proto_atom,XA_ATOM,32,
   		PropModePrepend,(char *)&delete_atom,1);
}


void startup()
{
   display=open_display();
   if(!display){
      fputs("Failed to open X display\n",stderr);
      exit(1);
   }
   screen=DefaultScreen(display);
   scrptr=DefaultScreenOfDisplay(display);
   root=DefaultRootWindow(display);
   white=WhitePixel(display,screen);
   black=BlackPixel(display,screen);
   maingc=XCreateGC(display,root,0,NULL);
   XCopyGC(display,DefaultGC(display,screen),~0,maingc);
   XSetGraphicsExposures(display,maingc,0);
   if(image_init()){
      if(mitshm){
         fputs("Failed to create X image - trying again with mitshm off\n",stderr);
         mitshm=0;
         if(image_init())exit(1);
      }
      else exit(1);
   }

   mainwin=XCreateSimpleWindow(display,root,0,0,
             hsize,vsize,0,0,0);
   notify();
   XSelectInput(display,mainwin,KeyPressMask|KeyReleaseMask|
      ExposureMask|EnterWindowMask|LeaveWindowMask|
      StructureNotifyMask);
   XMapRaised(display,mainwin);
   XFlush(display);
   refresh_screen=1;
}


/* x is in chars, y in pixels. d is byte with bitmap in. */
#ifdef INLINE_DRAWPIX
inline
#endif
void drawpix(int x,int y,int d)
{
unsigned char *tmp;
int mask=256;
#if SCALE>1
int j,k,m;
#endif

/* is just me, or was this approach not worth the effort in the end? :-/ */

switch(imagebpp)
  {
  case 32:
    tmp=image+(y*hsize+x*8)*4*SCALE;
    while((mask>>=1))
      {
#if SCALE>1
      for(j=0;j<SCALE;j++,tmp+=(hsize-SCALE)*4)
        for(k=0;k<SCALE;k++)
#endif
          if(byteorder==LSBFirst)
            {
            *tmp++=(d&mask)?black_bits0to7:white_bits0to7;
            *tmp++=(d&mask)?black_bits8to15:white_bits8to15;
            *tmp++=(d&mask)?black_bits16to23:white_bits16to23;
            *tmp++=(d&mask)?black_bits24to31:white_bits24to31;
            }
          else
            {
            *tmp++=(d&mask)?black_bits24to31:white_bits24to31;
            *tmp++=(d&mask)?black_bits16to23:white_bits16to23;
            *tmp++=(d&mask)?black_bits8to15:white_bits8to15;
            *tmp++=(d&mask)?black_bits0to7:white_bits0to7;
            }
#if SCALE>1
      /* we want to move one scaled-up pixel up, then one right */
      tmp-=(hsize*SCALE-SCALE)*4;
#endif
      }
    break;
  
  case 24:
    tmp=image+(y*hsize+x*8)*3*SCALE;
    while((mask>>=1))
      {
#if SCALE>1
      for(j=0;j<SCALE;j++,tmp+=(hsize-SCALE)*3)
        for(k=0;k<SCALE;k++)
#endif
          if(byteorder==LSBFirst)
            {
            *tmp++=(d&mask)?black_bits0to7:white_bits0to7;
            *tmp++=(d&mask)?black_bits8to15:white_bits8to15;
            *tmp++=(d&mask)?black_bits16to23:white_bits16to23;
            }
          else
            {
            *tmp++=(d&mask)?black_bits16to23:white_bits16to23;
            *tmp++=(d&mask)?black_bits8to15:white_bits8to15;
            *tmp++=(d&mask)?black_bits0to7:white_bits0to7;
            }
#if SCALE>1
      /* we want to move one scaled-up pixel up, then one right */
      tmp-=(hsize*SCALE-SCALE)*3;
#endif
      }
    break;
  
  case 15: case 16:
    tmp=image+(y*hsize+x*8)*2*SCALE;
    while((mask>>=1))
      {
#if SCALE>1
      for(j=0;j<SCALE;j++,tmp+=(hsize-SCALE)*2)
        for(k=0;k<SCALE;k++)
#endif
          if(byteorder==LSBFirst)
            {
            *tmp++=(d&mask)?black_bits0to7:white_bits0to7;
            *tmp++=(d&mask)?black_bits8to15:white_bits8to15;
            }
          else
            {
            *tmp++=(d&mask)?black_bits8to15:white_bits8to15;
            *tmp++=(d&mask)?black_bits0to7:white_bits0to7;
            }
#if SCALE>1
      /* we want to move one scaled-up pixel up, then one right */
      tmp-=(hsize*SCALE-SCALE)*2;
#endif
      }
    break;
  
  case 8:
    tmp=image+(y*hsize+x*8)*SCALE;
    while((mask>>=1))
#if SCALE<2
      /* i.e. actual size */
      *tmp++=(d&mask)?black:white;
#else
      {
      m=((d&mask)?black:white);
      for(j=0;j<SCALE;j++)
        for(k=0;k<SCALE;k++)
          tmp[j*hsize+k]=m;
      tmp+=SCALE;
      }
#endif
    break;
  
  /* XXX neither 4-bit nor 1-bit support SCALE>1.
   * XXX also, 4-bit might not work on servers with LSBFirst bit order.
   */
  case 4:
    /* XFree86's VGA16 server seems to act like an 8-bit one
     * which mysteriously has hardly any colours ;-), and I don't
     * have access to any other 4-bit server, so this is untested.
     */
    tmp=image+((y*hsize+x*8)>>1);
    while((mask>>=1))
      {
      static int oldmask;
      oldmask=mask; mask>>=1;
      *tmp++=((((d&oldmask)?black:white)<<4)|((d&mask)?black:white));
      }
    break;
  
  default:	/* assume mono */
    if(bitorder==LSBFirst)
      image[y*linelen+x]=revbitorder[d];	/* table includes bit-flip */
    else
      image[y*linelen+x]=~d;
  }
}


static void process_keypress(kev)
XKeyEvent *kev;
{
   char buf[3];
   KeySym ks;

   XLookupString(kev,buf,2,&ks,NULL);

   switch(ks){
      case XK_F1:
        help=!help;
        break;
      
      case XK_F10:
        exit_program();
        /* doesn't return */
        break;
      
      case XK_Escape:
        if(!ignore_esc) reset81();
        break;
      
      case XK_Return: 
              keyports[6]&=0xfe; break;
      case XK_Control_L: case XK_Shift_L: case XK_Shift_R:
      case XK_Alt_L: case XK_Alt_R: case XK_Meta_L: case XK_Meta_R:
              keyports[0]&=0xfe; break;
      case XK_BackSpace: case XK_Delete:
      case XK_Tab:
      case XK_Up:
      case XK_Down:
      case XK_Left:
      case XK_Right:
              /* XXX unmapped */ break;
#if 0	/* XXX */
      case XK_minus:
              keyports[8]|=0x02; break;
      case XK_underscore:
              keyports[8]|=0x02; break;
      case XK_equal:
              keyports[7]|=0x01; break;
      case XK_plus:
              keyports[7]|=0x01; break;
      case XK_semicolon:
              keyports[9]|=0x10; break;
      case XK_colon:
              keyports[9]|=0x10; break;
      case XK_apostrophe:
              keyports[8]|=0x10; break;
      case XK_quotedbl:
              keyports[3]|=0x02; break;
      case XK_exclam:
              keyports[2]|=0x04; break;
      case XK_at:
              keyports[8]|=0x10; break;
      case XK_numbersign:
              keyports[6]|=0x10; break;
      case XK_dollar:
              keyports[4]|=0x01; break;
      case XK_percent:
              keyports[1]|=0x40; break;
      case XK_asciicircum:
              keyports[6]|=0x01; break;
      case XK_ampersand:
              keyports[7]|=0x02; break;
      case XK_asterisk:
              keyports[8]|=0x01; break;
      case XK_parenleft:
              keyports[9]|=0x02; break;
      case XK_parenright:
              keyports[9]|=0x01; break;
      case XK_comma:
              keyports[8]|=0x80; break;
      case XK_less:
              keyports[8]|=0x80; break;
      case XK_period:
              keyports[9]|=0x80; break;
      case XK_greater:
              keyports[9]|=0x80; break;
      case XK_slash:
              keyports[6]|=0x20; break;
      case XK_question:
              keyports[6]|=0x20; break;
      case XK_backslash: case XK_bar:
              keyports[7]|=0x04; break;
              break;
#endif
      default:
              if((int)(ks-=32)>=0 && ks<128)
                 keyports[keytable[ks].port]&=keytable[ks].mask;
   }
}


static void process_keyrelease(kev)
XKeyEvent *kev;
{
   char buf[3];
   KeySym ks;

   XLookupString(kev,buf,2,&ks,NULL);

   switch(ks){
      case XK_Return: 
              keyports[6]|=1; break;
      case XK_Control_L: case XK_Shift_L: case XK_Shift_R:
      case XK_Alt_L: case XK_Alt_R: case XK_Meta_L: case XK_Meta_R:
              keyports[0]|=1; break;
      case XK_BackSpace: case XK_Delete:
      case XK_Escape:
      case XK_Tab:
      case XK_Up:
      case XK_Down:
      case XK_Left:
      case XK_Right:
              /* XXX unmapped */ break;
#if 0	/* XXX */
      case XK_minus:
              keyports[8]&=~0x02; break;
      case XK_underscore:
              keyports[8]&=~0x02; break;
      case XK_equal:
              keyports[7]&=~0x01; break;
      case XK_plus:
              keyports[7]&=~0x01; break;
      case XK_semicolon:
              keyports[9]&=~0x10; break;
      case XK_colon:
              keyports[9]&=~0x10; break;
      case XK_apostrophe:
              keyports[8]&=~0x10; break;
      case XK_quotedbl:
              keyports[3]&=~0x02; break;
      case XK_exclam:
              keyports[2]&=~0x04; break;
      case XK_at:
              keyports[8]&=~0x10; break;
      case XK_numbersign:
              keyports[6]&=~0x10; break;
      case XK_dollar:
              keyports[4]&=~0x01; break;
      case XK_percent:
              keyports[1]&=~0x40; break;
      case XK_asciicircum:
              keyports[6]&=~0x01; break;
      case XK_ampersand:
              keyports[7]&=~0x02; break;
      case XK_asterisk:
              keyports[8]&=~0x01; break;
      case XK_parenleft:
              keyports[9]&=~0x02; break;
      case XK_parenright:
              keyports[9]&=~0x01; break;
      case XK_comma:
              keyports[8]&=~0x80; break;
      case XK_less:
              keyports[8]&=~0x80; break;
      case XK_period:
              keyports[9]&=~0x80; break;
      case XK_greater:
              keyports[9]&=~0x80; break;
      case XK_slash:
              keyports[6]&=~0x20; break;
      case XK_question:
              keyports[6]&=~0x20; break;
      case XK_backslash: case XK_bar:
              keyports[7]&=~0x04; break;
              break;
#endif
      default:
              if((int)(ks-=32)>=0 && ks<96)
                 keyports[keytable[ks].port]|=~keytable[ks].mask;
   }
   return;
}


void check_events()
{
   static XEvent xev;
   XCrossingEvent *cev;
   
   while (XEventsQueued(display,QueuedAfterReading)){
      XNextEvent(display,&xev);
      switch(xev.type){
         case Expose:
            refresh_screen=1;
            break;
         case ConfigureNotify:
           /* XXX could probably lose this, but tweak XSelectInput() too */
            break;
         case MapNotify: case UnmapNotify: case ReparentNotify:
            break;
         case EnterNotify:
            cev=(XCrossingEvent *)&xev;
            if(cev->detail!=NotifyInferior)
               XAutoRepeatOff(display),XFlush(display);
            break;
         case LeaveNotify:
            cev=(XCrossingEvent *)&xev;
            if(cev->detail!=NotifyInferior)
               XAutoRepeatOn(display),XFlush(display);
            break;
         case KeyPress: process_keypress((XKeyEvent *)&xev);
            break;
         case KeyRelease: process_keyrelease((XKeyEvent *)&xev);
            break;
         case ClientMessage:
            if(xev.xclient.data.l[0]==delete_atom)
              exit_program();	/* got delete message from wm, quit */
            break;
         default:
            fprintf(stderr,"unhandled X event, type %d\n",xev.type);
      }
   }
}




/* draw everything which has changed, then find the smallest rectangle
 * which covers all the changes, and update that.
 */
void update_scrn(){
   unsigned char *ptr,*optr;
   int x,y,d;
   int xmin,ymin,xmax,ymax;
     
xmax=0;
ymax=0;
xmin=ZX_VID_X_WIDTH-1;
ymin=ZX_VID_X_HEIGHT-1;

for(y=0;y<ZX_VID_X_HEIGHT;y++)
  {
  ptr=scrnbmp+(y+ZX_VID_X_YOFS)*ZX_VID_FULLWIDTH/8+ZX_VID_X_XOFS/8;
  optr=scrnbmp_old+(ptr-scrnbmp);
  for(x=0;x<ZX_VID_X_WIDTH;x+=8,ptr++,optr++)
    {
    d=*ptr;
    
    if(d!=*optr || refresh_screen)
      {
      /* update size of area to be drawn */
      if(x<xmin) xmin=x;
      if(y<ymin) ymin=y;
      if(x+7>xmax) xmax=x+7;
      if(y>ymax) ymax=y;
      drawpix(x>>3,y,invert_screen?~d:d);
      }
    }
  }

/* now, copy new to old for next time */
memcpy(scrnbmp_old,scrnbmp,ZX_VID_FULLHEIGHT*ZX_VID_FULLWIDTH/8);

if(refresh_screen)
  xmin=0,ymin=0,xmax=ZX_VID_X_WIDTH-1,ymax=ZX_VID_X_HEIGHT-1;

if(xmax>=xmin && ymax>=ymin)
  {     
#ifdef MITSHM
   if(mitshm)
     XShmPutImage(display,mainwin,maingc,ximage,
       xmin*SCALE,ymin*SCALE,xmin*SCALE,ymin*SCALE,
       (xmax-xmin+1)*SCALE,(ymax-ymin+1)*SCALE,0);
   else
#endif
     XPutImage(display,mainwin,maingc,ximage,
       xmin*SCALE,ymin*SCALE,xmin*SCALE,ymin*SCALE,
       (xmax-xmin+1)*SCALE,(ymax-ymin+1)*SCALE);
  }

XFlush(display);
refresh_screen=0;
}


int main(int argc,char *argv[])
{
parseoptions(argc,argv);

startup();
initmem();
loadhelp();
zxpopen();

#ifdef OSS_SOUND_SUPPORT
if(sound)
  sound_init();
#endif
startsigsandtimer();
mainloop();

/* doesn't ever get here really, but all the same... */
exit_program();
return(0);	/* make -Wall happy */
}
