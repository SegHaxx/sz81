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
FRAMES:		defw	0
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

mem_16514:	jp	start
rnd_seed:	defb	0
sp_original:	defw	0
screen_buffer:	defw	0
fade_in_data:	defw	0

start:		ld	(sp_original),sp
		ld	hl,0
		add	hl,sp
		ld	bc,24*33
		ccf
		sbc	hl,bc
		ld	(screen_buffer),hl
		ld	bc,26*33*2
		ccf
		sbc	hl,bc
		ld	(fade_in_data),hl
		ld	sp,hl
		
		ld	a,_SPC			; Wipe screen buffer and
		call	screen_buffer_wipe	; set-up eol markers.
		;call	screen_buffer_blit

		call	fade_in_data_setup

		call	panel_draw
		call	board_draw
		call	screen_buffer_fade_in

		ld	sp,(sp_original)
		ret


; *********************************************************************
; Fade In Data Set-Up                                                 *
; *********************************************************************
; This creates the fade-in offsets from fade_in_data_x/y and stores
; them in a buffer within the stack. It takes a moment to set-up but
; the fade-ins are faster.

fade_in_data_setup:
		ld	de,0		; x
		ld	hl,0		; y
		ld	bc,0		; count
fidsul0:	push	de
		push	hl
		push	bc
		ld	bc,fade_in_data_y
		add	hl,bc
		ld	b,33
		ld	c,(hl)
		call	multiply
		push	bc
		ex	de,hl
		ld	bc,fade_in_data_x
		add	hl,bc
		ld	b,0
		ld	c,(hl)
		pop	hl
		add	hl,bc
		push	hl
		pop	de		; de is now the char offset.
		pop	hl
		push	hl
		add	hl,hl		; Point to a word.
		ld	bc,(fade_in_data)
		add	hl,bc
		ld	(hl),e
		inc	hl
		ld	(hl),d
		pop	bc
		pop	hl
		pop	de
		inc	e		; Increment the fade_in_data_x
		ld	a,33		; pointer and wrap it around to
		cp	e		; the start when necessary.
		jr	nz,fidsul2
		ld	e,0
fidsul2:	inc	l		; Increment the fade_in_data_y
		ld	a,26		; pointer and wrap it around to
		cp	l		; the start when necessary.
		jr	nz,fidsul3
		ld	l,0
fidsul3:	inc	bc
		ld	a,0x03
		cp	b
		jr	nz,fidsul0
		ld	a,0x5a		; 33*26=0x35a gives a nice
		cp	c		; result with two lines
		jr	nz,fidsul0	; repeated.
		ret

fade_in_data_x:	defb	26,29,10,22,7,1,24,14
		defb	16,32,15,4,30,11,28,8
		defb	17,31,5,3,20,9,12,0
		defb	25,27,19,2,21,6,18,23
		defb	13

fade_in_data_y:	defb	6,7,18,13,10,9,0,3
		defb	1,21,5,17,23,19,4,8
		defb	15,2,22,20,12,11,14,16
		defb	5,17

; *********************************************************************
; Screen Buffer Fade In                                               *
; *********************************************************************
; This transfers the contents of the screen buffer to the D_FILE in
; random char units to create a fade-in effect.

screen_buffer_fade_in:
		ld	hl,(D_FILE)
		inc	hl
		ld	de,(screen_buffer)
		ld	bc,0
sbfil0:		push	bc
		push	hl
		push	de
		push	hl
		push	de
		ld	hl,(fade_in_data)
		add	hl,bc
		ld	c,(hl)
		inc	hl
		ld	b,(hl)
		pop	hl
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
		jr	nz,sbfil0
		ld	a,0xb4
		cp	c
		jr	nz,sbfil0
		ret

; This is great but it's slightly too slow :s
;screen_buffer_fade_in:
;		ld	de,0		; x
;		ld	hl,0		; y
;		ld	bc,0		; count
;sbfil0:		push	bc
;		push	de
;		push	hl
;		ld	bc,fade_in_data_y
;		add	hl,bc
;		ld	b,33
;		ld	c,(hl)
;		call	multiply
;		push	bc
;		ex	de,hl
;		ld	bc,fade_in_data_x
;		add	hl,bc
;		ld	b,0
;		ld	c,(hl)
;		pop	hl
;		add	hl,bc		; hl is now the char offset.
;		push	hl
;		ld	bc,(screen_buffer)
;		add	hl,bc
;		ld	a,(hl)		; Retrieve char from screen buffer.
;		pop	hl
;		ld	bc,(D_FILE)
;		inc	bc
;		add	hl,bc
;		ld	(hl),a		; Write char to display file.
;		pop	hl
;		pop	de
;		pop	bc
;		inc	e		; Increment the fade_in_data_x
;		ld	a,33		; pointer and wrap it around to
;		cp	e		; the start when necessary.
;		jr	nz,sbfil2
;		ld	e,0
;sbfil2:		inc	l		; Increment the fade_in_data_y
;		ld	a,26		; pointer and wrap it around to
;		cp	l		; the start when necessary.
;		jr	nz,sbfil3
;		ld	l,0
;sbfil3:		inc	bc
;		ld	a,0x03
;		cp	b
;		jr	nz,sbfil0
;		ld	a,0x5a		; 33*26=0x35a gives a nice
;		cp	c		; result with two lines
;		jr	nz,sbfil0	; repeated.
;		ret

