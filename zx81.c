#include "zx81.h"
#include "sdl.h"
#include "sdl_loadsave.h"
#include "sdl_sound.h"
#include "config.h"
#include "zx81config.h"
#include "common.h"
#include "sound.h"
#include "z80/z80.h"
#include "w5100.h"

#include "types.h"
#include "dissz80.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#define LASTINSTNONE  0
#define LASTINSTINFE  1
#define LASTINSTOUTFE 2
#define LASTINSTOUTFD 3
#define LASTINSTOUTFF 4

#ifdef ZXMORE
#define WAITMOD
#endif

#ifdef ZXNU
#ifndef VDRIVE
#define VDRIVE
#endif
#endif

// #define VRCNTR

ZX81 zx81;
BYTE *memory;
extern BYTE *sz81mem;
extern int rwsz81mem;

/* odd place to have this, but the display does work in an odd way :-) */
unsigned char scrnbmp_new[ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT/8];  /* written */
unsigned char scrnbmp[ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT/8];      /* displayed */

/* chroma */
unsigned char scrnbmpc_new[ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT];   /* written */
unsigned char scrnbmpc[ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT];       /* displayed */

static int RasterX = 0;
static int RasterY = 0;
static int TVP;
static int dest;

/* TV specifications */

#define HTOLMIN 414-30
#define HTOLMAX 414+30
#define VTOLMIN 310-100
#define VTOLMAX 310+100
#define HMIN 8
#define HMAX 32
#define VMIN 170

static int HSYNC_TOLERANCEMIN = HTOLMIN;
static int HSYNC_TOLERANCEMAX = HTOLMAX;
static int VSYNC_TOLERANCEMIN = VTOLMIN;
static int VSYNC_TOLERANCEMAX = VTOLMAX;
static int HSYNC_MINLEN = HMIN;
static int HSYNC_MAXLEN = HMAX;
static int VSYNC_MINLEN = VMIN;

int int_pending, nmi_pending, hsync_pending;
long noise;
int border, ink, paper;
int SelectAYReg;
BYTE font[512];
BYTE memhrg[1024];
int borrow=0;

unsigned long tstates=0;
unsigned long tsmax=0;
unsigned long frames=0;

/* I/O port 1 allows reading of the zx81 structure */
int configbyte=0;

int NMI_generator;
int VSYNC_state, HSYNC_state, SYNC_signal;
int psync, sync_len;
int setborder=0;
int LastInstruction;
int MemotechMode=0;
int shift_register, shift_reg_inv;
int rowcounter=0;
int rowcounter_hold = 0;
int zx81_stop=0;
int hsync_counter=0;
int ispeedup;
int ffetch;

/* ZXmore stuff */

int zxmspeed=1;
int zxmoff=0;
int zxmnmicounter=0;
int zxmnmi=0;
int zxmfetching=0;
int zxmroml=0x3f, zxmraml=0xf3;
int zxmvideo=0;
int zxmrowcounter=0;
unsigned int zxmoffset=0;

int zxmflash=0, zxmmod=0;

#ifdef ZXNU
int zxnu_latch=0;
#endif

extern void loadrombank(int offset);

#ifdef SNDPRC
extern int sp_on;
extern void sp_start(int idx);
#endif

#ifdef ZXPAND
extern void ZXpand_Update(int millis);
extern void ZXpand_IO_Write(int addr, int data);
extern int ZXpand_IO_ReadStatus();
extern int ZXpand_IO_Read(int addr);
#endif

#ifdef APU
void am_write_io8(uint8_t addr, uint8_t data);
uint8_t am_read_io8(uint8_t addr);
#endif

#ifdef ZXMORE

int zxmAddress(int Address, int rd)
{
  int romsize, swap, offset;

  romsize = 0x1000 + ((zxmraml & 0x0f) << 12);
  swap = zxmraml & 0x10;

  if ((swap && Address>=romsize) || (!swap && Address<romsize)) {

/* ram access */

    // if (zxmvideo) Address &= 0x7fff; /* more or less what happened with early CPLDs */

    if ((!zxmnmi || !rd) && (zxmroml&0x40)) Address |= 0x8000;

#ifdef ZXMSHMEM
    if ((zxmnmi && rd) || (Address>=0x2000 && Address<0x4000)) offset = 0x70000;
#else
    if (zxmnmi && rd) offset = 0x70000;
#endif
    else offset = (zxmraml & 0xe0) << 11;

    offset += 0x80000;

  } else {

/* rom access */

    if (zxmnmi) offset = 0x70000;
    else offset = (zxmroml & 0x07) << 16;

  }

  return Address + offset;

}

void zxm_writebyte(int Address, int Data)
{
        int iAddress, ib, ie;

	iAddress = zxmAddress(Address,0);

	if (iAddress>=0x80000) {
		memory[iAddress] = Data;
		return;
	}

	if (zxmroml&0x08) return;

/* modifications are saved in exitmem() in common.c */

	switch (zxmflash) {
		case    0: if (Address==0x5555 && Data==0xaa) zxmflash++;
			   break;
		case    1: if (Address==0x2aaa && Data==0x55) zxmflash++; else zxmflash=0;
			   break;
		case    2: if (Address==0x5555) zxmflash=Data; else zxmflash=0;
			   break;
		case 0x80: if (Address==0x5555 && Data==0xaa) zxmflash++; else zxmflash=0;
			   break;
		case 0x81: if (Address==0x2aaa && Data==0x55) zxmflash++; else zxmflash=0;
			   break;
		case 0x82: ib = iAddress & 0xff000;
			   ie = ib | 0x00fff;
			   while (ib<=ie) memory[ib++] = 0xff;
//			   fprintf(stdout,"SERS %04x: %05x %02x %02x\n", z80.pc.w, iAddress, Data, zxmflash);
		           zxmflash = 0;
			   zxmmod = 1;
			   break;
		case 0xa0: memory[iAddress] = Data;
//			   fprintf(stdout,"BPRG %04x: %05x %02x %02x\n", z80.pc.w, iAddress, Data, zxmflash);
		           zxmflash = 0;
			   zxmmod = 1;
			   break;
		default: zxmflash = 0;
	}
}

BYTE zxm_readbyte(int Address)
{
        int iAddress;
        int romsize;

	romsize = 0x1000 + ((zxmraml & 0x0f) << 12);
	if (!zxmfetching && zxmnmi && Address>=romsize && (zxmraml&0xe0)!=0xe0) {
	  zxmnmi = 0;
	}

	iAddress = zxmAddress(Address,1);

	return memory[iAddress];
}

#else

void zxm_writebyte(int Address, int Data)
{
	memory[Address] = Data;
}

BYTE zxm_readbyte(int Address)
{
	return memory[Address];
}

#endif

/* in common.c */
void aszmic4hacks();
void aszmic7hacks();
void kcomm(int a);
unsigned char lcomm(int a1, int a2);

void disassemble(const unsigned int dAddr, const BYTE opcode)
{
        DISZ80  *d;                     /* Pointer to the Disassembly structure */
        int     err;

/* Allocate the dZ80 structure */
        d = (DISZ80*) malloc(sizeof(DISZ80));
        if (d == NULL)
                {
                printf("dz80: cannot allocate %ld bytes\n", sizeof(DISZ80));
                exit(1);
                }

/* Set up dZ80's structure - it's not too fussy */
        memset(d, 0, sizeof(DISZ80));

/* Set the default radix and strings (comments and "db") */ 
        dZ80_SetDefaultOptions(d);              

/* Set the CPU type */
        d->cpuType = DCPU_Z80;

/* Set the start of the Z80's memory space - not used */
        d->mem0Start = NULL;

/* Indicate we're disassembling a single instruction */
        d->flags |= DISFLAG_SINGLE;

/* Set the disassembly address */
        d->start = d->end = dAddr;

/* :-) */
	d->op = opcode;
	d->availop = 1;

        err = dZ80_Disassemble(d);
        if (err != DERR_NONE)
                        {
                        printf("**** dZ80 error:  %s\n", dZ80_GetErrorText(err));
                        }
/* Display the disassembled line, using the hex dump and disassembly buffers in the DISZ80 structure */
#ifdef ZXMORE                
        printf(" %6ld %02X %02X %d %d %04X %04X %04X %04X %04X %04X %04X %04X%10s:  %s\n",
	       tstates, zxmroml, zxmraml, zxmnmi, zxmvideo, dAddr,
	       z80.af.w, z80.bc.w, z80.de.w, z80.hl.w, z80.sp.w, z80.ix.w, z80.iy.w,
	       d->hexDisBuf, d->disBuf);
#else
        printf(" %6ld %04X %04X %04X %04X %04X %04X %10s:  %s\n",
	       tstates, dAddr,
	       z80.af.w, z80.bc.w, z80.de.w, z80.hl.w, z80.sp.w,
	       d->hexDisBuf, d->disBuf);
#endif
        free(d);
}

/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
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
 * zx81.c
 *
 */

/* Used to be in AccDraw */

void Plot(int c)
{
	int k, kh, kl;
	unsigned char b, m;

	k = TVP + dest + RasterX;
	RasterX++;
	if (RasterX >= ZX_VID_FULLWIDTH) return;

	if (k >= ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT) return;

	kh = k >> 3;
	kl = k & 7;
	m = 0x80 >> kl;

	b = scrnbmp_new[kh];
	if (c&0x01) b |= m; else b &= ~m;
	if (zx81.colour!=COLOURDISABLED) scrnbmpc_new[k] = c >> 4;
	scrnbmp_new[kh] = b;
}

int myrandom( int x )
{
  return rand() % ( x + 1 );
}

