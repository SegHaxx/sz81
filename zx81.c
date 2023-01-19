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

#define LASTINSTNONE  0
#define LASTINSTINFE  1
#define LASTINSTOUTFE 2
#define LASTINSTOUTFD 3
#define LASTINSTOUTFF 4

// #define WAITMOD
// #define VRCNTR

ZX81 zx81;
BYTE *memory;
extern BYTE *sz81mem;
extern int rwsz81mem;

/* odd place to have this, but the display does work in an odd way :-) */
unsigned char scrnbmp_new[ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT/8];  /* written */
unsigned char scrnbmp[ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT/8];      /* displayed */
unsigned char scrnbmp_old[ZX_VID_FULLWIDTH*ZX_VID_FULLHEIGHT/8];  /* checked against for diffs */

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
int zx81_stop=0;
int hsync_counter=0;
int ispeedup;
int doda;

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
        printf(" %6ld %04X %04X %04X %04X %04X %04x %10s:  %s\n",
	       tstates, dAddr,
	       z80.af.w, z80.bc.w, z80.de.w, z80.hl.w, z80.sp.w,
	       d->hexDisBuf, d->disBuf);
                
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

/* zx97 code has been removed */

/* In sz81, the extended instructions were called within edops.c;
   to keep the current Philip Kendall's z80_ed.c clean, the instructions are
   tested here in line with EightyOne */

