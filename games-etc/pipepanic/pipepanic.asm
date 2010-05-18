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
MARGIN:		defb	55		; Blank lines above/below TV picture: US = 31, UK = 55.
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

;                   v
;                   |
;    +------->------+
;    |              |
;    |              v
;    |              |
;    |         +---------+
;    |         | Splash  |
;    |         | Screen  |
;    |         |         |
;    |         +---------+
;    |            |   |
;    |       Back ^   v
;    |            |   |
;    |         +---------+  Help   +---------+
;    |         | Options |---->----| Help    |
;    ^----<----| Screen  |         | Screens |
;    |   Save  |         |----<----|         |
;    |         +---------+   Back  |         |
;    |            |   |            |         |
;    |       Back ^   v Play       |   ~~~   |
;    |            |   |            |         |
;    |         +---------+  Help   |         |
;    |         | Game    |---->----|         |
;    +----<----| Screen  |         |         |
;        Save  |         |----<----|         |
;              +---------+   Back  +---------+
; 

WRITE_TO_D_FILE	equ	0
WRITE_TO_SCRBUF	equ	1

STATE_SAVE	equ	0
STATE_QUIT	equ	1
STATE_SPLASH	equ	2
STATE_OPTIONS	equ	3
STATE_HELP	equ	4
STATE_GAME	equ	5

mem_16514:	jr	start
screen_buffer:	defw	0
fade_offsets:	defw	0
temp_buffer:	defw	0
board_data:	defw	0
keyb_buffer:	defw	0
keyb_buffer_old: defw	0
events_array:	defw	0
sp_original:	defw	0
rnd_seed:	defb	0
program_state:	defb	0
previous_state:	defb	0

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
		ld	bc,6
		ccf
		sbc	hl,bc
		ld	(temp_buffer),hl
		ld	bc,(2*8+1)*3*8+1
		ccf
		sbc	hl,bc
		ld	(board_data),hl
		ld	bc,40
		ccf
		sbc	hl,bc
		ld	(keyb_buffer),hl
		ld	bc,40
		ccf
		sbc	hl,bc
		ld	(keyb_buffer_old),hl
		ld	bc,16
		ccf
		sbc	hl,bc
		ld	(events_array),hl
		ld	sp,hl

		ld	a,_SPC			; Initialise the screen
		call	screen_buffer_wipe	; buffer eol markers.
		call	splash_draw
		call	screen_buffer_blit
		call	fade_offsets_create
		call	board_data_create
		ld	a,STATE_QUIT
		ld	(previous_state),a
		ld	a,STATE_SPLASH
		ld	(program_state),a
		;Register blink for txt_press_a_key here.

stl0:		call	events_generate
		ld	a,(program_state)
		cp	STATE_QUIT
		jp	z,st_quit
		cp	STATE_SPLASH
		jr	nz,stl20
		; *********************
		; * STATE_SPLASH      *
		; *********************

		call	keyboard_get	; temp temp
		cp	0xff
		jr	z,stl100
		ld	a,STATE_OPTIONS
		call	state_change
		call	options_draw
		call	screen_buffer_fade

		jr	stl100
stl20:		cp	STATE_OPTIONS
		jr	nz,stl40
		; *********************
		; * STATE_OPTIONS     *
		; *********************

		call	keyboard_get	; temp temp
		cp	0xff
		jr	z,stl100
		ld	a,STATE_GAME
		call	state_change
		call	game_draw
		call	screen_buffer_fade

		jr	stl100
stl40:		cp	STATE_HELP
		jr	nz,stl60
		; *********************
		; * STATE_HELP        *
		; *********************

		jr	stl100
stl60:		cp	STATE_GAME
		jr	nz,stl100
		; *********************
		; * STATE_GAME        *
		; *********************

		call	keyboard_get	; temp temp
		cp	0xff
		jr	z,stl100
		ld	a,STATE_QUIT
		call	state_change
		
		jr	stl100

