; Pipepanic - a Sinclair ZX81 pipe connecting game
; Copyright (C) 2010 Thunor <thunorsif@hotmail.com>
; 
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

; =====================================================================
; pipepanic 0.0.1
; 
; Tabs are set to 8.
; 
; See pipepanic.txt for compilation instructions and information.
; =====================================================================

; Sinclair character codes.

_SPC		equ	0x00		; " "
_DQT		equ	0x0b		; "
_PND		equ	0x0c		; £
_DLR		equ	0x0d		; $
_CLN		equ	0x0e		; :
_QMK		equ	0x0f		; ?
_OBR		equ	0x10		; (
_CBR		equ	0x11		; )
_GTH		equ	0x12		; >
_LTH		equ	0x13		; <
_EQU		equ	0x14		; =
_PLS		equ	0x15		; +
_MNS		equ	0x16		; -
_ASK		equ	0x17		; *
_SLS		equ	0x18		; /
_SMC		equ	0x19		; ;
_CMA		equ	0x1a		; ,
_FST		equ	0x1b		; .
_0		equ	0x1c
_1		equ	0x1d
_2		equ	0x1e
_3		equ	0x1f
_4		equ	0x20
_5		equ	0x21
_6		equ	0x22
_7		equ	0x23
_8		equ	0x24
_9		equ	0x25
_A		equ	0x26
_B		equ	0x27
_C		equ	0x28
_D		equ	0x29
_E		equ	0x2a
_F		equ	0x2b
_G		equ	0x2c
_H		equ	0x2d
_I		equ	0x2e
_J		equ	0x2f
_K		equ	0x30
_L		equ	0x31
_M		equ	0x32
_N		equ	0x33
_O		equ	0x34
_P		equ	0x35
_Q		equ	0x36
_R		equ	0x37
_S		equ	0x38
_T		equ	0x39
_U		equ	0x3a
_V		equ	0x3b
_W		equ	0x3c
_X		equ	0x3d
_Y		equ	0x3e
_Z		equ	0x3f

; Inverse equivalents of the above.