BYTE zx81_readbyte(int Address);
void zx81_writebyte(int Address, int Data);

void h4th_store(int n)
{
	FILE *f;
	char filnam[10];
	int blkloc, i, j;
	unsigned char data;

	sprintf(filnam,"%05d.4th", n);
	blkloc = (zx81_readbyte(0xfc77) << 8) | zx81_readbyte(0xfc76);

	fprintf(stderr,"STOREing %s...\n",filnam);

	f = fopen(filnam,"w");
	for (j=0; j<16; j++) {
		for (i=0; i<32; i++) {
			data = zx81_readbyte(blkloc++);
			data = (data&0x80) + 32 + (data&0x7f);
			fprintf(f,"%c", data);
		}
		blkloc++;
		fprintf(f,"\n");
	}
	fclose(f);
}

int h4th_load(int n)
{
	FILE *f;
	char filnam[10];
	int blkloc, blkend, i, inv;
	unsigned char data;

	sprintf(filnam,"%05d.4th", n);
	blkloc = (zx81_readbyte(0xfc77) << 8) | zx81_readbyte(0xfc76);
	blkend = blkloc + 16*32;

	fprintf(stderr,"LOADing %s...\n",filnam);

	while (blkloc<blkend) zx81_writebyte(blkloc++,0);
	blkloc = blkend - 16*32;

	f = fopen(filnam,"r");
	if (!f) return 0;

	i = 0;
	while (blkloc<blkend) {
		data = fgetc(f);
		if (feof(f)) break;
		inv = data&0x80;
		data &= 0x7f;
		if (data < 0x20) {
			while (i&0x1f) { blkloc++; i++; }
			i = 0;
		} else {
			if (data>=0x60) data -= 0x40; else data -= 0x20;
			if (inv) data |= 0x80;
			zx81_writebyte(blkloc++,data);
			i++;
		}
	}
	fclose(f);

	return 1;
}

/* zx97 code has been removed */

/* In sz81, the extended instructions were called within edops.c;
   to keep the current Philip Kendall's z80_ed.c clean, the instructions are
   tested here in line with EightyOne */

/* TODO: zxmoffset ;-) */

int PatchTest(int pc)
{
        int byte, edbyte, offset=0;

#ifdef ZXMORE
	offset = (zxmroml & 0x07) << 16;
	if (offset==0x70000) return(pc);
#endif

	byte = memory[pc+offset];

	if (byte==0xed) {
		edbyte = memory[pc+offset+1];
#ifdef ZXMORE
		offset += 0x80000;
		if (zxmroml&0x40)  offset += 0x8000;
#endif
		switch (edbyte) {
		case 0xfa: if (pc==0x0692 || pc==0x0693) {
				kcomm(z80.hl.w);
				return(pc+2);
			    }
			    if (pc==0x157b) {
				h4th_store(z80.hl.w);
				return(pc+2);
			    }
			    break;
		case 0xfb: if (pc==0x06c5 || pc==0x06c6) {
				z80.af.b.h=lcomm(z80.af.b.h,z80.hl.w);
				return(pc+2);
			    }
			    if (pc==0x1611) {
				if (h4th_load(z80.hl.w)) return(pc+0x7b);
				return(pc+2);
			    }
			    break;
	        case 0xfc: if (pc==0x348 && zx81.machine==MACHINEZX81) {
#ifdef ZXMORE
			     if (zxmroml&0x40) zxmoffset += 0x8000;
#endif
			     if (z80.hl.w < 0x8000)
			       sdl_load_file(z80.hl.w+offset,LOAD_FILE_METHOD_NAMEDLOAD);
		             else
			       sdl_load_file(z80.hl.w+offset,LOAD_FILE_METHOD_SELECTLOAD);
#ifdef ZXMORE
			     if (zxmroml&0x40) zxmoffset -= 0x8000;
#endif
		             return(z80.pc.w);
	                   }
	                   if (zx81.machine==MACHINEZX80 && pc==0x206) {
#ifdef ZXMORE
			     if (zxmroml&0x40) zxmoffset += 0x8000;
#endif
		               sdl_load_file(z80.hl.w+offset,LOAD_FILE_METHOD_SELECTLOAD);
#ifdef ZXMORE
			     if (zxmroml&0x40) zxmoffset -= 0x8000;
#endif
		               return(pc+2);
	                   }
			   if (pc==0x19f0 && zx81.machine==MACHINELAMBDA) {
#ifdef ZXMORE
			     if (zxmroml&0x40) zxmoffset += 0x8000;
#endif
			     if (z80.hl.w < 0x8000)
			       sdl_load_file(z80.hl.w+offset,LOAD_FILE_METHOD_NAMEDLOAD);
		             else
			       sdl_load_file(z80.hl.w+offset,LOAD_FILE_METHOD_SELECTLOAD);
#ifdef ZXMORE
			     if (zxmroml&0x40) zxmoffset -= 0x8000;
#endif
		             return(z80.pc.w);
	                   }
			   break;
		case 0xfd: if (zx81.machine==MACHINEZX81 && pc==0x02fc) {
#ifdef ZXMORE
			        if (zxmroml&0x40) zxmoffset += 0x8000;
#endif
				sdl_save_file(z80.hl.w+offset,SAVE_FILE_METHOD_NAMEDSAVE);
#ifdef ZXMORE
			        if (zxmroml&0x40) zxmoffset -= 0x8000;
#endif
				return(pc+2);
			   }
			   if (zx81.machine==MACHINEZX80 && pc==0x01b6) {
#ifdef ZXMORE
			        if (zxmroml&0x40) zxmoffset += 0x8000;
#endif
			        sdl_save_file(z80.hl.w+offset,SAVE_FILE_METHOD_UNNAMEDSAVE);
#ifdef ZXMORE
			        if (zxmroml&0x40) zxmoffset -= 0x8000;
#endif
		                return(pc+2);
		           }
			   if (zx81.machine==MACHINELAMBDA && pc==0x0d0d) {
#ifdef ZXMORE
			        if (zxmroml&0x40) zxmoffset += 0x8000;
#endif
			        sdl_save_file(z80.hl.w+offset,SAVE_FILE_METHOD_UNNAMEDSAVE);
#ifdef ZXMORE
			        if (zxmroml&0x40) zxmoffset -= 0x8000;
#endif
		                return(pc+2);
		           }
			    break;
		default  : ;
		}
	}

	if (zx81.single_step) {
		printf("PC: %04X  OP: %02X  SP: %04X\n", pc, byte, z80.sp.w);
		printf("AF: %02X %02X\n", z80.af.b.h, z80.af.b.l);
		printf("BC: %04X  DE: %04X  HL: %04X\n", z80.bc.w, z80.de.w, z80.hl.w);
		printf("\n");
	}

        return(pc);
}

int zx81_contend(int Address, int states, int time)
{
        return(time);
}

void zx81_writebyte(int Address, int Data)
{
#ifdef VISUALS
	if (Address>=0x8000 && !(Data&64) && !NMI_generator) {
		shift_register |= Data;
	}
#endif
        noise = (noise<<8) | Data;

        if (zx81.aytype == AY_TYPE_QUICKSILVA)
        {
                if (Address == 0x7fff) SelectAYReg=Data&15;
                if (Address == 0x7ffe) sound_ay_write(SelectAYReg,Data,0);
        }

        // The lambda colour board has 1k of RAM mapped between 8k-16k (8 shadows)
        // with a further 8 shadows between 49152 and 57344.

        if (zx81.colour==COLOURLAMBDA && ((Address>=8192 && Address<16384)
					  || (Address>=49152 && Address<57344)))
        {
		if ((Address&0x3000) == 0x3000) {
			printf("Write Lambda colour RAM (0x%04X:0x%02X)\n", Address, Data);
			return;
		}
                Address = (Address&1023)+8192;
                zxm_writebyte(Address,Data);
                return;
        }

        if (zx81.chrgen==CHRGENQS && Address>=0x8400 && Address<=0x87ff)
        {
                font[Address-0x8400]=Data;
                zx81.enableqschrgen=1;
        }

        if (Address>zx81.RAMTOP) {
		if (Address==0x8003) {
			if (Data==0x80 && sdl_aszmicrom.state) {
				memcpy(mem,sdl_aszmicrom.data,4096);
				printf("Going to ASZMIC version %d...\n",mem[0x16]);
				if (mem[0x16]==0x07) aszmic7hacks(); else aszmic4hacks();
				memcpy(mem+4096,mem,4096);
			} else if (Data==0x0f) {
				printf("Going to BASIC...\n");
				memcpy(mem,sdl_zx81rom.data,8192);
			}
		}
		if (zx81.RAMTOP==0xbfff) Address &= 0x7fff;
		else Address = (Address&(zx81.RAMTOP));
	}

        if (Address<=zx81.ROMTOP && zx81.protectROM)
        {
                if ((zx81.truehires==HIRESMEMOTECH) && (Address<1024))
                                memhrg[Address]=Data;
#ifdef ZXMORE
// possibly flash programming
		zxm_writebyte(Address,Data);
#endif
                return;
        }

#ifdef FNMI
	/* 50 notes music interface" by Gaetano Marano, implementation by Stefano Bodrato */
        if (Address>8191 && Address<16384) sound_fnmi(Data);
#endif

        if (Address>8191 && Address<16384 && zx81.shadowROM && zx81.protectROM) return;
        if (Address<10240 && zx81.truehires==HIRESMEMOTECH) return;
        if (Address>=10240 && Address<12288 && zx81.truehires==HIRESG007) return;

        if (zx81.wsz81mem && Address>=0x4000 && Address<0x8000) sz81mem[Address-0x4000]=Data;
        zxm_writebyte(Address,Data);
}