stl100:		ld	a,(program_state)
		cp	STATE_SPLASH
		jr	nz,stl120
		call	splash_draw
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*23+10
		ld	de,txt_press_a_key
		call	string_write
		jr	stl200
stl120:		cp	STATE_OPTIONS
		jr	nz,stl140
		call	options_draw
		jr	stl200
stl140:		cp	STATE_HELP
		jr	nz,stl160
		;call	help_draw
		jr	stl200
stl160:		cp	STATE_GAME
		jr	nz,stl200
		call	game_draw
		jr	stl200

stl200:		
		ld	hl,(screen_buffer)	; temp temp
		ld	bc,33*9
		add	hl,bc
		ld	a,(rnd_seed)
		inc	a
		bit	5,a
		jr	z,stl1
		xor	a
stl1:		ld	(rnd_seed),a
		ld	b,0
		ld	c,a
		add	hl,bc
		ld	(hl),_ASKV

		;call	blink_apply
		call	screen_buffer_blit
		;call	timer_tick		; Wishful thinking ;)
		jp	stl0

st_quit:	ld	sp,(sp_original)
		ld	a,(program_state)	; Return program state
		ld	b,0			; in bc so that SAVE
		ld	c,a			; can be checked for.
		ret

; *********************************************************************
; Events Generate                                                     *
; *********************************************************************

events_generate:
		call	keyb_buffer_update
		; Compare keyb_buffer to keyb_buffer_old here
		; and generate pressed/released events.
		ret

; *********************************************************************
; Keyboard Buffer Update                                              *
; *********************************************************************
; This first copies the existing keyb_buffer to keyb_buffer_old and
; then records the current state of all keys within the keyb_buffer.
; 
; This is how the keyboard is organised :-
; 
; +-----------------------------+
; | Port | Keys  |  Keys | Port |
; +      |---------------|      +
; |      |     Bits      |      |
; |      | 01234   43210 |      |
; +-----------------------------+
; | F7FE | 12345 | 67890 | EFFE |
; | FBFE | QWERT | YUIOP | DFFE | ^ = Shift
; | FDFE | ASDFG | HJKL* | BFFE | * = Newline
; | FEFE | ^ZXCV | BNM._ | 7FFE | _ = Space
; +-----------------------------+
; 
; So...
; ld bc,{choose a port from above}
; in a,(c)
; cpl
; ...returns 0x81 if any keys under bit0 are pressed, 0x84 under bit2
; and 0x90 under bit4

keyb_buffer_update:
		ld	hl,(keyb_buffer)	; Copy current contents
		ld	de,(keyb_buffer_old)	; of keyb_buffer to
		ld	bc,40			; keyb_buffer_old.
		ldir

;-----------------------------------------
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*2
		ld	de,kbu_data
		;call	string_write

		ld	a,0
		ld	bc,0xf7fe
		ld	hl,33*2+6
kbul0:		push	af
		push	bc
		push	hl
		in	a,(c)
		cpl
		ld	d,0
		ld	e,a
		ld	b,h
		ld	c,l
		ld	hl,0x0002
		ld	a,WRITE_TO_SCRBUF
		;call	hex_write
		pop	hl
		ld	bc,33
		add	hl,bc
		pop	bc
		rrc	b
		pop	af
		inc	a
		bit	2,a
		jr	z,kbul0

		ld	a,0
		ld	bc,0xeffe
		ld	hl,33*2+11
kbul1:		push	af
		push	bc
		push	hl
		in	a,(c)
		cpl
		ld	d,0
		ld	e,a
		ld	b,h
		ld	c,l
		ld	hl,0x0002
		ld	a,WRITE_TO_SCRBUF
		;call	hex_write
		pop	hl
		ld	bc,33
		add	hl,bc
		pop	bc
		rlc	b
		pop	af
		inc	a
		bit	2,a
		jr	z,kbul1

		ret