_SPCV		equ	_SPC+0x80	; [ ]
_DQTV		equ	_DQT+0x80	; ["]
_PNDV		equ	_PND+0x80	; [£]
_DLRV		equ	_DLR+0x80	; [$]
_CLNV		equ	_CLN+0x80	; [:]
_QMKV		equ	_QMK+0x80	; [?]
_OBRV		equ	_OBR+0x80	; [(]
_CBRV		equ	_CBR+0x80	; [)]
_GTHV		equ	_GTH+0x80	; [>]
_LTHV		equ	_LTH+0x80	; [<]
_EQUV		equ	_EQU+0x80	; [=]
_PLSV		equ	_PLS+0x80	; [+]
_MNSV		equ	_MNS+0x80	; [-]
_ASKV		equ	_ASK+0x80	; [*]
_SLSV		equ	_SLS+0x80	; [/]
_SMCV		equ	_SMC+0x80	; [;]
_CMAV		equ	_CMA+0x80	; [,]
_FSTV		equ	_FST+0x80	; [.]
_0V		equ	_0+0x80
_1V		equ	_1+0x80
_2V		equ	_2+0x80
_3V		equ	_3+0x80
_4V		equ	_4+0x80
_5V		equ	_5+0x80
_6V		equ	_6+0x80
_7V		equ	_7+0x80
_8V		equ	_8+0x80
_9V		equ	_9+0x80
_AV		equ	_A+0x80
_BV		equ	_B+0x80
_CV		equ	_C+0x80
_DV		equ	_D+0x80
_EV		equ	_E+0x80
_FV		equ	_F+0x80
_GV		equ	_G+0x80
_HV		equ	_H+0x80
_IV		equ	_I+0x80
_JV		equ	_J+0x80
_KV		equ	_K+0x80
_LV		equ	_L+0x80
_MV		equ	_M+0x80
_NV		equ	_N+0x80
_OV		equ	_O+0x80
_PV		equ	_P+0x80
_QV		equ	_Q+0x80
_RV		equ	_R+0x80
_SV		equ	_S+0x80
_TV		equ	_T+0x80
_UV		equ	_U+0x80
_VV		equ	_V+0x80
_WV		equ	_W+0x80
_XV		equ	_X+0x80
_YV		equ	_Y+0x80
_ZV		equ	_Z+0x80

_NL		equ	0x76

_INT		equ	0xcf
_USR		equ	0xd4
_PWR		equ	0xd8		; **
_REM		equ	0xea
_LET		equ	0xf1
_RUN		equ	0xf7
_RAND		equ	0xf9

; RST routines (a selection of).

ERROR		equ	0x08		; Follow with a byte error code -1.
PRINT_CHAR	equ	0x10		; regA contains the char.

; ROM routines (a selection of).

KEYB_SCAN	equ	0x02bb
KEYB_DECODE	equ	0x07bd

; Start of the system variables area.

ERR_NR		equ	0x4000
FLAGS		equ	0x4001
ERR_SP		equ	0x4002
RAMTOP		equ	0x4004
MODE		equ	0x4006
PPC		equ	0x4007

p_start:	org	0x4009

VERSN:		defb	0
E_PPC:		defw	20		; BASIC line number of line with cursor.
D_FILE:		defw	display_file
DF_CC:		defw	display_file+1
VARS:		defw	variables
DEST:		defw	0
E_LINE:		defw	edit_line
CH_ADD:		defw	p_end-1
X_PTR:		defw	0
STKBOT:		defw	p_end
STKEND:		defw	p_end
BERG:		defb	0
MEM:		defw	MEMBOT
SPARE1:		defb	0
DF_SZ:		defb	2		; Number of lines in lower part of screen.
S_TOP:		defw	20		; BASIC line number of line at top of screen.
LAST_K:		defw	0xffff
DB_ST:		defb	0
MARGIN:		defb	0x37
NXTLIN:		defw	display_file	; Memory address of next program line to be executed.
OLDPPC:		defw	0
FLAGX:		defb	0
STRLEN:		defw	0
T_ADDR:		defw	0x0c8d
SEED:		defw	0
FRAMES:		defw	0		; Updated once for every TV frame displayed.
COORDS:		defw	0
PR_CC:		defb	0xbc
S_POSN:		defb	0x21,0x18
CDFLAG:		defb	0x40
PRBUF:		defs	0x20
		defb	_NL
MEMBOT:		defs	0x1e
SPARE2:		defw	0

; Start of the BASIC area for user programs.

basic_0001:	defb	0,1		; 1 REM
		defw	basic_0010-basic_0001-4
		defb	_REM

; Start of user machine code program vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

WRITE_TO_D_FILE	equ	0
WRITE_TO_SCRBUF	equ	1

mem_16514:	jr	start
rnd_seed:	defb	0
sp_original:	defw	0
screen_buffer:	defw	0
fade_offsets:	defw	0

start:		ld	(sp_original),sp
		ld	hl,0
		add	hl,sp
		ld	bc,33*24
		ccf
		sbc	hl,bc
		ld	(screen_buffer),hl
		ld	bc,33*26*2
		ccf
		sbc	hl,bc
		ld	(fade_offsets),hl
		ld	sp,hl
		
		ld	a,_SPC			; Wipe screen buffer and
		call	screen_buffer_wipe	; set-up eol markers.

		call	splash_draw
		call	screen_buffer_blit

		ld	a,WRITE_TO_D_FILE	; speed test temp temp
		ld	bc,33*0
		ld	de,(FRAMES)
		ld	h,2
		call	hex_write

		call	fade_offsets_create

		ld	a,WRITE_TO_D_FILE	; speed test temp temp
		ld	bc,33*1
		ld	de,(FRAMES)
		ld	h,2
		call	hex_write

		ld	a,WRITE_TO_D_FILE
		ld	bc,33*24-1-12
		ld	de,txt_press_a_key
		call	char_write

		call	keyboard_wait
		
		;call	main_draw
		;call	screen_buffer_fade

		call	panel_draw
		call	board_draw

		call	screen_buffer_fade

		call	keyboard_wait		; temp temp

		ld	sp,(sp_original)
		ret

; *********************************************************************
; Keyboard Wait                                                       *
; *********************************************************************
; This waits for a key press and returns the result. Note that if more
; than one key is being pressed (excluding SHIFT) then it will always
; return 0x00 i.e. space.
; 
; On exit: a = char pressed

keyboard_wait:	push	bc
		push	de
		push	hl
kwl0:		call	KEYB_SCAN
		ld	b,h
		ld	c,l
		ld	a,0xff
		cp	l
		jr	z,kwl0
		call	KEYB_DECODE
		ld	a,(hl)
		pop	hl
		pop	de
		pop	bc
		ret

; *********************************************************************
; Fade Offsets Create                                                 *
; *********************************************************************
; This creates the fade offsets from fade_data_x/y and stores them in a
; buffer. It takes 28 frames @50Hz (0.56s) to set-up (the time can be
; lost in a splash screen) but once it's done the fades are just 11
; frames @50Hz (0.22s).

fade_offsets_create:
		ld	de,0		; x
		ld	hl,0		; y
		ld	bc,0		; count
focl0:		push	de
		push	hl
		push	bc
		ld	bc,fade_data_y
		add	hl,bc
		;ld	b,33		; Slow - 
		;ld	c,(hl)		; 66 frames @50Hz = 1.32s.
		;call	multiply8
		push	hl		; Much faster -
		ld	a,(hl)		; 28 frames @50Hz = 0.56s.
		ld	l,a
		ld	h,0
		ld	b,5		; Multiply by 32.
focl1:		sla	l		;   h     c     l
		rl	h		; 0011 <- 1 <- 1001
		djnz	focl1
		ld	c,a		; b is already 0.
		add	hl,bc		; And add (hl) to equal x33.
		ld	b,h
		ld	c,l
		pop	hl
		push	bc
		ex	de,hl
		ld	bc,fade_data_x
		add	hl,bc
		ld	b,0
		ld	c,(hl)
		pop	hl
		add	hl,bc
		ex	de,hl		; de is now the char offset.
		pop	hl
		push	hl
		add	hl,hl		; Point to a word.
		ld	bc,(fade_offsets)
		add	hl,bc
		ld	(hl),e
		inc	hl
		ld	(hl),d
		pop	bc
		pop	hl
		pop	de
		inc	e		; Increment the fade_data_x
		ld	a,33		; pointer and wrap it around to
		cp	e		; the start when necessary.
		jr	nz,focl2
		ld	e,0
focl2:		inc	l		; Increment the fade_data_y
		ld	a,26		; pointer and wrap it around to
		cp	l		; the start when necessary.
		jr	nz,focl3
		ld	l,0
focl3:		inc	bc
		ld	a,0x03
		cp	b
		jr	nz,focl0
		ld	a,0x5a		; 33*26=0x35a gives a nice
		cp	c		; result with some repetition
		jr	nz,focl0	; (66).
		ret

fade_data_x:	defb	26,29,10,22,7,1,24,14
		defb	16,32,15,4,30,11,28,8
		defb	17,31,5,3,20,9,12,0
		defb	25,27,19,2,21,6,18,23
		defb	13

fade_data_y:	defb	6,7,18,13,10,9,0,3
		defb	1,21,5,17,23,19,4,8
		defb	15,2,22,20,12,11,14,16
		defb	5,17

; *********************************************************************
; Screen Buffer Fade                                                  *
; *********************************************************************
; This transfers the contents of the screen buffer to the D_FILE in
; random char units to create a fade effect.

screen_buffer_fade:
		ld	hl,(D_FILE)
		inc	hl
		ld	de,(screen_buffer)
		ld	bc,0
sbfl0:		push	bc
		push	hl
		push	de
		push	hl
		ld	hl,(fade_offsets)
		add	hl,bc
		ld	c,(hl)
		inc	hl
		ld	b,(hl)
		ex	de,hl
		add	hl,bc
		ld	a,(hl)
		pop	hl
		add	hl,bc
		ld	(hl),a
		pop	de
		pop	hl
		pop	bc
		inc	bc
		inc	bc
		ld	a,0x06		; 33*26*2=1716=0x6b4
		cp	b
		jr	nz,sbfl0
		ld	a,0xb4
		cp	c
		jr	nz,sbfl0
		ret

; *********************************************************************
; Screen Buffer Blit                                                  *
; *********************************************************************
; This transfers the contents of the screen buffer to the D_FILE.

screen_buffer_blit:
		ld	hl,(D_FILE)
		inc	hl
		ld	de,(screen_buffer)
		ex	de,hl
		ld	bc,33*24
		ldir
		ret

; *********************************************************************
; Screen Buffer Wipe                                                  *
; *********************************************************************
; This wipes the screen buffer and also sets-up the eol markers.
;
; On entry: a = the value to wipe with

screen_buffer_wipe:
		ld	hl,(screen_buffer)
		ld	b,24
sbwl1:		ld	c,b
		ld	b,32
sbwl2:		ld	(hl),a
		inc	hl
		djnz	sbwl2
		ld	(hl),_NL
		inc	hl
		ld	b,c
		djnz	sbwl1
		ret

; *********************************************************************
; Splash Draw                                                         *
; *********************************************************************
; This draws the splash screen to the screen buffer.

splash_draw:	ld	a,WRITE_TO_SCRBUF
		ld	bc,33*24-1-12
		ld	de,txt_initialising
		call	char_write
		ret

; *********************************************************************
; Panel Draw                                                          *
; *********************************************************************
; This draws the panel to the screen buffer.

panel_draw:	ld	a,WRITE_TO_SCRBUF
		ld	bc,0
		ld	de,panel_data
		call	char_write
		ret
				
; *********************************************************************
; Board Draw                                                          *
; *********************************************************************
; This draws the game board to the screen buffer.

board_draw:	ld	hl,(screen_buffer)
		ld	de,8
		add	hl,de
		ld	a,_SPC
		ld	d,_SLS
		ld	b,8
bdl0:		push	bc
		ld	b,3
bdl1:		push	bc
		ld	b,8
bdl2:		push	bc
		ld	b,3
bdl3:		ld	(hl),a
		inc	hl
		djnz	bdl3
		ld	c,a
		ld	a,d
		ld	d,c
		pop	bc
		djnz	bdl2
		ld	bc,9
		add	hl,bc
		pop	bc
		djnz	bdl1
		ld	c,a
		ld	a,d
		ld	d,c
		pop	bc
		djnz	bdl0
		ret

; *********************************************************************
; Hex Write                                                           *
; *********************************************************************
; This writes hexadecimal (or BCD) numbers somewhere.
; 
; On entry: a  = 0 to write to the D_FILE
;           a  = 1 to write to the screen buffer
;           bc = offset into the D_FILE (the initial NL is accounted
;                for so don't include it) or screen buffer
;           de = the number to write
;           h  = 0 for no leading chars
;           h  = 1 for leading spaces
;           h  = 2 for leading zeroes

hex_write_data:	defs	6

hex_write:	push	af
		push	bc
		ld	bc,hex_write_data
		ld	l,4
hwl0:		ld	a,l
		cp	4
		jr	nz,hwl2
		ld	a,d
hwl1:		srl	a
		srl	a
		srl	a
		srl	a
		jr	hwl6
hwl2:		cp	3
		jr	nz,hwl4
		ld	a,d
hwl3:		and	0x0f
		jr	hwl6
hwl4:		cp	2
		jr	nz,hwl5
		ld	a,e
		jr	hwl1
hwl5:		ld	a,e
		jr	hwl3
hwl6:		or	a
		jr	nz,hwl8
		ld	a,h
		or	a
		jr	z,hwl10
		cp	1
		jr	nz,hwl7
		xor	a
		jr	hwl9
hwl7:		xor	a
hwl8:		add	a,0x1c
hwl9:		ld	(bc),a
		inc	bc
hwl10:		dec	l
		jr	nz,hwl0
		ld	a,0xff
		ld	(bc),a
		inc	bc
		dec	a
		ld	(bc),a
		pop	bc
		pop	af
		ld	de,hex_write_data
		call	char_write
		ret

; *********************************************************************
; Char Write                                                          *
; *********************************************************************
; This writes chars somewhere.
;
; On entry: a  = 0 to write to the D_FILE
;           a  = 1 to write to the screen buffer
;           bc = offset into the D_FILE (the initial NL is accounted
;                for so don't include it) or screen buffer
;           de = address of data, lines terminated with 0xff
;                and the data terminated with 0xfe

char_write:	or	a
		jr	nz,cwl0
		ld	hl,(D_FILE)
		inc	hl
		jr	cwl1
cwl0:		ld	hl,(screen_buffer)
cwl1:		add	hl,bc
cwl2:		push	hl
cwl3:		ld	a,(de)
		inc	de
		cp	0xff
		jr	z,cwl4
		ld	(hl),a
		inc	hl
		jr	cwl3
cwl4:		pop	hl
		ld	bc,33
		add	hl,bc
		ld	a,(de)
		cp	0xfe
		jr	nz,cwl2
		ret

; *********************************************************************
; Multiply8                                                           *
; *********************************************************************
; A general straightforward byte multiplication routine for use when
; speed isn't an issue or when simplicity is more desirable.
; 
; On entry: b * c
;  On exit: bc = result

multiply8:	push	af
		push	de
		push	hl
		ld	hl,0
		xor	a
		cp	b
		jr	z,mul8l2
		cp	c
		jr	z,mul8l2
		ld	e,c
		ld	d,0
mul8l1:		add	hl,de
		djnz	mul8l1
mul8l2:		ld	b,h
		ld	c,l
		pop	hl
		pop	de
		pop	af
		ret

; *********************************************************************
; Get Random Number                                                   *
; *********************************************************************
; This uses a linear feedback shift register (LFSR) algorithm with
; a maximal length tap sequence of [8,6,5,4]
; (see http://www.wikipedia.org/wiki/Linear_feedback_shift_register).
; 
; On exit: a = random number

get_random_number:
		ld 	a,(rnd_seed)
		or	a		; Zero will result in no
		jr	nz,grnl0	; feedback and must be checked
		dec	a		; for and dealt with.
grnl0:		sra	a		; The tap sequence relates to
		and	0x7f		; the bit index like this :-
		ld	b,a		; Taps: 12345678
		sra	a		; Bits: 76543210
		sra	a
		xor	b		; tap6 xor tap8
		ld	a,b
		sra	a
		sra	a
		sra	a
		xor	b		; tap5 xor (tap6 xor tap8)
		ld	a,b
		sra	a
		sra	a
		sra	a
		sra	a
		xor	b		; tap4 xor (tap5 xor (tap6 xor tap8))
		and	0x1
		rrca
		or	b
		ld	(rnd_seed),a
		ret

; *********************************************************************
; Data                                                                *
; *********************************************************************

panel_data:	defb	_H   ,_I   ,_S   ,_C   ,_O   ,_R   ,_EV  ,0xff
		defb	0xff
		defb	0xff
		defb	_S   ,_C   ,_O   ,_R   ,_E   ,0xff
		defb	0xff
		defb	0xff
		defb	_T   ,_I   ,_M   ,_E   ,_OBR ,_S   ,_CBR ,0xff
		defb	0xff
		defb	0xff
		defb	_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,0xff
		defb	0x00 ,0x00,0x00 ,_GTH ,0xff
		defb	0x00 ,0x00,0x00 ,_GTH ,0xff
		defb	0x00 ,0x00,0x00 ,_GTH ,0xff
		defb	_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,0xff
		defb	0xff
		defb	_FV  ,_I   ,_L   ,_L   ,0xff
		defb	0xff
		defb	_NV  ,_E   ,_W   ,0xff
		defb	0xff
		defb	_SV  ,_A   ,_V   ,_E   ,0xff
		defb	0xff
		defb	_HV  ,_E   ,_L   ,_P   ,0xff
		defb	0xff
		defb	_BV  ,_A   ,_C   ,_K   ,0xff
		defb	0xfe

txt_initialising:
		defb	_I,_N,_I,_T,_I,_A,_L,_I,_S,_I,_N,_G,0xff
		defb	0xfe

txt_press_a_key:
		defb	_SPC,_P,_R,_E,_S,_S,_SPC,_A,_SPC,_K,_E,_Y,0xff
		defb	0xfe

; End of user machine code program ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

		defb	_NL
basic_0010:	defb	0,10		; 10 RAND USR 16514
		defw	basic_0020-basic_0010-4
		defb	_RAND,_USR,_1,_6,_5,_1,_4
		defb	0x7e
		defb	0x8f
		defb	0x01
		defb	0x04
		defb	0x00
		defb	0x00
		defb	_NL
basic_0020:	defb	0,20		; 20 REM ** TYPE RUN **
		defw	basic_end-basic_0020-4
		defb	_REM,_PWR,_SPC,_T,_Y,_P,_E,_RUN,_PWR
		defb	_NL
basic_end:

; Start of the display file (synonymous with video memory).

; Compressed for <= 3k RAM.

;display_file:	defb	_NL
;		defb	_NL,_NL,_NL,_NL,_NL,_NL,_NL,_NL
;		defb	_NL,_NL,_NL,_NL,_NL,_NL,_NL,_NL
;		defb	_NL,_NL,_NL,_NL,_NL,_NL,_NL,_NL

; Expanded for > 3k RAM.

display_file:	defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL
		defs	32
		defb	_NL

; Start of the variables area used by BASIC.

variables:	defb	0x80

; Start of the edit line used by BASIC.

edit_line:

p_end:		end