BYTE zx81_readbyte(int Address)
{
        int data;

        // The lambda colour board has 1k of RAM mapped between 8k-16k (8 shadows)
        // with a further 8 shadows between 49152 and 57344.

        if (zx81.colour==COLOURLAMBDA && ((Address>=8192 && Address<16384)
					  || (Address>=49152 && Address<57344)))
        {
		if ((Address&0x3000) == 0x3000) {
			data=zxm_readbyte(Address);
			printf("Read Lambda colour RAM (0x%04X:0x%02X)\n", Address, data);
		} else {
			Address = (Address&1023)+8192;
			data=zxm_readbyte(Address);
		}
                return(data);
        }

        if (zx81.rsz81mem && Address>=0xc000) return sz81mem[Address-0xc000];

        if (Address<=zx81.RAMTOP) data=zxm_readbyte(Address);
	//        else data=memory[(Address&(zx81.RAMTOP-16384))+16384];
        else if (zx81.RAMTOP==0xbfff) data=zxm_readbyte(Address&0x7fff);
	else data=zxm_readbyte(Address&zx81.RAMTOP);

        if ((Address<1024 && (zx81.truehires==HIRESMEMOTECH)) && (z80.i&1))
                        data=memhrg[Address];

        if ((Address>=0x0c00 && Address<=0x0cff) && (zx81.truehires==HIRESG007))
	  data=zxm_readbyte(Address+8192);

        if ((Address<256 || (Address>=512 && Address<768))
	    && (z80.i&1) && (zx81.truehires==HIRESG007))
	  data=zxm_readbyte(Address+8192);

#ifdef VISUALS
	if (Address>=0x8000 && !(data&64) && !NMI_generator) {
		shift_register |= data;
	}
#endif
        noise = (noise<<8) | data;
        return(data);
}

// BYTE opcode_fetch(int Address)
//
// Given an address, opcode fetch return the byte at that memory address,
// modified depending on certain circumstances.
// It also loads the video shift register and generates video noise.
//
// If Address is less than M1NOT, all code is executed,
// the shift register is cleared and video noise is set to what is on
// the data bus.
//
// If Address >= M1NOT, and bit 6 of the fetched opcode is not set
// a NOP is returned and we load the shift register accordingly,
// depending on which video system is in use (WRX/Memotech/etc.)
//
// The ZX81 has effectively two busses.  The ROM is on the first bus
// while (usually) RAM is on the second.  In video generation, the ROM
// bus is used to get character bitmap data while the second bus
// is used to get the display file.  This is important because depending
// on which bus RAM is placed, it can either be used for extended
// Fonts OR WRX style hi-res graphics, but never both.

BYTE zx81_opcode_fetch_org(int Address)
{
#ifdef ZXPAND
        static int calls = 0;
#endif
        int inv;
        int opcode, bit6, update=0;
	BYTE data;
#ifndef ZXMORE
        BYTE data2;
#endif

#ifdef ZXPAND
// very rough timing here;
// assuming a 1mhz call rate it will be 1ms every 1000 calls.
// it may be too rough, with "in" and "jp" instructions;
// what is the unit of the delay in zxpand/zxpandcom.h"?
// ms is too much; for now microseconds are assumed.
// 10 calls may be 30 us.

        ++calls;
        if (calls == 10)
        {
                calls = 0;
                ZXpand_Update(1);
        }
#endif

#ifdef ZXMORE
        if (!zxmvideo || Address<zx81.m1not)
#else
        if (Address<zx81.m1not)
#endif
        {
                // This is not video related, so just return the opcode
                // and make some noise onscreen.
	        zxmfetching = 1;
                data = zxm_readbyte(Address);
	        zxmfetching = 0;
#ifdef ZXMORE
	        if (data==0xc9) zxmnmi = 0;
#endif
                noise |= data;
                return(data);
        }

        // We can only execute code below M1NOT.  If an opcode fetch occurs
        // above M1NOT, we actually fetch (address&32767).  This is important
        // because it makes it impossible to place the display file in the
        // 48-64k region if a 64k RAM Pack is used.  How does the real
        // Hardware work?

#ifdef ZXMORE
        data = zxm_readbyte((Address>=zx81.m1not)?Address&32767:Address);
#else
        data = zxm_readbyte((Address>=49152)?Address&32767:Address);
#endif
        opcode=data;
        bit6=opcode&64;

        // Since we got here, we're generating video (ouch!)
        // Bit six of the opcode is important.  If set, the opcode
        // gets executed and nothing appears onscreen.  If unset
        // the Z80 executes a NOP and the code is used to somehow
        // generate the TV picture (exactly how depends on which
        // display method is used)

        if (!bit6) opcode=0;
        inv = data&128;

        // First check for WRX graphics.  This is easy, we just create a
        // 16 bit Address from the IR Register pair and fetch that byte
        // loading it into the video shift register.
        if (z80.i>=zx81.maxireg && zx81.truehires==HIRESWRX && !bit6)
        {
                if (zx81.colour==COLOURCHROMA)
                {
#ifdef ZXMORE
			ink = 0;
			paper = 15;
#else
                        int c;
                        // If the Chroma 81 interface is enabled, we had better fetch
                        // the ink and paper colour from memory too.

                        c = zxm_readbyte(Address);
                                
                        ink = c&15;
                        paper = (c>>4) & 15;
#endif
		}

                data=zxm_readbyte((z80.i<<8) | (z80.r7 & 128) | ((z80.r-1) & 127));
                update=1;
        }
        else if ((z80.i&1) && MemotechMode)
        {
                // Next Check Memotech Hi-res.  Memotech is only enabled
                // When the I register is odd.
		// TODO: RasterX<66 check

                if ((opcode!=118 || RasterX<(13+58-2)*2) && RasterY>=56 && RasterY<=(56+192))
                {
                        opcode=0;
                        inv=(MemotechMode==3);
                        update=1;
                }
        }
        else if ((z80.i&1) && (zx81.truehires==HIRESG007))
        {
                // Like Memotech, G007 is enabled when I is odd.
                // However, it is much simpler, in that it disables
                // the bit 6 detection entirely and relies on the R
                // register to generate an interupt at the right time.

                opcode=0;
                inv=0;
                update=1;
        }
        else if (!bit6)
        {
                // If we get here, we're generating normal Characters
                // (or pseudo Hi-Res), but we still need to figure out
                // where to get the bitmap for the character from

                if (zx81.colour==COLOURCHROMA)
                {
#ifdef ZXMORE
			ink = 0;
			paper = 15;
#else
                        int c;

                        // If the Chroma 81 interface is enabled, we had better fetch
                        // the ink and paper colour from memory too.

                        if (zx81.chromamode&0x10) {    // Attribute file
                                c=zxm_readbyte(Address);
			} else {                       // Character code
                                data2 = ((data&0x80)>>1) | (data&0x3f);
	                        c=zxm_readbyte(0xc000 + (data2<<3) + rowcounter);
			}
                                
                        ink = c&15;
                        paper = (c>>4) & 15;
#endif
                }
                // First try to figure out which character set we're going
                // to use if CHR$x16 is in use.  Else, standard ZX81
                // character sets are only 64 characters in size.

                if ((zx81.chrgen==CHRGENCHR16 && (z80.i&1))
		 || (zx81.chrgen==CHRGENQS && zx81.enableqschrgen))
                        data = ((data&128)>>1)|(data&63);
#if ZXMORE
                else if (z80.i&1) data = ((data&128)>>1)|(data&63);
#endif
                else data = data&63;

                // If I points to ROM, OR I points to the 8-16k region for
                // CHR$x16, we'll fetch the bitmap from there.
                // Lambda and the QS Character board have external memory
                // where the character set is stored, so if one of those
                // is enabled we better fetch it from the dedicated
                // external memory.
                // Otherwise, we can't get a bitmap from anywhere, so
                // display 11111111 (??What does a real ZX81 do?).

                if (z80.i<64 || (z80.i>=128 && z80.i<192 && zx81.chrgen==CHRGENCHR16))
                {
                        if ((zx81.extfont && z80.i<32) || (zx81.chrgen==CHRGENQS && zx81.enableqschrgen))
                                data=font[(data<<3) + rowcounter];
                        else    data=zxm_readbyte((((z80.i&254)<<8) + (data<<3)) + rowcounter);
                }
                else data=255;

                update=1;
        }

        if (update)
        {
                // Update gets set to true if we managed to fetch a bitmap from
                // somewhere.  The only time this doesn't happen is if we encountered
                // an opcode with bit 6 set above M1NOT.

                if (zx81.colour==COLOURLAMBDA)
                {
                        int c;

                        // If Lambda colour is enabled, we had better fetch
                        // the ink and paper colour from memory too.

                        c = zxm_readbyte(8192+(Address&1023)+1);

                        ink = c&15;
                        paper = (c>>4) & 15;

                        if (setborder) // TODO
                        {
                                border=0;
                                setborder=0;
                        }
                }
                else if (zx81.colour==COLOURCHROMA)
                {
                        if (setborder)
                        {
                                border=zx81.chromamode & 15;
                                setborder=0;
                        }
                }

                // Finally load the bitmap we retrieved into the video shift
                // register, remembering to make some video noise too.

                shift_register |= data;
                shift_reg_inv |= inv? 255:0;
                if (zx81.machine==MACHINELAMBDA) noise |= (Address>>8);
                else noise |= z80.i;
                return(0);
        }
        else
        {
                // This is the fallthrough for when we found an opcode with
                // bit 6 set in the display file.  We actually execute these
                // opcodes, and generate the noise.

                if (zx81.colour==COLOURLAMBDA)
                {
                        ink = paper = 0;
		}

                if (zx81.colour==COLOURCHROMA)
                {
                        ink = paper = zx81.chromamode & 15;
		}

                noise |= data;
                return(opcode);
        }
}