kbu_data:	defb	_1,_2,_3,_4,_5,0xd4,_SPC,_MNS,0xd4,_SPC,_6,_7,_8,_9,_0,0xf1
		defb	_Q,_W,_E,_R,_T,0xd4,_SPC,_MNS,0xd4,_SPC,_Y,_U,_I,_O,_P,0xf1
		defb	_A,_S,_D,_F,_G,0xd4,_SPC,_MNS,0xd4,_SPC,_H,_J,_K,_L,_ASK,0xf1
		defb	_ASK,_Z,_X,_C,_V,0xd4,_SPC,_MNS,0xd4,_SPC,_B,_N,_M,_FST,_ASK,0xf1
		defb	0xf0
;-----------------------------------------

		;call	KEYB_SCAN

		;ld	hl,(MARGIN)

;		ld	a,h		; Invert the bits in hl.
;		xor	0xff
;		ld	h,a
;		ld	a,l
;		xor	0xff
;		ld	l,a

;		push	hl		; temp temp
;		ld	a,WRITE_TO_SCRBUF
;		ld	bc,33*2+5
;		ld	d,h
;		ld	e,l	
;		ld	hl,0x0004
;		call	hex_write
;		pop	hl


;		ld	de,(keyb_buffer)
;		ld	bc,0x0401
;kbul0:		ld	a,h
;		and	b
;		jr	z,kbul1
;		ld	a,l
;		and	c
;		jr	z,kbul1
;		ld	a,1
;kbul1:		ld	(de),a
;		inc	de
;		sla	b
;		bit	6,b
;		jr	z,kbul0
;		ld	b,0x02
;		sla	c
;		jr	nz,kbul0

;		ld	bc,0		; temp temp
;kbul99:		push	bc
;		ld	hl,(keyb_buffer)
;		add	hl,bc
;		ld	e,(hl)
;		ld	hl,33*3
;		add	hl,bc
;		add	hl,bc
;		ld	b,h
;		ld	c,l	
;		ld	hl,0x0001
;		ld	a,WRITE_TO_SCRBUF
;		call	hex_write
;		pop	bc
;		inc	c
;		ld	a,14
;		cp	c
;		jr	nz,kbul99
		
		;bit	0,h
		;jr	z,kbul0
		;res	0,h		; Convert the 0100 SHIFT
		;set	1,h		; to a more usable 0201.
		;set	0,l

		ret

; *********************************************************************
; Timer Tick                                                          *
; *********************************************************************
; This ticks at 25Hz on a 50Hz ZX81.

timer_tick:	ld	a,(FRAMES)
		and	1
		jr	nz,timer_tick
		ret

; *********************************************************************
; State Change                                                        *
; *********************************************************************
; On entry: a = the new program state (the previous state is recorded)

state_change:	ld	b,a
		ld	a,(program_state)
		ld	(previous_state),a
		ld	a,b		
		ld	(program_state),a
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

splash_draw:	ld	a,_SPC
		call	screen_buffer_wipe
		ld	a,WRITE_TO_SCRBUF
		ld	bc,0
		ld	de,splash_data
		call	string_write
		ret

; *********************************************************************
; Options Draw                                                        *
; *********************************************************************
; This draws the options screen to the screen buffer.

options_draw:	ld	a,_SPC
		call	screen_buffer_wipe
		ld	a,WRITE_TO_SCRBUF
		ld	bc,0
		ld	de,options_data
		call	string_write
		ret
		
; *********************************************************************
; Game Draw                                                           *
; *********************************************************************
; This draws the game screen to the screen buffer.

game_draw:	ld	a,WRITE_TO_SCRBUF
		ld	bc,0
		ld	de,panel_data
		call	string_write
		ld	a,WRITE_TO_SCRBUF
		ld	bc,8
		ld	de,(board_data)
		call	string_write
		ret

; *********************************************************************
; Board Data Create                                                   *
; *********************************************************************
; This creates the board data.

board_data_create:
		ld	hl,(board_data)
		ld	a,_SPC
		ld	d,_SLS
		ld	b,8
bdcl0:		push	bc
		ld	b,3
bdcl1:		push	bc
		ld	b,8