int PatchTest(int pc)
{
        int byte, edbyte;
        byte=memory[pc];

	if (byte==0xed) {
		edbyte = memory[pc+1];
		switch (edbyte) {
		case 0xfa: if (pc==0x0692 || pc==0x0693) {
				kcomm(z80.hl.w);
				return(pc+2);
			    }
			    break;
		case 0xfb: if (pc==0x06c5 || pc==0x06c6) {
				z80.af.b.h=lcomm(z80.af.b.h,z80.hl.w);
				return(pc+2);
			    }
			    break;
	        case 0xfc: if (pc==0x348 && zx81.machine==MACHINEZX81) {
			     if (z80.hl.w < 0x8000)
			       sdl_load_file(z80.hl.w,LOAD_FILE_METHOD_NAMEDLOAD);
		             else
			       sdl_load_file(z80.hl.w,LOAD_FILE_METHOD_SELECTLOAD);
		             return(z80.pc.w);
	                   }
	                   if (zx81.machine==MACHINEZX80 && pc==0x206) {
		               sdl_load_file(z80.hl.w,LOAD_FILE_METHOD_SELECTLOAD);
		               return(pc+2);
	                   }
			   if (pc==0x19f0 && zx81.machine==MACHINELAMBDA) {
			     if (z80.hl.w < 0x8000)
			       sdl_load_file(z80.hl.w,LOAD_FILE_METHOD_NAMEDLOAD);
		             else
			       sdl_load_file(z80.hl.w,LOAD_FILE_METHOD_SELECTLOAD);
		             return(z80.pc.w);
	                   }
			   break;
		case 0xfd: if (zx81.machine==MACHINEZX81 && pc==0x02fc) {
				sdl_save_file(z80.hl.w,SAVE_FILE_METHOD_NAMEDSAVE);
				return(pc+2);
			   }
			   if (zx81.machine==MACHINEZX80 && pc==0x01b6) {
			        sdl_save_file(z80.hl.w,SAVE_FILE_METHOD_UNNAMEDSAVE);
		                return(pc+2);
		           }
			   if (zx81.machine==MACHINELAMBDA && pc==0x0d0d) {
			        sdl_save_file(z80.hl.w,SAVE_FILE_METHOD_UNNAMEDSAVE);
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
                memory[Address]=Data;
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
		if (zx81.RAMTOP==0xbfff) Address = memory[Address&0x7fff];
		else Address = (Address&(zx81.RAMTOP));
	}

        if (Address<=zx81.ROMTOP && zx81.protectROM)
        {
                if ((zx81.truehires==HIRESMEMOTECH) && (Address<1024))
                                memhrg[Address]=Data;
                return;
        }

        if (Address>8191 && Address<16384 && zx81.shadowROM && zx81.protectROM) return;
        if (Address<10240 && zx81.truehires==HIRESMEMOTECH) return;
        if (Address>=10240 && Address<12288 && zx81.truehires==HIRESG007) return;

        if (zx81.wsz81mem && Address>=0x4000 && Address<0x8000) sz81mem[Address-0x4000]=Data;
        memory[Address]=Data;
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
			data=memory[Address];
			printf("Read Lambda colour RAM (0x%04X:0x%02X)\n", Address, data);
		} else {
			Address = (Address&1023)+8192;
			data=memory[Address];
		}
                return(data);
        }

        if (zx81.rsz81mem && Address>=0xc000) return sz81mem[Address-0xc000];

        if (Address<=zx81.RAMTOP) data=memory[Address];
	//        else data=memory[(Address&(zx81.RAMTOP-16384))+16384];
        else if (zx81.RAMTOP==0xbfff) data=memory[Address&0x7fff];
	else data=memory[Address&zx81.RAMTOP];

        if ((Address<1024 && (zx81.truehires==HIRESMEMOTECH)) && (z80.i&1))
                        data=memhrg[Address];

        if ((Address>=0x0c00 && Address<=0x0cff) && (zx81.truehires==HIRESG007))
                data=memory[Address+8192];

        if ((Address<256 || (Address>=512 && Address<768))
                && (z80.i&1) && (zx81.truehires==HIRESG007))
                        data=memory[Address+8192];

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
        int inv;
        int opcode, bit6, update=0;
        BYTE data, data2;

        if (Address<zx81.m1not)
        {
                // This is not video related, so just return the opcode
                // and make some noise onscreen.
                data = zx81_readbyte(Address);
                noise |= data;
                return(data);
        }

        // We can only execute code below M1NOT.  If an opcode fetch occurs
        // above M1NOT, we actually fetch (address&32767).  This is important
        // because it makes it impossible to place the display file in the
        // 48-64k region if a 64k RAM Pack is used.  How does the real
        // Hardware work?

        data = zx81_readbyte((Address>=49152)?Address&32767:Address);
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
                        int c;
                        // If the Chroma 81 interface is enabled, we had better fetch
                        // the ink and paper colour from memory too.

                        c = memory[Address];
                                
                        ink = c&15;
                        paper = (c>>4) & 15;
		}

                data=zx81_readbyte((z80.i<<8) | (z80.r7 & 128) | ((z80.r-1) & 127));
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
                        int c;

                        // If the Chroma 81 interface is enabled, we had better fetch
                        // the ink and paper colour from memory too.

                        if (zx81.chromamode&0x10) {    // Attribute file
                                c=memory[Address];
			} else {                       // Character code
                                data2 = ((data&0x80)>>1) | (data&0x3f);
	                        c=memory[0xc000 + (data2<<3) + rowcounter];
			}
                                
                        ink = c&15;
                        paper = (c>>4) & 15;
                }

                // First try to figure out which character set we're going
                // to use if CHR$x16 is in use.  Else, standard ZX81
                // character sets are only 64 characters in size.

                if ((zx81.chrgen==CHRGENCHR16 && (z80.i&1))
		 || (zx81.chrgen==CHRGENQS && zx81.enableqschrgen))
                        data = ((data&128)>>1)|(data&63);
                else    data = data&63;


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
                        if (zx81.extfont || (zx81.chrgen==CHRGENQS && zx81.enableqschrgen))
                                data= font[(data<<3) | rowcounter];
                        else    data=zx81_readbyte((((z80.i&254)<<8) + (data<<3)) | rowcounter);
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

                        c = memory[8192+(Address&1023)+1];

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

	if (Address>=sdl_emulator.bdis && Address<=sdl_emulator.edis && doda)
	        disassemble(Address, opcode);
	doda = 0;

	return opcode;
}