BYTE zx81_opcode_fetch(int Address)
{
        BYTE opcode;

	opcode = zx81_opcode_fetch_org(Address);

#if 0
	if ((opcode==0xcd || (opcode&0xc7) == 0xc4) && Address>0x08cf ) // one of the nine calls
	  disassemble(Address, opcode);
#endif

	if (Address>=sdl_emulator.bdis && Address<=sdl_emulator.edis && ffetch)
	        disassemble(Address, opcode);
	ffetch = 0;

	return opcode;
}

#ifdef ZXMORE

BYTE zxmusbb[0x10000];
int zxmusbr=0, zxmusbrl=0;
int zxmusbw=0, zxmusbwf=0, zxmusbwl=0;
int zxmusbc=0, zxmusbs=0;
FILE *zxmusbf;
#define ZXMUSBCS 1018

void load_usb(int reset)
{
	static int stage, total, parts;
	int n, n1, n2, n3, n4, i;
	struct stat buf;

	zxmusbr = zxmusbrl = 0;

#ifdef DEBUGUSB
        fprintf(stdout,"load_usb %d %d\n", reset, stage);
#endif

	if (reset) {
		stage = 1;
		return;
	}

	i = 0;
	switch (stage) {
		case 1: zxmusbb[18]=0;
			printf("Opening file %s...\n", zxmusbb+6);
			zxmusbf = fopen((const char*)(zxmusbb+6),"rb");
			if (zxmusbf) {
			  fstat(fileno(zxmusbf),&buf);
			  total = buf.st_size;
			} else {
			  total = -1;
			}
			n1 = (total & 0x000000ff);
			n2 = (total & 0x0000ff00) >> 8;
			n3 = (total & 0x00ff0000) >> 16;
			n4 = (total & 0xff000000) >> 24;
			zxmusbb[i++]=0xff; zxmusbb[i++]=0x00;
			zxmusbb[i++]=n1; zxmusbb[i++]=n2;
			zxmusbb[i++]=n3; zxmusbb[i++]=n4;
			zxmusbrl = 6;
			if (total<0) {
				stage = 0;
				return;
			}
			parts = (n3>0) || (n4>0);
	       case 2: if ((total>ZXMUSBCS) && parts) {
				n = fread(zxmusbb+zxmusbrl+6, 1, ZXMUSBCS, zxmusbf);
				total -= ZXMUSBCS;
			} else {
				n = fread(zxmusbb+zxmusbrl+6, 1, total, zxmusbf);
				fclose(zxmusbf);
				total = 0;
			}
			n1 = (n & 0x000000ff);
			n2 = (n & 0x0000ff00) >> 8;
			zxmusbb[i++]=0xff; zxmusbb[i++]=0x00;
			zxmusbb[i++]=n1; zxmusbb[i++]=n2;
			zxmusbb[i++]=0; zxmusbb[i++]=0;
			zxmusbrl += 6 + n;
			if (stage==1) stage++;
			if (total==0) stage = 0;
			break;
		default:;
	}
}

void save_usb(int stage)
{
	switch (stage) {
		case 1: zxmusbb[18]=0;
			printf("Opening file %s...\n", zxmusbb+6);
			zxmusbf = fopen((const char*)(zxmusbb+6), "wb");
			break;
		case 2: fwrite(zxmusbb+zxmusbwf,zxmusbwl-zxmusbwf,1,zxmusbf);
			fflush(zxmusbf);
			break;
		case 3: fclose(zxmusbf);
			break;
	}
}

BYTE read_usb()
{
	BYTE b;

	zxmusbs = 0;

	b = zxmusbb[zxmusbr];

#ifdef DEBUGUSB
        fprintf(stdout,"RUSB %d,%d,%d %02x at %04X\n", zxmusbs, zxmusbr, zxmusbrl, b, z80.pc.w);
#endif

	zxmusbr++;

	if (zxmusbr>=0x10000) {
		fprintf(stdout,"Error reading from USB buffer\n");
		zxmusbr = 0;
	} else if (zxmusbr==zxmusbrl) {
#ifdef DEBUGUSB
		fprintf(stdout,"All bytes read from USB buffer\n");
#endif
		load_usb(0);
	}

        return b;
}

void write_usb(int Data)
{
	int n;

#ifdef DEBUGUSB
	fprintf(stdout,"WUSB %d,%d,%d %02x at %04X\n", zxmusbs, zxmusbw, zxmusbwl, Data, z80.pc.w);
#endif

	zxmusbr = 0;

	switch(zxmusbs) {
		case  0: if (Data!=0xa5) {
			 	 fprintf(stdout,"USB I/O out of sync\n");
			 } else {
			         zxmusbs++;
			 }
			 break;
		case  1: zxmusbs++;
			 zxmusbc = Data;
#ifdef DEBUGUSB
			 fprintf(stdout,"USB command: %02x\n", zxmusbc);
#endif
			 zxmusbb[0]=0xff; zxmusbb[1]=0x00;
			 switch(zxmusbc) {
				 case 0x0f: /* version */
					    zxmusbb[2]=1; zxmusbb[3]=2; zxmusbb[4]=3; zxmusbb[5]=4;
					    zxmusbrl = 6+64;
					    zxmusbs = 0;
					    break;
				 case 0x11: /* filename - load */
					    zxmusbrl = 6;
					    break;
				 case 0x18: /* filename - save */
#ifdef DEBUGUSB
					    printf("Saving 18a %s\n", zxmusbb+6);
#endif
					    zxmusbrl = 12;
					    break;
				 case 0x19: /* save */
#ifdef DEBUGUSB
					    printf("Saving 19a %s\n", zxmusbb+6);
#endif
					    zxmusbwl += 2;
					    zxmusbs++;
					    break;
				 default  : ;
			 }
			 break;
		case  2: zxmusbs++;
			 switch(zxmusbc) {
				 case 0x11: load_usb(1);
				 case 0x18: /* filename */
					    zxmusbw = 6;
					    zxmusbwl = 6 + Data;
				 default  : ;
			 }
			 break;
		case  3: zxmusbb[zxmusbw++] = Data;
			 if (zxmusbw==zxmusbwl) switch(zxmusbc) {
				 case 0x11: load_usb(0);
					    break;
				 case 0x18: save_usb(1);
					    break;
			 	 case 0x19: n = (zxmusbb[zxmusbw-1] << 8) | zxmusbb[zxmusbw-2];
#ifdef DEBUGUSB
					    printf("Saving 19b %s %d\n", zxmusbb+6, n);
#endif
					    if (n==0) {
						    save_usb(3);
						    zxmusbs = 0;
					    } else {
						    zxmusbwf = zxmusbw;
						    zxmusbwl += n;
						    zxmusbs++;
					    }
					    break;
				 default  : ;
			 }
			 break;
		case  4: zxmusbb[zxmusbw++] = Data;
			 if (zxmusbw==zxmusbwl) switch(zxmusbc) {
			 	 case 0x19: save_usb(2);
#ifdef DEBUGUSB
					    printf("Saving 19c %s %d %d\n", zxmusbb+6, zxmusbwf, zxmusbwl);
#endif
					    zxmusbw = zxmusbwf-2;
					    zxmusbwl = zxmusbw;
					    break;
				 default  : fprintf(stdout,"USB I/O out of sync\n");
			 }
	}
}

#endif