bdcl2:		ld	c,0xd3
		ld	(hl),c
		inc	hl
		ld	(hl),a
		inc	hl
		ld	c,a
		ld	a,d
		ld	d,c
		djnz	bdcl2
		ld	c,0xf1
		ld	(hl),c
		inc	hl
		pop	bc
		djnz	bdcl1
		ld	c,a
		ld	a,d
		ld	d,c
		pop	bc
		djnz	bdcl0
		ld	c,0xf0
		ld	(hl),c
		inc	hl
		ret

; *********************************************************************
; Hex Write                                                           *
; *********************************************************************
; This writes hexadecimal (or BCD) numbers somewhere.
; 
; On entry: a  = 0 to write to the D_FILE
;           a  = 1 to write to the screen buffer
;           bc = destination offset (the D_FILE's initial NL is
;                accounted for so don't include it yourself)
;           de = the number to write
;           h  = 0 for leading zeroes
;           h  = 1 for leading spaces
;           h  = 2 for no leading chars
;           l  = the number of nibbles to write: 4 to 1.

hex_write:	push	af
		push	bc
		ld	bc,(temp_buffer)
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
		jr	nz,hwl7
		ld	a,h
		cp	2
		jr	z,hwl9
		cp	1
		ld	a,0
		jr	z,hwl8
hwl7:		add	a,0x1c
hwl8:		ld	(bc),a
		inc	bc
hwl9:		dec	l
		jr	nz,hwl0
		ld	a,0xf0
		ld	(bc),a
		pop	bc
		pop	af
		ld	de,(temp_buffer)
		call	string_write
		ret

; *********************************************************************
; String Write                                                        *
; *********************************************************************
; This writes one or more strings somewhere. It accepts the following
; embedded formatting chars :-
; 
; * 0xd1 to 0xdf to duplicate the next char 1 to 15 times
; * 0xe1 to 0xef to skip 1 to 15 columns
; * 0xf1 to 0xff for 1 to 15 newlines (CR to original xoffset + LF)
; * 0xf0 which MUST terminate the data
; 
; Look in the data section for examples on how to use this.
; 
; On entry: a  = 0 to write to the D_FILE
;           a  = 1 to write to the screen buffer
;           bc = destination offset (the D_FILE's initial NL is
;                accounted for so don't include it yourself)
;           de = address of data

string_write:	or	a
		jr	nz,swl0
		ld	hl,(D_FILE)
		inc	hl
		jr	swl1
swl0:		ld	hl,(screen_buffer)
swl1:		add	hl,bc
swl2:		push	hl
swl3:		ld	a,(de)
		inc	de
		cp	0xd0
		jr	nc,swl4		; Jump if >=
		ld	(hl),a		; Normal char.
		inc	hl
		jr	swl3
swl4:		cp	0xe0
		jr	nc,swl6		; Jump if >=
		and	0x0f		; Duplicate next char.
		ld	b,a
		ld	a,(de)		; Get next char.
		inc	de
swl5:		ld	(hl),a
		inc	hl
		djnz	swl5
		jr	swl3
swl6:		cp	0xf0
		jr	nc,swl7		; Jump if >=
		and	0x0f		; Skip columns.
		ld	c,a
		ld	b,0
		add	hl,bc
		jr	swl3
swl7:		jr	nz,swl8		; Jump if a != 0xf0
		pop	hl		; End of data.
		ret
swl8:		pop	hl		; Newlines.
		and	0x0f
		ld	bc,33
swl9:		add	hl,bc
		dec	a
		jr	nz,swl9
		jr	swl2

; *********************************************************************
; Keyboard Wait                                                       *
; *********************************************************************
; This waits for a key press and returns the result. Note that if more
; than one key is being pressed (excluding SHIFT) then it will always
; return 0x00 i.e. space.
; 
; On exit: a = char pressed

keyboard_wait:	call	KEYB_SCAN	; Wait until the user has
		ld	a,0xff		; released the keyboard first.
		cp	l
		jr	nz,keyboard_wait
kwl0:		call	KEYB_SCAN	; Get keyboard state in hl.
		ld	a,0xff
		cp	l
		jr	z,kwl0
		ld	b,h
		ld	c,l
		call	KEYB_DECODE	; Decode bc to a char.
		ld	a,(hl)
		ret

; *********************************************************************
; Keyboard Get                                                        *
; *********************************************************************
; This returns the current key being pressed else 0xff.
; 
; On exit: a = char if key pressed
;          a = 0xff if no key pressed

keyboard_get:	call	KEYB_SCAN	; Get keyboard state in hl.
		ld	a,0xff
		cp	l
		jr	z,kgl0
		ld	b,h
		ld	c,l
		call	KEYB_DECODE	; Decode bc to a char.
		ld	a,(hl)

		push	af		; temp temp
		ld	d,0
		ld	e,a
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*1
		ld	hl,0x0004
		call	hex_write
		pop	af

kgl0:		ret

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
		ld 	a,(rnd_seed)	; Zero will result in no
		or	a		; feedback and must be checked
		jr	nz,grnl0	; for and dealt with.
		dec	a		; The tap sequence relates to
grnl0:		srl	a		; the bit index like this :-
		ld	b,a		; Taps: 12345678
		srl	a		; Bits: 76543210
		srl	a
		xor	b		; tap6 xor tap8
		ld	a,b
		srl	a
		srl	a
		srl	a
		xor	b		; tap5 xor (tap6 xor tap8)
		ld	a,b
		srl	a
		srl	a
		srl	a
		srl	a
		xor	b		; tap4 xor (tap5 xor (tap6 xor tap8))
		and	1
		rrca
		or	b
		ld	(rnd_seed),a
		ret

; *********************************************************************
; Data                                                                *
; *********************************************************************

options_data:	defb	0xf4
		defb	0xea,_S,_E,_T,_SPC,_CV,_O,_N,_T,_R,_O,_L,_S,0xf2
		defb	0xea,_U,_P,0xe5,_EQU,0xf1
		defb	0xea,_D,_O,_W,_N,0xe3,_EQU,0xf1
		defb	0xea,_L,_E,_F,_T,0xe3,_EQU,0xf1
		defb	0xea,_R,_I,_G,_H,_T,0xe2,_EQU,0xf1
		defb	0xea,_S,_E,_L,_E,_C,_T,0xe1,_EQU,0xf2
		defb	0xee,_PV,_L,_A,_Y,0xf2
		defb	0xee,_SV,_A,_V,_E,0xf2
		defb	0xee,_HV,_E,_L,_P,0xf2
		defb	0xee,_QV,_U,_I,_T,0xf3
		defb	0xe4,_OBR,_C,_CBR,_SPC,_2,_0,_1,_0,_SPC,_T,_H,
		defb	_U,_N,_O,_R,_CMA,_SPC,_G,_N,_U,_SPC,_G,_P,_L,0xf2
		defb	0xea,_S,_Z,_8,_1,_FST,_S,_F,_FST,_N,_E,_T
		defb	0xf0

panel_data:	defb	_H,_I,_S,_C,_O,_R,_EV,0xf3
		defb	_S,_C,_O,_R,_E,0xf3
		defb	_T,_I,_M,_E,_OBR,_S,_CBR,0xf3
		defb	0xd7,_MNS,0xf1
		defb	0xe3,_GTH,0xf1
		defb	0xe3,_GTH,0xf1
		defb	0xe3,_GTH,0xf1
		defb	0xd7,_MNS,0xf2
		defb	_FV,_I,_L,_L,0xf2
		defb	_NV,_E,_W,0xf2
		defb	_SV,_A,_V,_E,0xf2
		defb	_HV,_E,_L,_P,0xf2
		defb	_BV,_A,_C,_K
		defb	0xf0

splash_data:	defb	0xfa
		defb	0xe9,_S,_P,_L,_A,_S,_H,_SPC,_S,_C,_R,_E,_E,_N
		defb	0xf0

txt_press_a_key:
		defb	_P,_R,_E,_S,_S,_SPC,_A,_SPC,_K,_E,_Y
		defb	0xf0

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