;fade_in_data_x:	defb	26,29,10,22,7,1,24,14
;		defb	16,32,15,4,30,11,28,8
;		defb	17,31,5,3,20,9,12,0
;		defb	25,27,19,2,21,6,18,23
;		defb	13

;fade_in_data_y:	defb	6,7,18,13,10,9,0,3
;		defb	1,21,5,17,23,19,4,8
;		defb	15,2,22,20,12,11,14,16
;		defb	3,17

; This looks fantastic but the random data is 1584 bytes :s
;screen_buffer_fade_in:
;		ld	hl,(D_FILE)
;		inc	hl
;		ld	de,(screen_buffer)
;		ld	bc,0
;sbfil0:		push	bc
;		push	hl
;		push	de
;		push	hl
;		push	de
;		ld	hl,fade_in_offsets
;		add	hl,bc
;		ld	c,(hl)
;		inc	hl
;		ld	b,(hl)
;		pop	hl
;		add	hl,bc
;		ld	a,(hl)
;		pop	hl
;		add	hl,bc
;		ld	(hl),a
;		pop	de
;		pop	hl
;		pop	bc
;		inc	bc
;		inc	bc
;		ld	a,0x06		; 33*24*2=1584=0x630
;		cp	b
;		jr	nz,sbfil0
;		ld	a,0x30
;		cp	c
;		jr	nz,sbfil0
;		ret

;fade_in_offsets:
;		defw	213,757,238,535,48,253,139,617
;		defw	408,130,401,311,265,453,262,538
;		defw	531,291,232,668,603,434,40,461
;		defw	312,435,567,690,522,341,568,549
;		defw	421,616,769,740,215,251,221,548
;		defw	236,89,256,74,43,302,482,501
;		defw	120,718,523,644,237,63,25,81
;		defw	555,260,36,500,671,492,411,315
;		defw	732,771,539,598,161,254,332,784
;		defw	576,209,594,437,641,675,106,314
;		defw	402,743,185,448,636,273,660,760
;		defw	135,379,751,612,270,19,45,414
;		defw	573,386,422,551,426,439,433,112
;		defw	68,513,263,480,78,82,123,301
;		defw	735,416,693,220,462,521,37,737
;		defw	687,289,316,348,344,387,105,580
;		defw	441,219,425,278,473,697,777,162
;		defw	275,229,720,734,620,613,192,499
;		defw	116,464,624,380,51,772,744,639
;		defw	642,173,149,255,271,257,508,413
;		defw	126,571,396,674,393,507,38,268
;		defw	488,319,358,280,399,767,682,543
;		defw	140,31,445,282,659,468,293,400
;		defw	157,723,92,94,49,359,406,3
;		defw	290,310,615,701,210,560,575,684
;		defw	742,80,569,459,363,285,749,178
;		defw	510,259,292,436,649,579,199,595
;		defw	34,97,645,214,177,657,713,474
;		defw	339,61,475,208,182,656,103,382
;		defw	725,786,26,487,322,592,451,443
;		defw	470,698,27,438,56,469,62,403
;		defw	17,73,589,554,528,101,781,110
;		defw	364,190,269,530,572,383,242,509
;		defw	494,550,704,24,320,456,207,384
;		defw	692,367,731,768,602,132,463,7
;		defw	739,452,354,2,258,754,378,685
;		defw	458,590,298,124,347,756,351,529
;		defw	583,503,29,368,547,561,159,525
;		defw	696,274,188,385,223,647,665,197
;		defw	450,299,578,541,618,630,667,476
;		defw	729,205,191,626,643,60,444,395
;		defw	635,582,662,113,169,167,562,327
;		defw	284,127,736,13,172,288,244,133
;		defw	608,12,381,504,700,717,714,245
;		defw	328,673,79,553,212,790,313,782
;		defw	121,356,621,699,446,141,176,150
;		defw	224,154,42,496,691,485,136,4
;		defw	194,465,765,143,638,787,11,449
;		defw	181,759,710,484,330,472,455,766
;		defw	633,490,160,377,342,519,146,107
;		defw	272,627,58,165,688,144,622,478
;		defw	10,486,741,246,189,391,609,677
;		defw	96,565,648,321,333,337,775,747
;		defw	536,695,705,430,526,477,184,511
;		defw	407,689,20,23,566,175,505,514
;		defw	198,64,196,204,440,471,712,533
;		defw	491,222,72,728,75,489,745,83
;		defw	134,493,726,605,534,686,66,239
;		defw	394,516,397,761,286,318,527,418
;		defw	336,552,629,294,261,361,546,226
;		defw	365,604,195,389,21,84,410,69
;		defw	329,545,702,466,748,497,515,388
;		defw	247,563,307,721,338,200,128,114
;		defw	628,71,405,86,87,187,479,755
;		defw	231,779,369,502,599,650,544,304
;		defw	67,532,250,151,249,597,186,46
;		defw	90,730,357,683,417,93,588,18
;		defw	117,166,593,646,147,520,225,753
;		defw	109,227,733,651,664,419,104,428
;		defw	390,557,415,168,360,54,577,770
;		defw	460,240,611,495,99,230,277,155
;		defw	41,601,235,454,170,152,694,366
;		defw	52,279,708,119,179,709,77,625
;		defw	447,719,203,655,296,323,666,88
;		defw	193,95,353,267,370,371,343,512
;		defw	65,171,374,614,517,118,98,762
;		defw	156,350,5,216,788,317,111,591
;		defw	420,774,355,352,308,345,39,676
;		defw	783,658,654,76,297,349,44,431
;		defw	158,340,252,724,785,789,584,6
;		defw	295,498,432,266,8,163,142,764
;		defw	559,115,145,233,218,47,234,558
;		defw	70,776,637,653,727,183,607,376
;		defw	429,334,372,326,442,129,581,606
;		defw	524,746,100,398,518,672,305,287
;		defw	131,331,427,32,59,137,33,206
;		defw	412,392,248,15,600,91,506,711
;		defw	483,706,722,303,679,610,243,585
;		defw	703,623,148,715,791,283,640,53
;		defw	663,55,457,335,634,22,346,619
;		defw	778,596,201,661,631,373,180,264
;		defw	773,752,423,102,707,57,35,122
;		defw	9,217,409,587,570,174,276,763
;		defw	300,138,85,669,678,306,324,556
;		defw	228,125,153,281,424,404,0,716
;		defw	28,30,758,750,738,586,375,574
;		defw	652,362,780,108,14,467,50,481
;		defw	211,564,537,164,309,542,540,241
;		defw	680,670,325,1,632,681,202,16