void zx81_writeport(int Address, int Data, int *tstates)
{
#ifdef ZXMORE
        int speed;
#endif
        if (Address==0x7fef && zx81.Chroma81) {
                chromamode = Data&0x30; // TODO should be 0x20
                zx81.colour = chromamode ? COLOURCHROMA : COLOURDISABLED;
		if (chromamode) {
			zx81.chromamode = Data | 0x20;
			printf("Selecting Chroma 81 mode 0x%x...\n",zx81.chromamode);
		} else {
			printf("Switching off Chroma 81...\n");
			zx81.chromamode = 0;
			ink=0; paper=border=7;
			refresh_screen = 1;
		}
                return; 
        }

        switch(Address&255)
        {
        case 0x01:
                configbyte=Data;
                break;

#ifdef ZXPAND
        case 0x07:
                ZXpand_IO_Write(Address>>8, Data);
                break;
#endif

        case 0x0f:
#ifdef SNDPRC
		if (SelectAYReg >= 15) {
			if (Data<64) sp_start(Data);
			return;
		}
#endif
                if (zx81.aytype==AY_TYPE_ZONX)
                        sound_ay_write(SelectAYReg, Data, 0);
                break;
        case 0x1f:
#ifndef ZXMORE
        case 0xf7:
#endif
#ifdef SNDPRC
		if (SelectAYReg >= 15) {
			if (Data<64) sp_start(Data);
			return;
		}
#endif
                if (zx81.aytype==AY_TYPE_ZONX)
                        sound_ay_write(SelectAYReg, Data, 1);
                break;

        case 0xbf:
        case 0xcf:
        case 0xdf:
                if (zx81.aytype==AY_TYPE_ACE) sound_ay_write(SelectAYReg, Data, 0);
                if (zx81.aytype==AY_TYPE_ZONX) SelectAYReg=Data&15;
                break;

#ifdef APU
        case 0x67:
        case 0xe7:
		am_write_io8(Address&255,Data);
                break;
#endif

		/*
        case 0x3f:
                if (zx81.aytype==AY_TYPE_FULLER)
                        SelectAYReg=Data&15;
        case 0x5f:
                if (zx81.aytype==AY_TYPE_FULLER)
                        sound_ay_write(SelectAYReg, Data);
                break;
        case 0x73:
                if (zx81.ts2050) d8251writeDATA(Data);
                break;
        case 0x77:
                if (zx81.ts2050) d8251writeCTRL(Data);
                break;
        case 0xc7:
                d8255_write(D8255PRTA,Data);
                break;

        case 0xcf:
                d8255_write(D8255PRTB,Data);
                break;

        case 0xd7:
                d8255_write(D8255PRTC,Data);
                break;
		*/

        case 0xdd:
                if (zx81.aytype==AY_TYPE_ACE) SelectAYReg=Data;
                break;

        case 0xfb:
	        Data = printer_inout(1,Data);
		return;
                break;

        case 0xfd:
                if (zx81.machine==MACHINEZX80) break;
                LastInstruction = LASTINSTOUTFD;
                break;
        case 0xfe:
                if (zx81.machine==MACHINEZX80) break;
                LastInstruction = LASTINSTOUTFE;
                break;

	case WIZ_MODE:
	case WIZ_ADRH:
	case WIZ_ADRL:
	case WIZ_DATA: if (sdl_emulator.networking) w_write(Address&255,Data); break;

#ifdef ZXMORE
	case 0xf5:
#ifdef ZXMROML
		   break;
#endif
		   zxmroml = Data;
	           speed = (zxmroml & 0x10) ? 2 : 1;
		   zxmoff = (zxmroml & 0x80) ? 1 : 0;
		   if (zxmoff) zxmnmi = 0;
		   if (speed != zxmspeed) {
		     zxmspeed = speed;
#if 0
		     if (zxmspeed == 1) fprintf(stderr,"Setting ZXmore to normal speed %d %d\n", RasterX, RasterY);
		     else fprintf(stderr,"Setting ZXmore to power mode %d %d\n", RasterX, RasterY);
#endif
		   }
		   //zx81.machine = (zxmroml&0x07)>0 ? MACHINEZX81 : MACHINEZX80;
		   //*sdl_emulator.model = (zxmroml&0x07)>0 ? MODEL_ZX81 : MODEL_ZX80;
		   //zx80 = zx81.machine==MACHINEZX81;
	           switch (zxmroml&0x07) {
		   	case 0: paper = 0x08 | 7; break;
		   	case 1: paper = 0x08 | 6; break;
		   	case 2: paper = 0x08 | 3; break;
		   	case 3: paper = 0x08 | 2; break;
		   	case 4: paper = 0x08 | 5; break;
		   	case 5: paper = 0x08 | 4; break;
		   	case 6: paper = 0x08 | 1; break;
		   	case 7: paper = 0x00 | 7; break;
	           }
		   refresh_screen = 1;
		   break;
	case 0xf6:
#ifdef ZXMRAML
		   break;
#endif
		   zxmraml = Data;
		   nmi_pending = 0;
		   zxmoffset = 0x80000 + ((zxmraml & 0xe0) << 11); // for LOAD/SAVE
		   zx81.colour = COLOURCHROMA;
	           switch ((zxmraml&0xe0) >> 5) {
		   	case 0: zx81.chromamode = chromamode = 0x28 | 7; break;
		   	case 1: zx81.chromamode = chromamode = 0x28 | 6; break;
		   	case 2: zx81.chromamode = chromamode = 0x28 | 3; break;
		   	case 3: zx81.chromamode = chromamode = 0x28 | 2; break;
		   	case 4: zx81.chromamode = chromamode = 0x28 | 5; break;
		   	case 5: zx81.chromamode = chromamode = 0x28 | 4; break;
		   	case 6: zx81.chromamode = chromamode = 0x28 | 1; break;
		   	case 7: zx81.chromamode = chromamode = 0x20 | 7; break;
	           }
		   refresh_screen = 1;
		   break;
	case 0xf7: write_usb(Data);
		   break;
#endif

        case 0xff: // default out handled below
	        break;

        default:
		//		printf("Unhandled port write: %d\n", Address);
                break;
        }

        if (!LastInstruction) LastInstruction=LASTINSTOUTFF;
}

#ifdef VDRIVE
#define VDRDBG 0
#define VDRVRB 0

// TODO: CR has been added to the beginning of the DIR response;
// should that be done for other commands as well?

typedef unsigned char BYTE;

// defaults (p.16)
static int vd_mmode = 1;  // 1=ECS, 0=SCS
static int vd_vbinary = 1; // 1=binary, 0=ASCII
// further global variables
static int vd_error = 0;
static FILE *vd_fp = NULL;
static int vd_nbytes = 0;

void vdrive_failed(char *buf)
{
	if (vd_mmode)
		strcpy(buf,"Command Failed\r");
	else
		strcpy(buf,"CF\r");
	vd_error = 1;
}

void vdrive_open(char *buf)
{
	if (vd_mmode)
		strcpy(buf,"File Open\r");
	else
		strcpy(buf,"FO\r");
	vd_error = 2;
}

unsigned int vdrive_number(char *buf)
{
	unsigned int n;
	union {
		unsigned int n;
		unsigned char b[4];
	} u;
	

	if (vd_vbinary) {
		u.b[0] = buf[3];
		u.b[1] = buf[2];
		u.b[2] = buf[1];
		u.b[3] = buf[0];
		n = u.n;
	} else {
		n = sscanf(buf, "%i", &n);
		if (n != 1) vdrive_failed(buf);
	}

	return n;
}

int vdrive_dir(char *buf, char *p)
{
	DIR *d;
	struct dirent *dir;
	struct stat sbuf;
	int n;
	union {
		unsigned int n;
		unsigned char b[4];
	} u;

	n = 0;
	
	if (p) {

		if (!stat(p, &sbuf)) {
			u.n = sbuf.st_size;
			if (vd_vbinary) {
				n = strlen(p);
				sprintf(buf,"\r%s %c%c%c%c\r", p, u.b[0], u.b[1], u.b[2], u.b[3]);
				n += 6;
			} else {
				sprintf(buf,"\r%s $%02x $%02x $%02x $%02x\r", p, u.b[0], u.b[1], u.b[2], u.b[3]);
			}
		} else {
			vdrive_failed(buf);
		}

	} else {

	d = opendir(".");
	if (d) {
	    strcpy(buf,"\r");
	    while ((dir = readdir(d)) != NULL) {
		strcat(buf,dir->d_name);
	        strcat(buf," \r");
	    }
	    closedir(d);
	}

	}

	return n;
}

int vdrive_fs(char *buf)
{
	int n;
	union {
		unsigned int n;
		unsigned char b[4];
	} u;

	n = 0;
	
	u.n = 0; // TODO
	
	if (vd_vbinary) {
		sprintf(buf,"%c%c%c%c\r", u.b[0], u.b[1], u.b[2], u.b[3]);
		n = 4;
	} else {
		sprintf(buf,"$%02x $%02x $%02x $%02x\r", u.b[0], u.b[1], u.b[2], u.b[3]);
	}

	return n;
}