void zx81_writeport(int Address, int Data, int *tstates)
{
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

        case 0x0f:
                if (zx81.aytype==AY_TYPE_ZONX)
                        sound_ay_write(SelectAYReg, Data, 0);
                break;
        case 0x1f:
        case 0xf7:
                if (zx81.aytype==AY_TYPE_ZONX)
                        sound_ay_write(SelectAYReg, Data, 1);
                break;

        case 0xbf:
        case 0xcf:
        case 0xdf:
                if (zx81.aytype==AY_TYPE_ACE) sound_ay_write(SelectAYReg, Data, 0);
                if (zx81.aytype==AY_TYPE_ZONX) SelectAYReg=Data&15;
                break;

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

        case 0xff: // default out handled below
	        break;

        default:
		//		printf("Unhandled port write: %d\n", Address);
                break;
        }

        if (!LastInstruction) LastInstruction=LASTINSTOUTFF;
}

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

                case 0x5f:
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

void checkhsync(int tolchk)
{
	if ( ( !tolchk && sync_len >= HSYNC_MINLEN && sync_len <= HSYNC_MAXLEN && RasterX>=HSYNC_TOLERANCEMIN ) ||
	     (  tolchk &&                                                         RasterX>=HSYNC_TOLERANCEMAX ) )
	{
		RasterX = 0;
		RasterY++;
		dest += TVP;
	}
}

void checkvsync(int tolchk)
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

void checksync()
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
			VSYNC_state = 0;
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

        do
        {

/* at this point, z80.pc points to the instruction to be executed;
   so if nmi or int is pending, the RST instruction with the right number of tstates
   is emulated */

		ts = 0;

                if (int_pending && !nmi_pending) {
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

		if (nmi_pending) ts = z80_nmi(0);

		LastInstruction = LASTINSTNONE;
		if (!nmi_pending && !int_pending) {
		        doda = 1;
			z80.pc.w = PatchTest(z80.pc.w);
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
                        NMI_generator=0;
			anyout();
                        break;
                case LASTINSTOUTFE:
                        if (zx81.machine!=MACHINEZX80) NMI_generator=1;
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
			if (hsync_counter>=machine.tperscanline) {
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

				if (VSYNC_state) {
					rowcounter = 0;
				} else {
					rowcounter++;
					rowcounter &= 7;
				}

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

/* Configuration variables used in EightyOne code */

	zx81.chrgen          = CHRGENSINCLAIR;
	zx81.Chroma81        = sdl_emulator.ramsize < 48 ? 0 : 1;
	zx81.dirtydisplay    = 0;
	zx81.enableqschrgen  = 0;
	zx81.extfont         = 0;
	zx81.colour          = COLOURDISABLED;
	zx81.m1not           = (sdl_emulator.m1not && (sdl_emulator.ramsize>=32)) ? 49152 : 32768;
	zx81.machine         = zx80 ? MACHINEZX80 : MACHINEZX81;
	zx81.maxireg         = (sdl_emulator.ramsize==24 || sdl_emulator.ramsize==56) ? 0x20 : 0x40;
	zx81.NTSC            = 0;    // set to 1 for H.E.R.O.
	zx81.protectROM      = 1;
	zx81.RAMTOP          = (sdl_emulator.ramsize < 16) ? (0x4000+sdl_emulator.ramsize*0x400-1) : ((sdl_emulator.ramsize < 48) ? ((sdl_emulator.ramsize == 32) ? 0xbfff : 0x7fff) : 0xffff);
	zx81.ROMTOP          = zx80 ? 0x0fff : 0x1fff;
	zx81.speedup         = 0;
	zx81.shadowROM       = 0;
	zx81.simpleghost     = 0;
	zx81.single_step     = 0;
	zx81.truehires       = HIRESWRX; // or HIRESDISABLED and check zx81.maxireg
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