; *********************************************************************
; Screen Buffer Blit                                                  *
; *********************************************************************
; This transfers the contents of the screen buffer to the D_FILE.

screen_buffer_blit:
		ld	hl,(D_FILE)
		inc	hl
		ld	de,(screen_buffer)
		ex	de,hl
		ld	bc,24*33
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
wsbl1:		push	bc
		ld	b,32
wsbl2:		ld	(hl),a
		inc	hl
		djnz	wsbl2
		ld	(hl),_NL
		inc	hl
		pop	bc
		djnz	wsbl1
		ret

; *********************************************************************
; Panel Draw                                                          *
; *********************************************************************
; This draws the panel to the screen buffer.

panel_draw:	ld	hl,(screen_buffer)
		ld	de,panel_data
		ex	de,hl
		ld	b,24
pdl0:		push	bc
		ld	b,0
		ld	c,8
pdl1:		ldi
		jp	pe,pdl1
		ld	b,33-8
pdl2:		inc	de
		djnz	pdl2
		pop	bc
		djnz	pdl0
		ret

panel_data:	defb	_H   ,_I   ,_S   ,_C   ,_O   ,_R   ,_EV  ,0x00
		defb	_0   ,_0   ,_0   ,_0   ,0x00 ,0x00 ,0x00 ,0x00
		defb	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
		defb	_S   ,_C   ,_O   ,_R   ,_E   ,0x00 ,0x00 ,0x00
		defb	_0   ,_0   ,_0   ,_0   ,0x00 ,0x00 ,0x00 ,0x00
		defb	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
		defb	_T   ,_I   ,_M   ,_E   ,_OBR ,_S   ,_CBR ,0x00
		defb	_0   ,_0   ,_0   ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
		defb	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
		defb	_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,0x00
		defb	0x00 ,0x00 ,0x00 ,_GTH ,0x00 ,0x88 ,0x00 ,0x00
		defb	0x00 ,_FSTV,0x00 ,_GTH ,0x88 ,0x88 ,0x88 ,0x00
		defb	0x00 ,_SPCV,0x00 ,_GTH ,0x00 ,0x00 ,0x00 ,0x00
		defb	_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,_MNS ,0x00
		defb	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
		defb	_FV  ,_I   ,_L   ,_L   ,0x00 ,0x00 ,0x00 ,0x00
		defb	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
		defb	_NV  ,_E   ,_W   ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
		defb	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
		defb	_SV  ,_A   ,_V   ,_E   ,0x00 ,0x00 ,0x00 ,0x00
		defb	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
		defb	_HV  ,_E   ,_L   ,_P   ,0x00 ,0x00 ,0x00 ,0x00
		defb	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
		defb	_QV  ,_U   ,_I   ,_T   ,0x00 ,0x00 ,0x00 ,0x00

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
; Multiply                                                            *
; *********************************************************************
; On entry: b * c
;  On exit: bc = result

multiply:	push	af
		push	de
		push	hl
		ld	hl,0
		xor	a
		cp	b
		jr	z,mull2
		cp	c
		jr	z,mull2
		ld	e,c
		ld	d,0
mull1:		add	hl,de
		djnz	mull1
mull2:		push	hl
		pop	bc
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






