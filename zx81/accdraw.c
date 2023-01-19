#include <stdio.h>
#include <stdlib.h>
#include "../config.h"
#include "../zx81config.h"
#include "../common.h"
#include "zx81.h"

/* odd place to have this, but the display does work in an odd way :-) */
unsigned char scrnbmp[ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT/8];    /* displayed */
unsigned char scrnbmp_old[ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT/8];
                                                /* checked against for diffs */

/* chroma */
unsigned char scrnbmpc[ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT];   /* displayed */

/* Minimized (so no longer really Accurate...)
  from the libretro adaptation of the EightyOne source */

extern ZX81 zx81;
extern BYTE scanline[];
extern long noise;
extern int scanline_len;
extern int sync_len;
extern int sync_valid;
int RasterX = 0;
int RasterY;
int TVH;
int TVP;
static int ScanLen;
static int dest;

#define HTOL 400
#define VTOLMIN 260
#define VTOLMAX 359

#define HMIN 10
#define VMIN 350

static int HSYNC_TOLERANCE = HTOL;
static int VSYNC_TOLERANCEMIN = VTOLMIN;
static int VSYNC_TOLERANCEMAX = VTOLMAX;
static int HSYNC_MINLEN = HMIN;
static int VSYNC_MINLEN = VMIN;

void Plot(int c)
{
	int k, kh, kl;
	unsigned char b, m;

	// if (c!=208) printf("%d %d %d %d\n",dest,RasterX,RasterY,c);

	if (RasterX >= ZX_VID_FULLWIDTH) {
		//		printf("RasterX: %d\n", RasterX);
		return;
	}
	k = dest + RasterX;
	if (k >= ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT) {
		//		printf("dest: %d, RasterX: %d, RasterY: %d\n", dest,RasterX, RasterY);
		return;
	}

	kh = k >> 3;
	kl = k & 7;
	m = 0x80 >> kl;

	b = scrnbmp[kh];
	if (c&0x01) b |= m; else b &= ~m;
	if (zx81.colour!=COLOURDISABLED) scrnbmpc[k] = c >> 4;
	scrnbmp[kh] = b;
}

int myrandom( int x )
{
  return rand() % ( x + 1 );
}

void add_blank( int tstates, BYTE colour )
{
  while ( tstates-- > 0 )
  {
    scanline[ scanline_len++ ] = colour;
    scanline[ scanline_len++ ] = colour;
  }
}

void AccurateInit()
{
  dest = 0;

  RasterX = 0;
  RasterY = myrandom( 256 );

  ScanLen = 2 + machine.tperscanline * 2;

  HSYNC_MINLEN = 10;

  TVP = ZX_VID_X_WIDTH; // EO
  TVH = 380;
}

void AccurateDraw( void )
{
  register int i, c;

  for ( i = 0; i < scanline_len; i++ )
  {
    c = scanline[ i ];

    Plot(c);

    RasterX++;

    if ( RasterX > ScanLen )
    {
      RasterX = 0;
      RasterY++;
      dest += TVP;
      if ( RasterY >= TVH )
      {
	i = scanline_len + 1;
        sync_valid = 1;
      }
    }
  }

  if ( sync_len < HSYNC_MINLEN )
  {
    sync_valid = 0;
  }
  
  if ( sync_valid )
  {
    if ( RasterX > HSYNC_TOLERANCE )
    {
      RasterX = 0;
      RasterY++;
      dest += TVP;
    }
    
    if ( RasterY >= TVH || RasterY>=VSYNC_TOLERANCEMAX
	 ||  (sync_len>VSYNC_MINLEN
	      && RasterY>VSYNC_TOLERANCEMIN)) {
      RasterX = RasterY = 0;
      dest = 0;
    }

    sync_len = 0;
    sync_valid = -1;
  }

}