int vdrive(char *buf, int bufptr)
{
	unsigned char cmd;
	char *s=NULL, *p=NULL, *pp=NULL;
	char pwd[128];
	int prompt=1;
	int n;

#if (VDRDBG>=1)
	printf("vdrive buf: %s\n", buf);
#endif
	vd_error = 0;

	n = 0;

	if (vd_nbytes < 0) {
#if VDRVRB
		printf("WRF %d\n", bufptr);
#endif
		vd_nbytes = 0;
		fwrite(buf, bufptr, 1, vd_fp); // TODO: error check
		strcpy(buf, ""); // will be handled shortly
	}

	if (!strcmp(buf,"")) {
		if (vd_mmode==1)
			strcpy(buf,"EMPTY");
		else
			buf[0] = 0;
	}
	
	s = strtok(buf, " ");

	if (vd_mmode==1) { // with ECS, translate commands to SCS

		if (!s) cmd = 0x00;
		else if (!strcasecmp(s,"EMPTY")) cmd = 0x00;
		else if (!strcasecmp(s,"SCS")) cmd = 0x10;
		else if (!strcasecmp(s,"ECS")) cmd = 0x11;
		else if (!strcasecmp(s,"IPA")) cmd = 0x90;
		else if (!strcasecmp(s,"IPH")) cmd = 0x91;
		else if (!strcasecmp(s,"FWV")) cmd = 0x13;
		else if (!strcmp(s,"E")) cmd = 0x45;
		else if (!strcmp(s,"e")) cmd = 0x65;
		else if (!strcasecmp(s,"DIR")) cmd = 0x01;
		else if (!strcasecmp(s,"CD"))  cmd = 0x02;
		else if (!strcasecmp(s,"RD"))  cmd = 0x04;
		else if (!strcasecmp(s,"DLD")) cmd = 0x05;
		else if (!strcasecmp(s,"MKD")) cmd = 0x06;
		else if (!strcasecmp(s,"DLF")) cmd = 0x07;
		else if (!strcasecmp(s,"WRF")) cmd = 0x08;
		else if (!strcasecmp(s,"OPW")) cmd = 0x09;
		else if (!strcasecmp(s,"CLF")) cmd = 0x0a;
		else if (!strcasecmp(s,"RDF")) cmd = 0x0b;
		else if (!strcasecmp(s,"REN")) cmd = 0x0c;
		else if (!strcasecmp(s,"OPR")) cmd = 0x0e;
		else if (!strcasecmp(s,"FS"))  cmd = 0x12;
		else if (!strcasecmp(s,"FSE")) cmd = 0x93;
		else if (!strcasecmp(s,"IDD")) cmd = 0x0f;
		else if (!strcasecmp(s,"IDDE"))cmd = 0x94;
		else if (!strcasecmp(s,"SEK")) cmd = 0x28;

		else cmd = -1;

	} else {

		if (!strcasecmp(buf,"SCS")) cmd = 0x10;
		else if (!strcasecmp(buf,"ECS")) cmd = 0x11;
		else cmd = buf[0];
	}

	if (cmd==0x08 || cmd==0x0b || cmd==0x28) { // WRF or RDF or SEK
		if (vd_mmode)
			vd_nbytes = vdrive_number(buf+4);
		else
			vd_nbytes = vdrive_number(buf+2);
	}
		
	p = strtok(NULL, " ");
	pp = strtok(NULL, " ");

	if (cmd==0x08) return 0; // WRF is handled in a next call
	
	strcpy(buf, "");

	switch (cmd)
	{
	case 0x00: // EMPTY
		break;
	case 0x10: // SCS
		vd_mmode = 0;
		break;
	case 0x11: // ECS
		vd_mmode = 1;
		break;
	case 0x90: // IPA
		vd_vbinary = 0;
		break;
	case 0x91: // IPH
		vd_vbinary = 1;
		break;
	case 0x13: // FWV
		strcpy(buf,"\rsz81\r");
		break;
	case 0x45: // E (sync without prompt)
#if VDRVRB
		printf("Sync E\n");
#endif
		strcpy(buf,"E\r");
		prompt = 0;
		break;
	case 0x65: // e (sync without prompt)
#if VDRVRB
		printf("Sync e\n");
#endif
		strcpy(buf,"e\r");
		prompt = 0;
		break;
	case 0x01: // DIR
#if VDRVRB
		printf("DIR %s\n",p);
#endif
		n = vdrive_dir(buf,p);
		break;
	case 0x02: // CD
		if (chdir(p)) vdrive_failed(buf);
		break;
	case 0x04: // RD
		if (!vd_fp) {
#if VDRVRB
		        printf("RD %s\n", p);
#endif
			vd_fp = fopen(p, "rb");
			if (vd_fp) {
				n = fread(buf, 1, 0x10000, vd_fp);
				fclose(vd_fp);
			} else {
				vdrive_failed(buf);
			}
			vd_fp = NULL;
		} else {
		       	vdrive_failed(buf);
		}
		break;
	case 0x05: // DLD
		if (rmdir(p)) vdrive_failed(buf);
		break;
	case 0x06: // MKD
		if (mkdir(p,0755)) vdrive_failed(buf);
		break;
	case 0x07: // DLF
		if (unlink(p)) vdrive_failed(buf);
		break;
	case 0x09: // OPW
#if VDRVRB
		printf("OPW %s\n", p);
#endif
		if (!vd_fp) {
			if (!(vd_fp = fopen(p,"wb"))) vdrive_failed(buf);
		} else {
			vdrive_open(buf);
		}
		break;
	case 0x0a: // CLF
#if VDRVRB
		printf("CLF %s\n", p);
#endif
		if (vd_fp) {
			fclose(vd_fp);
			vd_fp = NULL;
		}
		break;
	case 0x0b: // RDF
#if VDRVRB
		printf("RDF %d\n", vd_nbytes);
#endif
		if (vd_fp) {
			n = fread(buf, 1, vd_nbytes, vd_fp);
			if (n != vd_nbytes) vdrive_failed(buf);
			vd_nbytes = 0;
		} else {
			vdrive_failed(buf);
		}
		break;
	case 0x0c: // REN
		if (rename(p,pp)) vdrive_failed(buf);
		break;
	case 0x0e: // OPR
#if VDRVRB
		printf("OPR %s\n", p);
#endif
		if (!vd_fp) {
			if (!(vd_fp = fopen(p,"rb"))) vdrive_failed(buf);
		} else {
			vdrive_open(buf);
		}
		break;
	case 0x12: // FS
	case 0x93: // FSE TODO
		n = vdrive_fs(buf);		
		break;
	case 0x0f: // IDD
	case 0x94: // IDDE
		strcpy(buf,"Virtual Drive\r");
		break;
	case 0x28: // SEK
#if VDRVRB
		printf("SEK %d\n", vd_nbytes);
#endif
		if (vd_fp) {
			if (fseek(vd_fp,vd_nbytes,SEEK_SET)) vdrive_failed(buf);
		} else {
			vdrive_failed(buf);
		}
		vd_nbytes = 0;
		break;
	default:
		if (vd_mmode==1)
			strcpy(buf,"Bad Command>\r");
		else
			strcpy(buf,"BC>\r");
		vd_error = 3;
	}

	if (n == 0) n = strlen(buf);

	if (!prompt) return n;
	
#if (VDRDBG>=1)
	printf("vdrive cmd:%02x; parameter:%s; nbytes:%d n:%d\n", cmd, p, vd_nbytes, n);
#endif
	if (!vd_error) {
		if (vd_mmode) {
			getcwd(pwd,128);
			sprintf(buf+n,"%s>\r",pwd);
			n += strlen(pwd) + 2;
		} else {
			strcpy(buf+n, ">\r");
			n += 2;
		}
	} else {
		printf("vdrive error: %d\n",vd_error);
		if (vd_error==1) perror("vdrive error");
	}

	return n;
}

BYTE vdrive_io(BYTE b)
{
	static BYTE state;
	static BYTE rw='R';
	static BYTE n=0;
	static BYTE br, bw, bs;
	static char buf[0x10000];
	static int bufptr=-1;
	static int len=0;
	int binarg;

	b &= 0x0f;
	switch (b) {
		case 0x0c: state=bw=bs=n=0;
			   if (bufptr<0) {
				   strcpy(buf,"VDRIVE On-Line>\r");
				   bufptr = 0;
				   len = strlen(buf);
			   }
			   break;
		case 0x0f: if (state==0) {
				state++;
				return 0; // should be OK
			   } else if (state==1) {
				rw = 'R';
				if (len>=0) {
				    br = buf[bufptr];
#if (VDRDBG>=3)
				    printf("vdrive br: %d %d 0x%02x %c\n",bufptr,br,br,br);
#endif
				    bufptr++;
				    len--;
				}
				state++;
			   } else if (state==3) {
				bw = (bw << 1) | 0x01;
				bs = br & 0x80;
				br <<= 1;
				n++;
				if (n==8) state++;
			   } else {
				state++;
			   }
			   break;
		case 0x0e: if (state==0) {
				state++;
				return 0; // should be OK
			   } else if (state==1) {
				if (rw!='W') bufptr = 0;
				rw = 'W';
				state++;
			   } else if (state==3) {
				bw <<= 1;
				bs = br & 0x80;
				br <<= 1;
				n++;
				if (n==8) state++;
			   } else {
				state++;
			   }
			   break;
		default: return 0xff;
	}

	if (state==4) {
#if (VDRDBG>=2)
		printf("vdrive %d %c %d %d %d %c %c %d %d\n", state, rw, n, bufptr, len, br, ((bw>=0x20&&bw<0x80)?bw:0x20), bw, vd_nbytes);
#endif
		if (rw=='W') {
			binarg = (!strncasecmp(buf,"RDF ",4) || !strncasecmp(buf,"SEK ",4) || !strncasecmp(buf,"WRF ",4));
			if (!vd_nbytes && (bw == '\r') && (!binarg || (binarg && (bufptr==8)))) {
				buf[bufptr] = '\0';
				len = vdrive(buf, bufptr);
				bufptr = 0;
			} else {
#if (VDRDBG>=3)
				printf("vdrive bw: %d %d 0x%02x %c\n",bufptr,bw,bw,bw);
#endif
//				if (vd_nbytes || bw<128 || vd_mmode==0) buf[bufptr++] = bw;
				buf[bufptr++] = bw;
				if (vd_nbytes && (vd_nbytes==bufptr)) {
					vd_nbytes = -1;
					len = vdrive(buf, bufptr);
				        bufptr = 0;
				}
			}
		}
	}

	if (state==3 || state==4) {
		return bs;
	} else if (state==5) {
		if (rw=='W') {
		    bs = 0;
		} else {
		    bs = (len>=0 ? 0 : 0xff);
		}
#if (VDRDBG>=2)
		printf("vdrive status: %d\n",bs);
#endif
		return bs;
	}

	return 0; // OK
}
#endif

BYTE zx81_readport(int Address, int *tstates)
{
        static int beeper;
	int ts=0;               /* additional cycles*256 */
	static int tapemask=0;
	int data=0;             /* = 0x80 if no tape noise (?) */
	int h, l;

	tapemask++;
	data |= (tapemask & 0x0100) ? 0x80 : 0;

        if (zx81.NTSC) data|=64;

	h = Address >> 8;
	l = Address & 0xff;

        if (Address==0x7fef) {
                return zx81.Chroma81 ? 0 : 255;
        }

#ifdef ZXPAND
	if (l==0x07) return ZXpand_IO_Read(Address>>8);
	else if (l==0x17) return ZXpand_IO_ReadStatus();
#endif
     
#ifdef ZXMORE
	if (l==0xf7) return read_usb();
	if (l==0xf4) { zxmnmicounter = 52; }
#endif

#ifdef APU
        if (l==0x67 || l==0xe7) {
		return am_read_io8(l);
	}
#endif

#ifdef ZXNU
	if (l==0x1e) {
			printf("%c", z80.bc.b.h & 0x7f);
			if (!(z80.bc.b.h & 0x80)) printf("\n");
			return 255;
		     }
	if (l==0x5e) {
			printf("%c", z80.bc.b.h & 0x7f);
			return 255;
		     }
#endif
		
	if (!(Address&1)) {
                LastInstruction=LASTINSTINFE;
		setborder=1;

		if (l==0x7e) return zx81.NTSC ? 1 : 0; // for Lambda

		switch(h)
		{
		case 0xfe:        return(ts|(keyports[0]^data));
		case 0xfd:        return(ts|(keyports[1]^data));
		case 0xfb:        return(ts|(keyports[2]^data));
		case 0xf7:        return(ts|(keyports[3]^data));
		case 0xef:        return(ts|(keyports[4]^data));
		case 0xdf:        return(ts|(keyports[5]^data));
		case 0xbf:        return(ts|(keyports[6]^data));
		case 0x7f:        return(ts|(keyports[7]^data));
		default:
			{
				int i,mask,retval=0xff;
				        /* some games (e.g. ZX Galaxians) do smart-arse things
					 * like zero more than one bit. What we have to do to
					 * support this is AND together any for which the corresponding
					 * bit is zero.
					 */
				for(i=0,mask=1;i<8;i++,mask<<=1)
					if(!(h&mask))
						retval&=keyports[i];
				return(ts|(retval^data));
			}
		}
	}

        switch(l)
                {
                case 0x01:
                {
                        char *config;
                        config=(char *)(&zx81);
                        return(config[configbyte]);
                }

#ifdef ZXNU
                case 0x5f:
			zxnu_latch = z80.bc.b.h;
			loadrombank((zxnu_latch&0xc0) << 7);
#else
#ifdef VDRIVE
		case 0x7f:
#endif
#endif
#ifdef VDRIVE
			return vdrive_io(z80.bc.b.h);
#endif
                        if (zx81.truehires==HIRESMEMOTECH) {
				MemotechMode=(Address>>8);
				printf("Selecting MemotechMode %d...\n", MemotechMode);
			}
                        return(255);

			/*
                case 0x73:
                        if (zx81.ts2050) return(d8251readDATA());

                case 0x77:
                        if (zx81.ts2050) return(d8251readCTRL());

                case 0xdd:
                        if (zx81.aytype==AY_TYPE_ACE)
                                return(sound_ay_read(SelectAYReg));
			*/

                case 0xf5:
                        beeper = 1-beeper;
                        if ((zx81.machine==MACHINELAMBDA) && zx81.vsyncsound) sound_beeper(beeper);
                        return(255);
                case 0xfb:
			data = printer_inout(0,0);
                        return (BYTE) data;

#ifdef SNDPRC
		case 0xdf:
		        return sp_on ? 255 : 0;
#endif

		case WIZ_MODE:
		case WIZ_ADRH:
		case WIZ_ADRL:
		case WIZ_DATA: if (sdl_emulator.networking) return w_read(Address&255);

                default:
                        break;
                }
        return(255);
}

/* Normally, these sync checks are done by the TV :-) */

static inline void checkhsync(int tolchk)
{
	if ( ( !tolchk && sync_len >= HSYNC_MINLEN && sync_len <= HSYNC_MAXLEN && RasterX>=HSYNC_TOLERANCEMIN ) ||
	     (  tolchk &&                                                         RasterX>=HSYNC_TOLERANCEMAX ) )
	{
		RasterX = 0;
		RasterY++;
		dest += TVP;
	}
}

static inline void checkvsync(int tolchk)
{
	if ( ( !tolchk && sync_len >= VSYNC_MINLEN && RasterY>=VSYNC_TOLERANCEMIN ) ||
	     (  tolchk &&                             RasterY>=VSYNC_TOLERANCEMAX ) )
	{
		RasterY = 0;
		dest = 0;

		memcpy(scrnbmp,scrnbmp_new,sizeof(scrnbmp));
		if (zx81.colour!=COLOURDISABLED) memcpy(scrnbmpc,scrnbmpc_new,sizeof(scrnbmpc));
	}
}

static inline void checksync()
{
	if (!SYNC_signal) {
		if (psync==1) sync_len = 0;
		sync_len++;
		checkhsync(1);
		checkvsync(1);
	} else {
		if (!psync) {
			checkhsync(0);
			checkvsync(0);
		}
	}
	psync = SYNC_signal;
}

/* The rowcounter is a 7493; as long as both reset inputs are high, the counter is at zero
   and cannot count. Any out sets it free. */

void anyout()
{
	if (VSYNC_state) {
		if (zx81.machine==MACHINEZX80)
			VSYNC_state = 2; // will be reset by HSYNC circuitry
		else
		{
			VSYNC_state = 0;
			// A fairly arbitrary value selected after comparing with a "real" ZX81
      		if ((hsync_counter < 16) || (hsync_counter >= 178) )
      		{
        		rowcounter_hold = 1;
      		}
		}

		if ((zx81.machine!=MACHINELAMBDA) && zx81.vsyncsound) sound_beeper(0);
#ifdef VRCNTR
		/* due to differences in when the "207" counters give the /HSYNC, and OUT instruction delay */
		hsync_counter = 25;
#endif
	}
}

/* Rewritten zx81_do_scanlines() and AccurateDraw()  */

int zx81_do_scanlines(int tstotal)
{
        int ts;
	int i, ipixel, istate;
	int tswait;
#ifdef ZXMORE
	int phalted=0;
#endif
        do
        {

/* at this point, z80.pc points to the instruction to be executed;
   so if nmi or int is pending, the RST instruction with the right number of tstates
   is emulated */

		ts = 0;

                if (int_pending && !nmi_pending) {
#ifdef ZXMORE
			rowcounter = zxmrowcounter;
			zxmvideo++;
#endif
                        ts = z80_interrupt(0);
#ifndef VRCNTR
			hsync_counter = -2;             /* INT ACK after two tstates */
			hsync_pending = 1;              /* a HSYNC may be started */
#else
			if (zx81.machine==MACHINEZX80) {
				hsync_counter = -2;
				hsync_pending = 1;
		        }
#endif
                }

		if (nmi_pending) {
		  if (zxmnmicounter > 0) zxmnmicounter--;
		  if (zxmnmicounter == 0) {
		    if (!zxmoff) zxmnmi = 1;
		    ts = z80_nmi(0);
		  }
		}

		LastInstruction = LASTINSTNONE;
		if (!nmi_pending && !int_pending) {
		        ffetch = 1;
			z80.pc.w = PatchTest(z80.pc.w);
#ifdef ZXMORE
			phalted = z80.halted;
#endif
			ts = z80_do_opcode();
		}
		nmi_pending = int_pending = 0;

		tstates += ts;

/* check iff1 even though it is checked in z80_interrupt() */

                if (!((z80.r-1) & 64) && z80.iff1) {
			int_pending = 1;
                        paper = border;
		}

                switch(LastInstruction)
                {
                case LASTINSTOUTFD:
                        NMI_generator = nmi_pending = 0;
			anyout();
                        break;
                case LASTINSTOUTFE:
#ifdef ZXMORE
		        zxmvideo = 0;
#endif
		        if (zx81.machine!=MACHINEZX80) { NMI_generator=1; zxmnmicounter=0; }
			anyout();
                        break;
                case LASTINSTINFE:
                        if (!NMI_generator)
                        {
                                if (VSYNC_state==0) {
					VSYNC_state = 1;
					if ((zx81.machine!=MACHINELAMBDA) && zx81.vsyncsound) sound_beeper(1);
				}
                        }
                        break;
                case LASTINSTOUTFF:
			anyout();
                        if (zx81.machine==MACHINEZX80) hsync_pending=1;
                        break;
                default:
                        break;
                }

/* do what happened during the last instruction */

		for (istate=0, ipixel=0; istate<ts; istate++)
		{
			/* draw two pixels for this tstate */

			for (i=0; i<2; i++, ipixel++) {

				int colour, bit;

				bit=((shift_register^shift_reg_inv)&128);

				if (SYNC_signal) {
					colour = (bit ? ink:paper);
					if (zx81.colour==COLOURLAMBDA) switch (colour&0x07) {
							// see http://www.2kb.dk/pic/farvemodul_side1_s.jpg
						case 2  : colour = 12; break;
						case 3  : colour = 13; break;
						case 4  : colour = 10; break;
						case 5  : colour = 11; break;
						default : colour += 8;
						}
					colour <<= 4;
				} else {
					colour = border << 4;
				        bit = 1; // set to 0 for no SYNC effects
				}

				colour += (bit ? 1 : 0); // hack for B/W & Chroma

				shift_register<<=1;
				shift_reg_inv<<=1;

				Plot(colour);
			}

			hsync_counter++;
			if (hsync_counter >= machine.tperscanline*(zxmvideo?1:zxmspeed)) {
				hsync_counter = 0;
				if (zx81.machine!=MACHINEZX80) hsync_pending = 1;
			}

			// Start of HSYNC, and NMI if enabled

			if (hsync_pending==1 && hsync_counter>=16) {
				if (NMI_generator) {
					nmi_pending = 1;
#ifndef WAITMOD
					if (ts==4) tswait = 14+istate; else tswait = 14;
#else
					if (z80.halted) tswait = 14+istate; else tswait = 0;
#endif
					ts += tswait;
					tstates += ts;
				}

				HSYNC_state = 1;

				if (VSYNC_state || rowcounter_hold) {
#ifdef ZXMORE
					zxmrowcounter = 0;
#endif
					rowcounter = 0;
					rowcounter_hold = 0;
#ifndef ZXMORE
				} else {
					rowcounter++;
					rowcounter &= 7;
#endif
				}

#if 0
				if (!VSYNC_state) {
					zxmrowcounter++;
					zxmrowcounter &= 7;
					rowcounter = zxmrowcounter;
				}
#endif
				hsync_pending = 2;
			}

			// end of HSYNC

			if (hsync_pending==2 && hsync_counter>=32) {
				if (VSYNC_state==2) VSYNC_state = 0;
				HSYNC_state = 0;
				hsync_pending = 0;
			}

			// NOR the vertical and horizontal SYNC states to create the SYNC signal

			SYNC_signal = (VSYNC_state || HSYNC_state) ? 0 : 1;

			checksync();

		}

#ifdef ZXMORE
		if (z80.halted && !phalted) {
			if (zxmvideo>=2) {
				zxmrowcounter++;
				zxmrowcounter &= 7;
			}
			zxmvideo++;
		}
#endif

                tstotal -= ts;

		if (tstates >= tsmax) {
			if (ispeedup==-1) frame_pause();
			ispeedup++;
			if (ispeedup>=zx81.speedup) ispeedup=-1;
			frames++;
			tstates -= tsmax;
		}

	} while (tstotal>0 && !zx81_stop);

        return(tstotal);
}


/* (Modified) EightyOne code ends here */

void zx81_initialise(void)
{
	char filename[256];
	FILE *fp;

/* Just to avoid changing the variable name in the EightyOne code;
   note that memattr[] is not used (perhaps TODO) */

	memory = mem;

#ifdef ZXMORE
#ifdef ZXMROML
	zxmroml = ZXMROML;
#endif
#ifdef ZXMRAML
	zxmraml = ZXMRAML;
#endif
#endif

#ifdef ZXMORE
	sdl_emulator.ramsize = 48;
#endif
#ifdef ZXNU
	sdl_emulator.ramsize = 48;
#endif
#ifdef ZXPAND
	sdl_emulator.ramsize = 32;
#endif

/* Configuration variables used in EightyOne code */

	if (sdl_emulator.ramsize<=2) {
		sdl_emulator.chrgen = CHRGENSINCLAIR;
		sdl_emulator.wrx = HIRESWRX;
	}
	zx81.chrgen          = sdl_emulator.chrgen;
	zx81.Chroma81        = sdl_emulator.ramsize < 48 ? 0 : 1;
	zx81.dirtydisplay    = 0;
	zx81.enableqschrgen  = 0;
	zx81.extfont         = 0;
	zx81.colour          = COLOURDISABLED;
	zx81.m1not           = (sdl_emulator.m1not && (sdl_emulator.ramsize>=32)) ? 49152 : 32768;
	zx81.machine         = zx80 ? MACHINEZX80 : MACHINEZX81;
	zx81.truehires       = sdl_emulator.wrx; // HIRESWRX or HIRESDISABLED
	if (zx81.chrgen == CHRGENCHR16)
	{
		zx81.maxireg         = 0x40;
	} else {
		zx81.maxireg         = (zx81.truehires==HIRESWRX) ? 0x20 : 0x40;
	}
	zx81.NTSC            = 0;
	zx81.protectROM      = 1;
	zx81.RAMTOP          = (sdl_emulator.ramsize < 16) ? (0x4000+sdl_emulator.ramsize*0x400-1) : ((sdl_emulator.ramsize < 48) ? ((sdl_emulator.ramsize == 32) ? 0xbfff : 0x7fff) : 0xffff);
#ifdef ZXNU
	zx81.ROMTOP          = 0x1fff;
#else
	zx81.ROMTOP          = zx80 ? sdl_zx80rom.state-1 : sdl_zx81rom.state-1;
#endif
	zx81.speedup         = 0;
	zx81.shadowROM       = 0;
	zx81.simpleghost     = 0;
	zx81.single_step     = 0;
	zx81.ts2050          = 0;
	zx81.vsyncsound      = sdl_sound.device==DEVICE_VSYNC ? 1 : 0;
	zx81.rsz81mem       = rwsz81mem==1 ? 1 : 0;
	zx81.wsz81mem       = rwsz81mem==2 ? 1 : 0;

        machine.contendmem   = zx81_contend;
        machine.contendio    = zx81_contend;
	machine.opcode_fetch = zx81_opcode_fetch;
        machine.writebyte    = zx81_writebyte;
        machine.readbyte     = zx81_readbyte;
        machine.readport     = zx81_readport;
        machine.writeport    = zx81_writeport;

	machine.tperscanline = 207;
	machine.scanlines    = 310; /* PokeMon */
	if (zx81.NTSC) machine.scanlines -= (55-31)*2; /* difference in MARGINs */

	if (zx81.machine==MACHINEZX80)
		machine.tperframe    = machine.tperscanline * machine.scanlines - 3;
	else
		machine.tperframe    = machine.tperscanline * machine.scanlines - 7;

	tsmax = 65000; //machine.tperframe;

/* Initialise Accurate Drawing */

	RasterX = 0;
	RasterY = myrandom(VSYNC_TOLERANCEMAX);
	dest = 0;
	psync = 1;
	sync_len = 0;
	TVP = ZX_VID_X_WIDTH;

/* ULA */

        NMI_generator=0;
	int_pending=0;
	hsync_pending=0;
        VSYNC_state=HSYNC_state=0;
        MemotechMode=0;

#ifdef ZXPAND
	strcpy(filename, PACKAGE_DATA_DIR);
	strcatdelimiter(filename);
	strcat(filename, "zx81font.rom");
        if ((fp = fopen(filename, "rb")) != NULL) {
		fread(font, 1, 512, fp);
		fclose(fp);
	}
	zx81.extfont = 1;
#endif
	
/* Here are some alternatives to try (no fopen fail message!) */

	//zx81.truehires = HIRESMEMOTECH;
	if (zx81.truehires == HIRESMEMOTECH) {
		strcpy(filename, PACKAGE_DATA_DIR);
		strcatdelimiter(filename);
		strcat(filename, "memohrg.rom");
                if ((fp = fopen(filename, "rb")) != NULL) {
			fread(memory+8192, 1, 2 * 1024, fp);
			fclose(fp);
		}
	}

        //zx81.truehires = HIRESG007; // use 24K RAM setting; rowcounter is one line off (as in EightyOne)
	if (zx81.truehires == HIRESG007) {
		strcpy(filename, PACKAGE_DATA_DIR);
		strcatdelimiter(filename);
		strcat(filename, "g007hrg.rom");
                if ((fp = fopen(filename, "rb")) != NULL) {
			fread(memory+10240, 1, 2 * 1024, fp);
			fclose(fp);
		}
	}

	//zx81.machine = MACHINELAMBDA; // use 56K RAM setting; border is TODO
	if (zx81.machine==MACHINELAMBDA) {
		strcpy(filename, PACKAGE_DATA_DIR);
		strcatdelimiter(filename);
		strcat(filename, "lambda.rom");
                if ((fp = fopen(filename, "rb")) != NULL) {
			fread(sdl_zx81rom.data, 1, 8 * 1024, fp);
			fclose(fp);
		}
		strcpy(filename, PACKAGE_DATA_DIR);
		strcatdelimiter(filename);
		strcat(filename, "lmbfnt.rom");
                if ((fp = fopen(filename, "rb")) != NULL) {
			fread(font, 1, 512, fp);
			fclose(fp);
		}
		zx81.Chroma81 = 0;
		zx81.extfont = 1;
		zx81.colour = COLOURLAMBDA;
		chromamode = 1;
        }

        if (zx81.machine==MACHINELAMBDA) { ink=paper=border=0; }
        else { ink=0; paper=border=7; }

        z80_init();
        z80_reset();
}

void mainloop()
{
	int j, borrow=0;

#ifdef SZ81	/* Added by Thunor */
if(sdl_emulator.autoload)
  {
  sdl_emulator.autoload=0;
#ifdef ZXMORE
#endif
  /* This could be an initial autoload or a later forcedload */
  if(!sdl_load_file(0,LOAD_FILE_METHOD_DETECT))
    /* wait for a real frame, to avoid an annoying frame `jump'. */
	  ;	  // perhaps TODO    framewait=1;
  }
#endif

  tstates = 0;
  ispeedup = -1;

  while (1) {

  zx81.aytype = sound_ay_type;

  if (sdl_emulator.speed <= 20)
	  j = 20/sdl_emulator.speed - 1;
  else
	  j = 0;

  if (zx81.speedup != j) {
	  zx81.speedup = j;
	  printf("Setting speedup to %d...\n", j);
  }

  j = zx81.single_step ? 1 : ( machine.tperframe + borrow );

  if ( j != 1 )
  {
	  j += ( zx81.speedup * machine.tperframe ); // EO; was / machine.tperscanline;
  }

  borrow = zx81_do_scanlines(j);

  borrow = 0;

  /* this isn't used for any sort of Z80 interrupts,
   * purely for the emulator's UI.
   */
  if(interrupted)
    {
    if(interrupted==1)
      {
      do_interrupt();	/* also zeroes it */
      }
#ifdef SZ81	/* Added by Thunor */
    /* I've added these new interrupt types to support a thorough
     * emulator reset and to do a proper exit i.e. back to main */
    else if(interrupted==INTERRUPT_EMULATOR_RESET || 
            interrupted==INTERRUPT_EMULATOR_EXIT)
      {
      return;
      }
#else
    else	/* must be 2 */
      {
      /* a kludge to let us do a reset */
      }
#endif
    }

  }
}
