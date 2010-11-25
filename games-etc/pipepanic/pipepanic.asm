; Pipepanic - a Sinclair ZX81 pipe connecting game
; Copyright (C) 2010 Thunor <thunorsif_at_hotmail.com>
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
; pipepanic 0.1.3
; 
; Tabs are set to 8.
; 
; See pipepanic.txt for assembling instructions and information.
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

_VAL		equ	0xc5
_INT		equ	0xcf
_USR		equ	0xd4
_NOT		equ	0xd7
_PWR		equ	0xd8		; **
_THEN		equ	0xde
_STOP		equ	0xe3
_REM		equ	0xea
_LET		equ	0xf1
_PRINT		equ	0xf5
_RUN		equ	0xf7
_SAVE		equ	0xf8
_RAND		equ	0xf9
_IF		equ	0xfa

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
E_PPC:		defw	40		; BASIC line number of line with cursor.
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
S_TOP:		defw	10		; BASIC line number of line at top of screen.
LAST_K:		defw	0xffff
DB_ST:		defb	0
MARGIN:		defb	55		; Blank lines above/below TV picture: US = 31, UK = 55.
NXTLIN:		defw	basic_0040	; Memory address of next program line to be executed.
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

;                          |
;       +------>-----------v
;       |                  |
;       |             +---------+
;       |             | Engine  |
;       ^             | Init.   |
;       |             |         |
;       |             +---------+
;       |                  |
;       |                  v
;       |                  |
;  +---------+        +---------+
;  | Save    |        | Splash  |
;  |         |        | Screen  |---------<--------------+
;  |         |        |         |                        |
;  +---------+        +---------+                        |
;       |                  |                             |
;       ^                  v                             |
;       |                  |                             |
;       |     Save    +---------+  Help   +---------+    |
;       +------<------| Options |---->----| Help    |    |
;                     | Screen  |         | Screens |    |
;       +------<------|         |----<----|         |    |
;       |     Quit    +---------+   Back  |         |    |
;       |                |   |            |         |    |
;       v           Back ^   v Play       |   ~~~   |    ^
;       |                |   |            |         |    |
;  +---------+        +---------+  Help   |         |    |
;  | Stop    |        | Game    |---->----|         |    |
;  |         |        | Screen  |         |         |    |
;  |         |        |         |----<----|         |    |
;  +---------+        +---------+   Back  +---------+    |
;                          |                             |
;                          v Demo End                    |
;                          |                             |
;                          +-------------->--------------+

DEBUG_KEYB	equ	0
DEBUG_EVENTS	equ	0
DEBUG_STACK	equ	0
DEBUG_FILL	equ	0
DEBUG_ANIMPTR	equ	0
DEBUG_DEMOREC	equ	0

EVENTS_MAX	equ	16
TEMP_BUFFER_MAX	equ	6
HELP_PAGE_MAX	equ	3
PIPE_ARRAY_MAX	equ	70
COUNTDOWN_MAX	equ	0x0200	; BCD
FILL_ANIMATION_MAX equ	64*5	; I've experienced 214. 64*5 must be limit.
FILL_NODES_MAX	equ	8*3	; I've experienced 21. 8*3 must be limit.
WRITE_TO_D_FILE	equ	0
WRITE_TO_SCRBUF	equ	1
WRITE_TO_MEMBLK	equ	2

STATE_QUIT	equ	0
STATE_SAVE	equ	1
STATE_SPLASH	equ	2
STATE_OPTIONS	equ	3
STATE_HELP	equ	4
STATE_GAME	equ	5
STATE_HSFILL	equ	6
STATE_DEMO	equ	7

SUBSTATE_NONE	equ	0
SUBSTATE_SETCTRLS equ	1
SUBSTATE_GAME	equ	2
SUBSTATE_FILL	equ	3
SUBSTATE_REMDEAD equ	4
SUBSTATE_FILLANIM equ	5
SUBSTATE_END	equ	6
SUBSTATE_HSEND	equ	7

ONE_SECOND_PAL	equ	50
ONE_SECOND_NTSC	equ	60

FILL_CHAR	equ	0x08

mem_16514:	jp	start
temp_buffer:	defw	0
screen_buffer:	defw	0
fade_offsets:	defw	0
keyb_buffer:	defw	0
keyb_buffer_old: defw	0
event_queue:	defw	0
board_array:	defw	0
board_array2:	defw	0
pipe_pieces:	defw	0
pipe_array:	defw	0
fill_animation:	defw	0
fill_nodes:	defw	0
sp_original:	defw	0
rnd_seed:	defb	0
state_current:	defb	0
state_previous:	defb	0
substate_current: defb	0
screen_redraw:	defw	0
frames_last:	defw	0
event_queue_start: defb	0
event_queue_end: defb	0
blink_offset:	defw	0
blink_height:	defb	0
blink_width:	defb	0
blink_count:	defb	0
blink_delay:	defb	0
blink_last:	defw	0
action_up:	defb	_Q,1	; Character,repeat yes/no.
action_down:	defb	_A,1
action_left:	defb	_O,1
action_right:	defb	_P,1
action_select:	defb	_K,0
controls_set_idx: defb	0
event_repeat_delay_master: defw 0
event_repeat_interval_master: defw 0
event_repeat_delay: defw 0
event_repeat_last: defw	0
help_page:	defb	0
cursor_options_position: defb 0
cursor_options_offset: defw 0
cursor_game_position: defb 0
cursor_game_offset: defw 0
cursor_game_frame: defb	0
cursor_game_delay: defb	0
cursor_game_last: defw	0
cursor_panel_position: defb 0
cursor_panel_offset: defw 0
pipe_array_idx:	defb	0
preview_bar_slot0: defb	0
preview_bar_slot1: defb	0
score:		defw	0	; Stored as BCD.
hiscore:	defw	0	; Stored as BCD.
countdown_time: defw	0	; Stored as BCD.
countdown_delay: defb	0
countdown_last:	defw	0
fill_remdead_delay: defb 0
fill_remdead_last: defw	0
fill_animate_delay: defb 0
fill_animate_last: defw	0
fill_animate_frame: defb 0
fill_animate_subframe: defb 0
fill_animate_leak: defb	0
fill_animation_ptr: defw 0
hiscore_frame:	defb	0
hiscore_delay:	defb	0
hiscore_last:	defw	0
board_arrayhs:	defs	64
demo_start_delay: defw	0	; Used for splash timeout too.
demo_start_last: defw	0	; Used for splash timeout too.
demo_event_delay: defb	0
demo_event_last: defw	0
demo_actions_ptr: defw	0

start:		ld	(sp_original),sp
		ld	hl,0
		add	hl,sp			; Carry will be clear.
		ld	bc,TEMP_BUFFER_MAX
		sbc	hl,bc
		ld	(temp_buffer),hl
		ld	bc,33*24
		sbc	hl,bc
		ld	(screen_buffer),hl
		ld	bc,33*26*2
		sbc	hl,bc
		ld	(fade_offsets),hl
		ld	bc,40
		sbc	hl,bc
		ld	(keyb_buffer),hl
		sbc	hl,bc
		ld	(keyb_buffer_old),hl
		ld	bc,EVENTS_MAX*2
		sbc	hl,bc
		ld	(event_queue),hl
		ld	bc,8*8
		sbc	hl,bc
		ld	(board_array),hl
		sbc	hl,bc
		ld	(board_array2),hl
		ld	bc,9*19
		sbc	hl,bc
		ld	(pipe_pieces),hl
		ld	bc,PIPE_ARRAY_MAX
		sbc	hl,bc
		ld	(pipe_array),hl
		ld	bc,FILL_ANIMATION_MAX*3
		sbc	hl,bc
		ld	(fill_animation),hl
		ld	bc,FILL_NODES_MAX*5
		sbc	hl,bc
		ld	(fill_nodes),hl
		ld	sp,hl
		; -----------------------------------------------------
		; Engine Initialisation
		; -----------------------------------------------------
		ld	a,_SPC
		call	screen_buffer_wipe	; Initialise eol markers.
		ld	a,1			; Show the splash screen
		ld	(screen_redraw),a	; whilst things are
		call	splash_draw		; being initialised.
		call	screen_buffer_blit
		call	localise
		call	fade_offsets_create
		call	pipe_pieces_create
		ld	e,0			; Initialise the
		call	blink_register		; blink system.
		call	keyb_buffer_reset
		call	event_queue_flush
		ld	a,STATE_QUIT
		call	state_change
		ld	a,STATE_SPLASH
		call	state_change
		ld	a,SUBSTATE_NONE
		call	substate_change
		ld	a,2
		ld	(screen_redraw),a
		ld	a,1			; Default to Play.
		ld	(cursor_options_position),a
		ld	hl,(cursor_options_offsets+2)
		ld	(cursor_options_offset),hl
		xor	a
		ld	(help_page),a
		ld	hl,(FRAMES)
		ld	(demo_start_last),hl	; Reset splash timeout.
		; -----------------------------------------------------
		; Main Loop
		; -----------------------------------------------------
main_top:	call	events_generate
		ld	a,(state_current)
		cp	STATE_SPLASH
		jr	nz,main_not_splash
		; -----------------------------------------------------
		; STATE_SPLASH
		; -----------------------------------------------------
main_splash:	call	event_poll
		jr	c,main_spll0
		ld	bc,(FRAMES)		; No event found so
		ld	hl,(demo_start_last)	; after a short while
		and	a			; force options screen.
		sbc	hl,bc
		ld	bc,(demo_start_delay)
		ld	a,b
		cp	h
		jr	c,main_spll0
		jp	nz,main_update
		ld	a,c
		cp	l
		jp	nc,main_update
main_spll0:	call	blink_unregister	; Any press will
		ld	a,STATE_OPTIONS		; change to the
		call	state_change		; options screen.
		ld	a,SUBSTATE_NONE
		call	substate_change
main_spll1:	ld	a,1+2+4+8
		ld	(screen_redraw),a
		call	options_draw
main_spll2:	call	screen_buffer_fade
		call	event_queue_flush
		ld	hl,(FRAMES)
		ld	(demo_start_last),hl	; Reset demo countdown.
		jp	main_update

main_not_splash:
		cp	STATE_OPTIONS
		jp	nz,main_not_options
		; -----------------------------------------------------
		; STATE_OPTIONS
		; -----------------------------------------------------
main_options:	call	event_poll
		jr	c,main_optl1
		ld	bc,(FRAMES)		; No event found so
		ld	hl,(demo_start_last)	; after a short while
		and	a			; initiate demo mode.
		sbc	hl,bc
		ld	bc,(demo_start_delay)
		ld	a,b
		cp	h
		jr	c,main_optl0
		jp	nz,main_update
		ld	a,c
		cp	l
		jp	nc,main_update
main_optl0:	ld	a,(substate_current)	; But not when setting
		cp	SUBSTATE_SETCTRLS	; the controls.
		jp	z,main_update
		ld	hl,(FRAMES)
		ld	(demo_event_last),hl
		ld	a,STATE_DEMO
		jp	main_opt_sel_plyl0
main_optl1:	jr	z,main_options		; Ignore releases.
		ld	hl,(FRAMES)		; Event found so reset
		ld	(demo_start_last),hl	; demo countdown.
		ld	b,a
		ld	a,(substate_current)
		cp	SUBSTATE_SETCTRLS
		ld	a,b
		jr	nz,main_opt_up
		ld	c,a
		ld	a,8
		ld	(screen_redraw),a
		ld	b,5			; Manage setting 5
		ld	hl,action_up		; unique controls.
main_opt_ctr_setl0:
		ld	a,(controls_set_idx)
		cp	b
		jr	z,main_opt_ctr_setl1
		ld	a,c
		cp	(hl)
		jp	z,main_update		; Ignore duplicates.
		inc	hl
		inc	hl
		djnz	main_opt_ctr_setl0	; 0 won't be reached here.
main_opt_ctr_setl1:
		ld	(hl),c			; Store new control.
		dec	a
		ld	(controls_set_idx),a	; 5,4,3,2,1.
		or	a
		jp	nz,main_update		; Get another control.
		ld	a,SUBSTATE_NONE
		call	substate_change		; We're finished now.
		ld	a,2+4+8
		ld	(screen_redraw),a
		jp	main_update
main_opt_up:	ld	hl,action_up
		cp	(hl)
		jr	nz,main_opt_down
		ld	a,(cursor_options_position)
		or	a
		jr	nz,main_opt_upl0
		ld	a,5
main_opt_upl0:	dec	a
main_opt_upl1:	ld	(cursor_options_position),a
		ld	a,2+4
		ld	(screen_redraw),a
		jp	main_update
main_opt_down:	ld	hl,action_down
		cp	(hl)
		jr	nz,main_opt_leftright
		ld	a,(cursor_options_position)
		cp	4
		jr	nz,main_opt_downl0
		ld	a,-1
main_opt_downl0:
		inc	a
		jr	main_opt_upl1
main_opt_leftright:
		ld	hl,action_left
		cp	(hl)
		jr	z,main_opt_lefrigl0
		ld	hl,action_right
		cp	(hl)
		jr	nz,main_opt_select
main_opt_lefrigl0:
		ld	a,(cursor_options_position)
		or	a
		jr	z,main_opt_downl0
		xor	a
		jr	main_opt_upl1
main_opt_select:
		ld	hl,action_select
		cp	(hl)
		jp	nz,main_update
		ld	a,(cursor_options_position)
		or	a
		jr	nz,main_opt_sel_play
		call	blink_unregister
		ld	a,SUBSTATE_SETCTRLS
		call	substate_change
		ld	a,5
		ld	(controls_set_idx),a
		ld	a,8
		ld	(screen_redraw),a
		jp	main_update
main_opt_sel_play:
		cp	1
		jr	nz,main_opt_sel_save
		ld	a,STATE_GAME
main_opt_sel_plyl0:
		call	state_change
		call	game_new		; Initialise the game.
		xor	a
		ld	(cursor_game_frame),a	; Position the cursor
		ld	a,8*4+3			; mid board.
		ld	(cursor_game_position),a
		ld	a,0xff
		ld	(cursor_panel_position),a
		ld	hl,(FRAMES)
		ld	(cursor_game_last),hl
		ld	a,1+8+32+64
		ld	(screen_redraw),a
		ld	a,1+16
		ld	(screen_redraw+1),a
		call	game_draw
		jp	main_spll2
main_opt_sel_save:
		cp	2
		jr	nz,main_opt_sel_help
		ld	a,STATE_SAVE
		call	state_change
		jp	main_quit
main_opt_sel_help:
		cp	3
		jr	nz,main_opt_sel_quit
main_opt_sel_hlpl0:
		call	blink_unregister
		ld	a,STATE_HELP
		call	state_change
		ld	a,1
		ld	(screen_redraw),a
		call	help_draw
		jp	main_spll2
main_opt_sel_quit:
		ld	a,STATE_QUIT
		call	state_change
		jp	main_quit

main_not_options:
		cp	STATE_HELP
		jr	nz,main_not_help
		; -----------------------------------------------------
		; STATE_HELP
		; -----------------------------------------------------
main_help:	call	event_poll
		jp	nc,main_update
		jr	z,main_help		; Ignore releases.
		ld	hl,action_up
		cp	(hl)
		jr	nz,main_hlp_down
		ld	a,(help_page)
		or	a
		jp	z,main_update
		dec	a
main_hlp_upl0:	ld	(help_page),a
		ld	a,1
		ld	(screen_redraw),a
		jp	main_update
main_hlp_down:	ld	hl,action_down
		cp	(hl)
		jr	nz,main_hlp_select
		ld	a,(help_page)
		inc	a
		cp	HELP_PAGE_MAX
		jr	c,main_hlp_upl0
		jp	main_update
main_hlp_select:
		ld	hl,action_select
		cp	(hl)
		jp	nz,main_update
		ld	a,(state_previous)
		call	state_change
		cp	STATE_OPTIONS
		jp	z,main_spll1		; Back to Options.
		ld	a,1+16+32+64
		ld	(screen_redraw),a
		ld	a,(substate_current)
		cp	SUBSTATE_FILLANIM
		ld	a,1+16
		jr	c,main_hlp_sell0
		or	4
main_hlp_sell0:	ld	(screen_redraw+1),a
		call	game_draw
		jp	main_spll2		; Back to game.

main_not_help:	cp	STATE_HSFILL
		jr	z,main_game
		cp	STATE_GAME
		jp	nz,main_not_game
		; -----------------------------------------------------
		; STATE_GAME, STATE_HSFILL and indirectly STATE_DEMO
		; -----------------------------------------------------
main_game:	call	event_poll
		jp	nc,main_update
		jr	z,main_game		; Ignore releases.

		cond	DEBUG_DEMOREC
		push	af			; When recording a demo
		ld	b,5			; convert the keypress
		ld	c,1			; into an action.
		ld	hl,action_up
main_gam_drl0:	cp	(hl)			;  1   0 is nothing
		jr	z,main_gam_drl1		; 3+4  5 is select
		inc	c			;  2
		inc	hl		
		inc	hl		
		djnz	main_gam_drl0
		ld	c,0
main_gam_drl1:	ld	a,c
		ld	hl,(demo_actions_ptr)
		ld	(hl),a			; Record the action.
		inc	hl
		ld	(demo_actions_ptr),hl	; Update pointer.
		pop	af
		endc

main_gaml0:	ld	b,a
		ld	a,(cursor_panel_position)
		cp	0xff			; Is panel cursor off?
		ld	a,b
		jp	z,main_gam_up
main_gam_pnl_up:
		ld	hl,action_up
		cp	(hl)
		jr	nz,main_gam_pnl_down
		ld	a,(cursor_panel_position)
		or	a
		jr	nz,main_gam_pnl_upl0
		ld	a,5
main_gam_pnl_upl0:
		dec	a
main_gam_pnl_upl1:
		ld	(cursor_panel_position),a
		ld	a,16
		ld	(screen_redraw),a
		jp	main_update
main_gam_pnl_down:
		ld	hl,action_down
		cp	(hl)
		jr	nz,main_gam_pnl_leftright
		ld	a,(cursor_panel_position)
		cp	4
		jr	nz,main_gam_pnl_dnl0
		ld	a,-1
main_gam_pnl_dnl0:
		inc	a
		jr	main_gam_pnl_upl1
main_gam_pnl_leftright:
		ld	hl,action_left
		ld	b,7
		cp	(hl)
		jr	z,main_gam_pnl_lefrigl0
		ld	hl,action_right
		ld	b,0
		cp	(hl)
		jr	z,main_gam_pnl_lefrigl0
		jr	main_gam_pnl_sel
main_gam_pnl_lefrigl0:
		ld	a,(substate_current)	; Don't allow exiting
		cp	SUBSTATE_GAME		; the panel if the game
		jp	nz,main_update		; isn't running.
		ld	a,(cursor_panel_position)
		or	a
		jr	nz,main_gam_pnl_lefrigl2
main_gam_pnl_lefrigl1:
		add	a,b			; Transfer the cursor
		push	af			; to the game board.
		call	blink_unregister
		ld	a,0xff			; Panel cursor is off.
		ld	(cursor_panel_position),a
		pop	af
		jp	main_gam_upl2
main_gam_pnl_lefrigl2:
		cp	4
		jr	nz,main_gam_pnl_lefrigl3
		dec	a			; Now we have 1,2 or 3.
main_gam_pnl_lefrigl3:
		sla	a
		sla	a
		sla	a			; Multiply by 8.
		add	a,4*8
		jr	main_gam_pnl_lefrigl1
main_gam_pnl_sel:
		ld	hl,action_select
		cp	(hl)
		jp	nz,main_update
		ld	a,(cursor_panel_position)
		or	a
		jr	nz,main_gam_pnl_sel_fill
main_gam_pnl_sel_hifill:
		ld	bc,(hiscore)		; If hiscore is zero
		ld	a,b			; then no hiscore
		or	c			; network exists.
		jp	z,main_update
		ld	a,STATE_HSFILL
		call	state_change
		call	game_new		; Reinitialise the game.
		ld	hl,board_arrayhs	; Restore hiscore
		ld	de,(board_array)	; board array
		ld	bc,64
		ldir
		ld	hl,0			; Dump remaining time.
		ld	(countdown_time),hl
		ld	a,1+16+32+64		; Rebuild the screen
		ld	(screen_redraw),a	; now as fill_prepare
		ld	a,1+16			; will overwrite
		ld	(screen_redraw+1),a	; screen_redraw.
		call	game_draw
		call	fill_prepare
		jp	main_update
main_gam_pnl_sel_fill:
		cp	1
		jr	nz,main_gam_pnl_sel_new
		call	fill_prepare
		jp	main_update
main_gam_pnl_sel_new:
		cp	2
		jr	nz,main_gam_pnl_sel_help
		ld	a,STATE_GAME
		call	state_change
		call	game_new		; Reinitialise the game.
		ld	a,1+16+32+64
		ld	(screen_redraw),a
		ld	a,1+16
		ld	(screen_redraw+1),a
		jp	main_update
main_gam_pnl_sel_help:
		cp	3
		jp	z,main_opt_sel_hlpl0
main_gam_pnl_sel_back:
		jp	main_spll0

main_gam_up:	ld	hl,action_up
		cp	(hl)
		jr	nz,main_gam_down
		ld	a,(cursor_game_position)
		cp	8
		jr	nc,main_gam_upl0
		add	a,64
main_gam_upl0:	sub	8
main_gam_upl1:	push	af
		ld	a,4			; Firstly erase the
		ld	(screen_redraw),a	; existing game cursor.
		call	game_draw
		pop	af
main_gam_upl2:	ld	(cursor_game_position),a
		xor	a			; Reset to frame 0 and
		ld	(cursor_game_frame),a	; update frames last
		ld	bc,(FRAMES)		; so that it's visible.
		ld	(cursor_game_last),bc
		ld	a,8			; Draw the game cursor.
		ld	(screen_redraw),a	; at its new location.
		jp	main_update

main_gam_down:	ld	hl,action_down
		cp	(hl)
		jr	nz,main_gam_left
		ld	a,(cursor_game_position)
		cp	56
		jr	c,main_gam_dnl0
		sub	64
main_gam_dnl0:	add	a,8
		jr	main_gam_upl1

main_gam_left:	ld	hl,action_left
		cp	(hl)
		jr	nz,main_gam_right
		ld	a,(cursor_game_position)
		ld	b,a
		and	7			; Calculate the column.
		ld	a,b
		jr	nz,main_gam_lefl4
		srl	a			; Calculate the row.
		srl	a
		srl	a			; Divide by 8.
		jr	nz,main_gam_lefl1
main_gam_lefl0:	ld	(cursor_panel_position),a
		ld	a,4+16			; Erase the game cursor
		ld	(screen_redraw),a	; and show the panel
		call	game_draw		; cursor.
		ld	a,0xff			; Board cursor is off.
		ld	(cursor_game_position),a
		jp	main_update
main_gam_lefl1:	cp	5
		jr	c,main_gam_lefl3
main_gam_lefl2:	sub	4			; Rows 0, 5 to 7 will
		cp	3			; transfer to the panel.
		jr	nz,main_gam_lefl0
		inc	a
		jr	main_gam_lefl0
main_gam_lefl3:	ld	a,b
		add	a,8
main_gam_lefl4:	dec	a
		jr	main_gam_upl1

main_gam_right:	ld	hl,action_right
		cp	(hl)
		jr	nz,main_gam_sel
		ld	a,(cursor_game_position)
		ld	b,a
		and	7			; Calculate the column.
		cp	7
		ld	a,b
		jr	nz,main_gam_rigl3
		srl	a			; Calculate the row.
		srl	a
		srl	a			; Divide by 8.
		jr	z,main_gam_lefl0
		cp	5
		jr	c,main_gam_rigl2
		jr	main_gam_lefl2
main_gam_rigl2:	ld	a,b
		sub	8
main_gam_rigl3:	inc	a
		jp	main_gam_upl1

main_gam_sel:	ld	hl,action_select
		cp	(hl)
		jp	nz,main_update
		ld	hl,(board_array)
		ld	a,(cursor_game_position)
		ld	b,0
		ld	c,a
		add	hl,bc
		ld	a,(hl)
		ld	de,score
		ex	de,hl
		cp	2
		jr	nc,main_gam_sell0	; Space?
		ld	b,10			; +10 points.
		call	bcd_word_add
		jr	main_gam_sell2
main_gam_sell0:	jr	nz,main_gam_sell1	; Pipe-in?
		call	fill_prepare
		jp	main_update
main_gam_sell1:	cp	3			; Pipe-out?
		jp	z,main_update
		ld	b,10			; -10 points.
		call	bcd_word_subtract
main_gam_sell2:	ex	de,hl
		ld	a,(preview_bar_slot0)
		ld	(hl),a
		ld	a,(preview_bar_slot1)
		ld	(preview_bar_slot0),a
		call	pipe_get
		ld	(preview_bar_slot1),a
		ld	a,32+64
		ld	(screen_redraw),a
		call	game_draw
		ld	a,(cursor_game_position)
		jp	main_gam_upl1

main_not_game:	cp	STATE_DEMO
		jp	nz,main_update
		; -----------------------------------------------------
		; STATE_DEMO
		; -----------------------------------------------------
main_demo:	call	event_poll
		jp	nc,main_dmol0
		jr	z,main_demo		; Ignore releases.
		jp	main_spll0
main_dmol0:	ld	bc,(FRAMES)		; If it's time, read
		ld	hl,(demo_event_last)	; another action from
		and	a			; the actions array.
		sbc	hl,bc
		ld	a,(demo_event_delay)
		cp	l
		jp	nc,main_update
		ld	(demo_event_last),bc
		ld	hl,(demo_actions_ptr)
		ld	a,(hl)			; Read next action.
		inc	hl
		ld	(demo_actions_ptr),hl	; Update pointer.
		or	a			; Do nothing?
		jr	z,main_update
		cp	6			; The end?
		jr	nz,main_dmol1
		call	blink_unregister	; Back to Splash.
		ld	a,STATE_SPLASH
		call	state_change
		ld	a,1+2
		ld	(screen_redraw),a
		call	splash_draw
		jp	main_spll2
main_dmol1:	ld	hl,action_up		; 1 to 5 is doubled
		dec	hl			; and then used as an
		dec	hl			; offset into the list
		sla	a			; of actions.
		ld	b,0
		ld	c,a
		add	hl,bc
		ld	a,(hl)			; Inject the action
		jp	main_gaml0		; into the game.

		; -----------------------------------------------------
		; Update
		; -----------------------------------------------------
main_update:	ld	a,(state_current)
		cp	STATE_SPLASH
		jr	nz,main_upd_options
		call	splash_draw
		jr	main_upd_screen
main_upd_options:
		cp	STATE_OPTIONS
		jr	nz,main_upd_help
		call	options_draw
		jr	main_upd_screen
main_upd_help:	cp	STATE_HELP
		jr	nz,main_upd_game
		call	help_draw
		jr	main_upd_screen
main_upd_game:	cp	STATE_DEMO
		jr	z,main_upd_gal0
		cp	STATE_HSFILL
		jr	z,main_upd_gal0
		cp	STATE_GAME
		jr	nz,main_upd_screen
main_upd_gal0:	ld	a,(substate_current)
		cp	SUBSTATE_GAME
		jr	nz,main_upd_gal1
		ld	a,(screen_redraw)	; The countdown time
		or	128			; requires animating.
		ld	(screen_redraw),a
		ld	a,(cursor_game_position)
		cp	0xff			; Is game cursor off?
		jr	z,main_upd_gal1
		ld	a,(screen_redraw)	; The cursor requires
		or	2			; animating when it's
		ld	(screen_redraw),a	; on the board.
		jr	main_upd_gal5
main_upd_gal1:	cp	SUBSTATE_FILL
		jr	nz,main_upd_gal2
		call	fill_network
		jr	main_upd_gal5
main_upd_gal2:	cp	SUBSTATE_REMDEAD
		jr	nz,main_upd_gal3
		call	fill_remdead
		jr	main_upd_gal5
main_upd_gal3:	cp	SUBSTATE_FILLANIM
		jr	nz,main_upd_gal4
		call	fill_animate
		jr	main_upd_gal5
main_upd_gal4:	cp	SUBSTATE_HSEND
		jr	nz,main_upd_gal5
		ld	a,(screen_redraw+1)	; The new hiscore
		or	8			; requires flashing.
		ld	(screen_redraw+1),a
main_upd_gal5:	call	game_draw
		jr	main_upd_screen

main_upd_screen:
		call	dump_debug_info
		call	blink_apply
		call	screen_buffer_blit
		jp	main_top

main_quit:	ld	a,_SPC
		call	screen_buffer_wipe
		call	screen_buffer_blit
		ld	sp,(sp_original)
		ld	a,(state_current)	; Return program state
		ld	b,0			; in bc so that SAVE
		ld	c,a			; can be checked for.
		ret

; *********************************************************************
; Fill Animate                                                        *
; *********************************************************************
; This animates the filling of the pipe network at the unit of water
; level i.e. by the char. Nothing is being calculated here regarding the
; condition of the network as this was done earlier by fill_network.
; 
; The resolution of the board array is 8x8 and the resolution of this
; animated fill is 24x24, so a node will move in units of 3 across a
; single pipe (3 subframes of a pipe frame). Subframe 1 is the central
; unit of a pipe where most of the work is done regarding node creation
; and scoring. Subframe 2 is at the pipe's boundary and subframe 0 is
; the node's first foray into an adjoining pipe. Both of these latter
; subframes require little more than animating.
; 
; The comments down the side explain how this is achieved.

fill_animate:	ld	bc,(FRAMES)		; If it's time, animate
		ld	hl,(fill_animate_last)	; another unit of water
		and	a			; until a leak is found
		sbc	hl,bc			; or no more nodes.
		ld	a,(fill_animate_delay)
		cp	l
		ret	nc
		ld	(fill_animate_last),bc

		ld	hl,fill_animate_frame	; New frame.
		inc	(hl)

		ld	a,(fill_animate_subframe) ; New subframe.
		inc	a
		cp	3
		jr	c,fal10
		xor	a
fal10:		ld	(fill_animate_subframe),a

		ld	hl,(fill_nodes)
		ld	b,FILL_NODES_MAX
		ld	c,0			; Node found flag.
fal31:		push	hl
		push	bc
		ld	a,(hl)			; Get node's direction.
		or	a
		jp	z,fal200
		ld	b,a
		inc	hl
		ld	c,(hl)			; Get last subframe.
		ld	a,(fill_animate_subframe)
		cp	c
		jp	z,fal200		; Filter out new nodes.
		ld	c,a
		ld	(hl),c			; Update subframe.
		inc	hl
		ld	e,(hl)			; Get board_array2 offset.
		inc	hl

		cp	2
		jr	nz,fal50
		; ------------------------
		; Subframe 1 to Subframe 2
		; ------------------------
		; Update node.
		; Animate node.
		; If pipe is leaky then
		;     kill node and stop after current subframe.
		call	fa_node_update
		call	fa_node_animate
		call	fa_node_pipe_get
		and	0x40			; Is it leaky?
		jp	z,fal190
		call	fa_node_kill		; Yes, it's leaky.
		ld	a,1			; Prepare to end fill
		ld	(fill_animate_leak),a	; after current subframe.
		jr	fal200

fal50:		or	a
		jr	nz,fal60
		; ------------------------
		; Subframe 2 to Subframe 0
		; ------------------------
		; Update node.
		; Animate node.
		call	fa_node_update
		call	fa_node_animate
		jr	fal190

fal60:		; ------------------------
		; Subframe 0 to Subframe 1
		; ------------------------
		; Update node.
		; If pipe is scored then
		;     kill node.
		; else
		;     score pipe.
		;     If pipe is not an end-pipe nor a terminator then
		;         animate node.
		;     Kill node.
		;     Create new nodes for all pipe exits excluding
		;     originating direction.
		call	fa_node_update
		call	fa_node_pipe_get
		ld	d,a			; Backup pipe.
		and	0x80			; Is it already scored?
		jr	z,fal62
		call	fa_node_kill		; Yeah, so kill node.
		jr	fal200
fal62:		ld	a,d			; Retrieve pipe.
		or	0x80			; Mark pipe as scored.
		call	fa_node_pipe_set
		ld	a,(state_current)	; Is the hiscore network
		cp	STATE_HSFILL		; being reanimated?
		jr	z,fal63
		push	bc
		push	hl
		ld	hl,score
		ld	b,50			; +50 points.
		call	bcd_word_add
		ld	a,(screen_redraw)	; Flag score for
		or	64			; redisplaying.
		ld	(screen_redraw),a
		pop	hl
		pop	bc
fal63:		ld	a,d			; Retrieve pipe.
		and	0x1f			; Filter out flags.
		cp	3			; Is it the end-pipe?
		jr	z,fal66
		cp	7			; Is it a terminator?
		jr	c,fal64
		cp	11
		jr	c,fal66
fal64:		call	fa_node_animate
fal66:		call	fa_node_kill

		ld	a,d			; Retrieve pipe.
		and	0x1f			; Filter out flags.
		call	pipe_exits_get
		ld	d,a			; Backup exits.
		ld	a,b			; Retrieve direction.
		cp	4			; Reverse originating
		jr	nc,fal68		; direction so that it
		sla	b			; can be removed from
		sla	b			; the exits.
		jr	fal70
fal68:		srl	b
		srl	b
fal70:		ld	a,b
		cpl
		ld	b,a
		ld	a,d			; Retrieve exits.
		and	b			; Remove originating dir.
		ld	d,a			; Update exits.
		ld	b,1
fal72:		ld	a,d			; Retrieve exits.
		and	b			; Isolate direction.
		jr	z,fal74
		call	fa_node_add		; Create a new node.
fal74:		sla	b			; 1,2,4 and finally 8.
		bit	4,b
		jr	z,fal72

fal190:		pop	bc
		inc	c			; At least one node
		push	bc			; exists.

fal200:		pop	bc
		pop	hl
		inc	hl
		inc	hl
		inc	hl
		inc	hl
		inc	hl
		dec	b
		jp	nz,fal31

		ld	a,(screen_redraw+1)	; Flag frame for
		or	2			; drawing.
		ld	(screen_redraw+1),a

		ld	a,c			; End filling if no
		or	a			; more nodes exist.
		jr	z,fal210

		ld	a,(fill_animate_leak)	; End filling if a
		or	a			; leak was detected.
		jr	nz,fal210
		ret

fal210:		ld	a,(state_current)	; Ignore the demo score
		cp	STATE_DEMO		; when calculating the
		jr	z,fal220		; hiscore.
		
		ld	de,(score)		; Is this a hiscore?
		ld	a,d
		cp	0x80			; Don't score negative
		jr	nc,fal220		; numbers (9999 to 8000).
		ld	bc,(hiscore)
		cp	b
		jr	c,fal220
		jr	nz,fal211
		ld	a,e			; Hi bytes are equal,
		cp	c			; so check lo.
		jr	c,fal220
		jr	z,fal220
fal211:		ld	(hiscore),de		; Update hiscore.
		ld	hl,(board_array)	; Copy contents
		ld	de,board_arrayhs	; of board_array to
		ld	bc,64			; board_arrayhs for
		ldir				; user reanimating.

		ld	hl,0			; Prepare to flash
		ld	(hiscore_last),hl	; the new hiscore.
		xor	a
		ld	(hiscore_frame),a
		ld	a,SUBSTATE_HSEND
		jr	fal221

fal220:		ld	a,SUBSTATE_END
fal221:		call	substate_change		; Set new substate.

		cond	DEBUG_ANIMPTR
		ld	hl,(fill_animation_ptr)	; Display the end
		ld	de,(fill_animation)	; of the animation
		and	a			; array.
		sbc	hl,de
		ex	de,hl
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*23+5
		ld	hl,0x0004
		call	hex_write
		endc

		ret

; *********************************************************************
; Fill Animate - Node Add                                             *
; *********************************************************************
; This is used by fill_animate only.
; 
; On entry: a  = direction
;           c  = current subframe
;           e  = board_array2 offset
;           hl is pointing to the screen buffer offset.
;           No registers are modified here.

fa_node_add:	push	hl
		push	de
		push	af
		ld	e,(hl)			; Read screen buffer
		inc	hl			; offset.
		ld	d,(hl)

		ld	hl,(fill_nodes)		; Locate a free slot
fa_nadl10:	ld	a,(hl)			; to create a new node.
		or	a
		jr	z,fa_nadl20
		inc	hl
		inc	hl
		inc	hl
		inc	hl
		inc	hl
		jr	fa_nadl10

fa_nadl20:	pop	af			; Store new node.
		ld	(hl),a			; Direction.
		inc	hl
		ld	(hl),c			; Subframe.
		inc	hl
		inc	hl
		ld	(hl),e			; Screen buffer offset.
		inc	hl
		ld	(hl),d
		dec	hl
		dec	hl
		pop	de
		ld	(hl),e			; board_array2 offset.
		pop	hl
		ret

; *********************************************************************
; Fill Animate - Node Kill                                            *
; *********************************************************************
; This is used by fill_animate only.
; 
; On entry: hl is pointing to the screen buffer offset.
;           No registers are modified here.

fa_node_kill:	dec	hl
		dec	hl
		dec	hl
		ld	(hl),0			; Kill node.
		inc	hl
		inc	hl
		inc	hl
		ret

; *********************************************************************
; Fill Animate - Node Pipe Set                                        *
; *********************************************************************
; This is used by fill_animate only.
; 
; On entry: e = the node's board_array2 offset
;           a = the node's pipe
;           No registers are modified here.

fa_node_pipe_set:		
		push	de
		push	hl
		ld	hl,(board_array2)
		ld	d,0
		add	hl,de
		ld	(hl),a			; Set pipe.
		pop	hl
		pop	de
		ret

; *********************************************************************
; Fill Animate - Node Pipe Get                                        *
; *********************************************************************
; This is used by fill_animate only.
; 
; On entry: e = the node's board_array2 offset.
;  On exit: a = the node's pipe (including flags) from board_array2
;           No registers are modified here.

fa_node_pipe_get:
		push	de
		push	hl
		ld	hl,(board_array2)
		ld	d,0
		add	hl,de
		ld	a,(hl)			; Get pipe.
		pop	hl
		pop	de
		ret

; *********************************************************************
; Fill Animate - Node Animate                                         *
; *********************************************************************
; This is used by fill_animate only.
; 
; On entry: hl is pointing to the screen buffer offset.
;           No registers are modified here.

fa_node_animate:
		push	de
		push	hl
		ld	e,(hl)			; Read screen buffer
		inc	hl			; offset from fill_nodes.
		ld	d,(hl)

		ld	hl,(fill_animation_ptr)	; Retrieve pointer.
		ld	a,(fill_animate_frame)
		ld	(hl),a			; Store frame.
		inc	hl
		ld	(hl),e
		inc	hl			; Store screen buffer
		ld	(hl),d			; offset.
		inc	hl
		ld	(hl),0			; Current end of list.
		ld	(fill_animation_ptr),hl	; Update pointer.

		pop	hl
		pop	de
		ret

; *********************************************************************
; Fill Animate - Node Update                                          *
; *********************************************************************
; This is used by fill_animate only.
; 
; On entry: b  = direction
;           c  = current subframe
;           e  = board_array2 offset
;           hl is pointing to the screen buffer offset.
;           Only register a is modified here although e will be
;           updated on subframe 0.

fa_node_update: ld	a,c			; Moving to subframe 0
		or	a			; requires that the
		jr	nz,fa_nul20		; board_array2 offset
		ld	a,b			; is updated.
		cp	1
		jr	nz,fa_nul10
		ld	a,e
		sub	8
		jr	fa_nul13
fa_nul10:	cp	2
		jr	nz,fa_nul11
		ld	a,e
		inc	a
		jr	fa_nul13
fa_nul11:	cp	4
		jr	nz,fa_nul12
		ld	a,e
		add	a,8
		jr	fa_nul13
fa_nul12:	ld	a,e
		dec	a
fa_nul13:	ld	e,a
		dec	hl			; Update
		ld	(hl),e			; board_array2 offset.
		inc	hl

fa_nul20:	push	de			; Move the node's
		push	hl			; screen buffer offset
		ld	e,(hl)			; to its new location.
		inc	hl
		ld	d,(hl)
		ld	a,b
		cp	1
		jr	nz,fa_nul21
		ld	hl,-33
		jr	fa_nul24
fa_nul21:	cp	2
		jr	nz,fa_nul22
		ld	hl,1
		jr	fa_nul24
fa_nul22:	cp	4
		jr	nz,fa_nul23
		ld	hl,33
		jr	fa_nul24
fa_nul23:	ld	hl,-1
fa_nul24:	add	hl,de
		ex	de,hl
		pop	hl
		ld	(hl),e
		inc	hl			; Update screen buffer
		ld	(hl),d			; offset.
		dec	hl
		pop	de
		ret

; *********************************************************************
; Fill Remove Dead Pipes                                              *
; *********************************************************************
; This removes unvisited (dead pipes) from the board array, its copy
; board_array2 and animatedly from the screen buffer.
; 
; The comments down the side explain how this is achieved.

fill_remdead:	ld	bc,(FRAMES)		; If it's time, remove
		ld	hl,(fill_remdead_last)	; another dead pipe
		and	a			; until all are gone.
		sbc	hl,bc
		ld	a,(fill_remdead_delay)
		cp	l
		ret	nc
		ld	(fill_remdead_last),bc

		ld	de,0
frdl0:		ld	hl,(board_array2)
		add	hl,de
		ld	a,(hl)
		cp	4			; Filter out spaces and
		jr	c,frdl20		; an unvisited end-pipe.
		and	0x20			; Was it visited?
		jr	nz,frdl20		; Yes, so leave it.

		ld	b,e
		srl	b
		srl	b			; Divide board array
		srl	b			; index by 8.
		ld	a,e			; Calculate type of
		bit	0,b			; space to erase with.
		jr	nz,frdl1
		inc	a
frdl1:		and	1

		ld	(hl),a			; Erase the dead pipe
		ld	hl,(board_array)	; both in the board
		add	hl,de			; array and the copy.
		ld	(hl),a

		ld	c,a			; Erase the dead pipe
		ld	a,e			; within the screen
		and	7			; buffer too since
		ld	e,a			; redrawing the entire
		sla	a			; board for one pipe is
		add	a,e			; not very efficient.
		ld	d,0
		ld	e,a			; Normally I keep all
		ld	hl,(screen_buffer)	; the related drawing
		add	hl,de			; routines together
		ld	de,8			; but I can't put this
		add	hl,de			; in game_draw as it
		ld	a,b			; won't know the offset.
		or	a
		jr	z,frdl11
		ld	de,99
frdl10:		add	hl,de
		djnz	frdl10
frdl11:		ld	a,c
		ld	b,h
		ld	c,l
		call	pipe_draw

		ld	hl,score
		ld	b,10			; -10 points.
		call	bcd_word_subtract
		ld	a,(screen_redraw)	; Flag score for
		or	64			; redisplaying.
		ld	(screen_redraw),a
		ret
		
frdl20:		inc	e
		ld	a,e
		cp	64
		jr	c,frdl0

		; The code below initialises fill_animate which is the
		; next substate following this.

		ld	hl,(fill_nodes)		; Wipe the nodes
		ld	b,FILL_NODES_MAX	; list in preparation
		xor	a			; for fill_animate.
frdl30:		ld	(hl),a
		inc	hl
		inc	hl
		inc	hl
		inc	hl
		inc	hl
		djnz	frdl30

		ld	hl,(board_array2)	; Locate start-pipe.
		push	hl
		dec	hl
frdl40:		inc	hl
		ld	a,(hl)
		ld	b,a
		and	0x1f			; Filter out flags.
		cp	2
		jr	nz,frdl40
		ld	a,b
		or	0x80			; Mark as scored.
		ld	(hl),a
		pop	bc
		and	a
		sbc	hl,bc			; Make de an offset
		ex	de,hl			; into board_array2.

		ld	hl,0			; Create first node.
		ld	a,e
		and	7
		ld	b,a			; The node requires
		sla	a			; converting from a
		add	a,b			; board_array2 offset
		ld	b,0			; into a screen_buffer
		ld	c,a			; offset.
		add	hl,bc
		ld	a,e
		srl	a
		srl	a
		srl	a
		ld	bc,99
frdl50:		or	a
		jr	z,frdl51
		add	hl,bc
		dec	a
		jr	frdl50
frdl51:		ld	bc,33+9
		add	hl,bc
		ld	a,e
		ex	de,hl
		ld	hl,(fill_nodes)		; Store the node.
		ld	(hl),8			; Direction.
		inc	hl
		ld	(hl),1			; Subframe.
		inc	hl
		ld	(hl),a			; board_array2 offset.
		inc	hl
		ld	(hl),e			; Screen buffer offset.
		inc	hl
		ld	(hl),d

		ld	hl,(fill_animation)	; Set-up the animation
		ld	(fill_animation_ptr),hl ; list (there's no
		ld	(hl),0			; initial animation).

		xor	a
		ld	(fill_animate_leak),a

		ld	a,1
		ld	(fill_animate_frame),a
		ld	(fill_animate_subframe),a

		ld	a,(state_current)	; Is the hiscore network
		cp	STATE_HSFILL		; being reanimated?
		jr	z,frdl62
		ld	hl,score		; Score the start-pipe.
		ld	b,50			; +50 points.
		call	bcd_word_add
		ld	a,(screen_redraw)	; Flag score for
		or	64			; redisplaying.
		ld	(screen_redraw),a

frdl62:		ld	hl,0			; Prepare to animate
		ld	(fill_animate_last),hl	; the filling of the
		ld	a,SUBSTATE_FILLANIM	; pipe network.
		call	substate_change
		ret

; *********************************************************************
; Fill Network                                                        *
; *********************************************************************
; This fills the network at the board array level i.e. by the pipe.
; It's done instantly non-visually to mark the pipes as visited and/or
; leaky within the board array copy.
; 
; The comments down the side explain how this is achieved.

fill_network:	ld	hl,(board_array)	; Make a copy of the
		ld	de,(board_array2)	; board array.
		ld	bc,8*8
		ldir

		ld	hl,(fill_nodes)		; Wipe the nodes
		ld	b,FILL_NODES_MAX	; list.
		xor	a
fnl10:		ld	(hl),a
		inc	hl
		inc	hl
		djnz	fnl10

		ld	hl,(board_array2)	; Locate start-pipe
		push	hl			; down the right-hand
		ld	bc,7			; side of the board.
		add	hl,bc
		inc	bc
fnl20:		ld	a,(hl)
		cp	2
		jr	z,fnl21
		add	hl,bc
		jr	fnl20
fnl21:		or	0x20			; Mark as visited.
		ld	(hl),a
		pop	bc
		and	a
		sbc	hl,bc			; Make de an offset
		ex	de,hl			; into board_array2.

		ld	hl,(fill_nodes)		; Create first node.
		ld	(hl),8			;    N=1
		inc	hl			; W=8   E=2
		ld	(hl),e			;    S=4

fnl30:		ld	hl,(fill_nodes)		; Process nodes.
		ld	b,FILL_NODES_MAX
		ld	c,0			; Node found flag.
fnl31:		push	hl
		push	bc
		ld	a,(hl)			; Get node's direction.
		ld	(hl),0			; Kill it since it's
		inc	hl			; not needed anymore.
		ld	e,(hl)
		inc	hl
		or	a
		jp	z,fnl200

		ld	d,a			; Backup direction.

		cp	1			; Going north?
		jr	nz,fnl50
		ld	a,e			; Is it adjoining the
		cp	8			; board edge?
		jr	nc,fnl100
		jr	fnl80
fnl50:		cp	2			; Going east?
		jr	nz,fnl60
		ld	a,e			; Is it adjoining the
		and	7			; board edge?
		cp	7
		jr	nz,fnl100
		jr	fnl80
fnl60:		cp	4			; Going south?
		jr	nz,fnl70
		ld	a,e			; Is it adjoining the
		cp	56			; board edge?
		jr	c,fnl100
		jr	fnl80
fnl70:		ld	a,e			; Going west.
		and	7			; Is it adjoining the
		jr	nz,fnl100		; board edge?
fnl80:		ld	hl,(board_array2)
		ld	b,0
		ld	c,e
		add	hl,bc
		ld	a,(hl)
		or	0x40			; Mark as leaky.
		ld	(hl),a
		jp	fnl200			; Node is finished with.

fnl100:		ld	a,d			; Retrieve direction.

		ld	hl,(board_array2)
		ld	b,0
		ld	c,e
		add	hl,bc
		ld	bc,-8
		cp	1			; Get northerly pipe?
		jr	z,fnl110
		ld	bc,1
		cp	2			; Get easterly pipe?
		jr	z,fnl110
		ld	bc,8
		cp	4			; Get southerly pipe?
		jr	z,fnl110
		ld	bc,-1			; Get westerly pipe.
fnl110:		add	hl,bc

		ld	a,(hl)			; Get target pipe.
		and	0x1f			; Filter out flags.
		call	pipe_exits_get
		ld	b,a			; Backup exits.

		ld	a,d			; Retrieve direction.

		cp	4			; Reverse source
		jr	nc,fnl120		; direction so that the
		sla	d			; connection with the
		sla	d			; target pipe can be
		jr	fnl121			; validated.
fnl120:		srl	d
		srl	d
fnl121:		ld	a,b			; Retrieve exits.
		and	d			; Is source dir there?
		jr	z,fnl80			; No, so mark as leaky.
		ld	a,d
		cpl
		ld	d,a
		ld	a,b			; Retrieve exits.
		and	d			; Remove source direction.
		ld	b,a			; Update exits.

		ld	a,(hl)			; Get target pipe.
		ld	d,a
		and	0x20			; Already visited?
		jr	nz,fnl200		; Yeah, so move on.
		ld	a,d
		or	0x20			; Mark as visited.
		ld	(hl),a

		ld	de,(board_array2)
		and	a
		sbc	hl,de
		ex	de,hl			; e is now target offset.
		
		ld	hl,(fill_nodes)		; Locate a free slot
		ld	d,1			; to create a new node.
fnl130:		ld	a,(hl)
		or	a
		jr	z,fnl131
		inc	hl
		inc	hl
		jr	fnl130

fnl131:		ld	a,b			; Retrieve exits.
		and	d			; Isolate direction.
		jr	z,fnl132
		ld	(hl),a			; Store new node.
		inc	hl
		ld	(hl),e
		inc	hl
fnl132:		sla	d			; 1,2,4 and finally 8.
		bit	4,d
		jr	z,fnl130

		pop	bc
		inc	c			; At least one node
		push	bc			; exists.

fnl200:		pop	bc
		pop	hl
		inc	hl
		inc	hl
		dec	b
		jp	nz,fnl31
		ld	a,c
		or	a
		jp	nz,fnl30

		ld	hl,0			; Prepare to remove
		ld	(fill_remdead_last),hl	; any dead pipes.
		ld	a,SUBSTATE_REMDEAD	; Set new substate.
		call	substate_change
		ret

; *********************************************************************
; Pipe Exits Get                                                      *
; *********************************************************************
; This returns the exits for a particular pipe piece.
; 
; On entry: a = pipe piece
;  On exit: a = exits 1=N,2=E,4=S,8=W so a cross would be 0x0f
;           Only register a is modified here.

pipe_exits_get:	push	bc
		push	hl
		ld	hl,pipe_exits_data
		ld	b,0
		ld	c,a
		add	hl,bc
		ld	a,(hl)
		pop	hl
		pop	bc
		ret

; *********************************************************************
; Fill Prepare                                                        *
; *********************************************************************
; This prepares the interface for SUBSTATE_FILL and beyond.

fill_prepare:	ld	a,(substate_current)
		cp	SUBSTATE_GAME
		ret	nz

		ld	a,(cursor_panel_position) ; Is the panel cursor
		cp	0xff			; already visible?
		jr	nz,fpl10
		ld	a,4			; Erase the game cursor.
		ld	(screen_redraw),a
		call	game_draw
		ld	a,0xff			; Turn game cursor off.
		ld	(cursor_game_position),a
		ld	a,1			; Turn panel cursor on
		ld	(cursor_panel_position),a ; and position on Fill.

fpl10:		ld	hl,countdown_time+1	; Is the countdown time
		ld	a,(hl)			; zero?
		dec	hl
		or	(hl)
		jr	z,fpl20
		ld	b,1			; For every second
		call	bcd_word_subtract	; remaining add 5 to
		ld	hl,score		; the score.
		ld	b,5
		call	bcd_word_add
		jr	fpl10

fpl20:		ld	a,16+64			; Show the panel cursor,
		ld	(screen_redraw),a	; redisplay the score
		ld	a,1			; and countdown time.
		ld	(screen_redraw+1),a
		call	game_draw

		ld	a,SUBSTATE_FILL		; Set new substate.
		call	substate_change
		ret

; *********************************************************************
; Game New                                                            *
; *********************************************************************
; This reinitialises everything generally required to start a new game.

game_new:	call	blink_unregister
		ld	a,SUBSTATE_GAME
		call	substate_change
		ld	a,(FRAMES)		; Initialise random
		ld 	(rnd_seed),a		; seed from frames-low.
		call	board_array_reset
		ld	a,PIPE_ARRAY_MAX	; Force pipe array
		ld	(pipe_array_idx),a	; refill.
		call	pipe_get
		ld	(preview_bar_slot0),a
		call	pipe_get
		ld	(preview_bar_slot1),a
		ld	hl,0
		ld	(score),hl
		ld	hl,COUNTDOWN_MAX
		ld	(countdown_time),hl
		ld	hl,(FRAMES)
		ld	(countdown_last),hl
		ld	hl,demo_actions
		ld	(demo_actions_ptr),hl
		ret

; *********************************************************************
; Blink Unregister                                                    *
; *********************************************************************
; This will restore a previously registered blink to its original state
; and then unregister it.

blink_unregister:
		ld	a,(blink_height)
		or	a
		jr	z,bul0			; [Re]initialise only.
		ld	a,(blink_count)
		and	1
		jr	z,bul0			; No need to restore.
		xor	a			; Apply one more initial
		ld	(blink_count),a		; blink to reset to
		call	blink_apply		; original state.
bul0:		ld	e,0			; [Re]initialise the
		call	blink_register		; blink system.
		ret

; *********************************************************************
; Blink Register                                                      *
; *********************************************************************
; This registers an area of the screen buffer for blinking and then
; applies the initial blink. Subsequent blinks are applied within the
; main loop just before blitting the contents of the screen buffer.
; Passing zero as the height will [re]initialise the blink system.
; 
; On entry: bc = destination offset into screen buffer
;           d  = width
;           e  = height (0 to [re]initialise the blink system)

blink_register:	ld	(blink_offset),bc
		ld	(blink_height),de
		xor	a
		ld	(blink_count),a
		ld	(blink_last),a
		ld	(blink_last+1),a
		call	blink_apply
		ret
		
; *********************************************************************
; Blink Apply                                                         *
; *********************************************************************
; This blinks an area of the screen buffer that's been previously
; registered via blink_register. Since newly registered blinks have
; their initial blink automatically applied, it is not necessary for
; this to be called directly other than from the main loop.

blink_apply:	ld	a,(blink_height)	; If height is 0 then
		or	a			; nothing's registered.
		ret	z

		ld	bc,(FRAMES)
		ld	a,(blink_count)		; Don't wait to apply
		or	a			; the initial blink
		jr	z,bal1			; when blink_count = 0.
		ld	hl,(blink_last)
		and	a			; Clear carry flag.
		sbc	hl,bc
		ld	a,(blink_delay)
		cp	l			; Is it time to blink?
		ret	nc			; Not yet.

bal1:		ld	(blink_last),bc		; Store FRAMES.
		ld	hl,(screen_buffer)	; Apply the blink.
		ld	bc,(blink_offset)
		add	hl,bc
		ld	a,(blink_height)
		ld	b,a
bal2:		ld	c,b
		ld	d,h
		ld	e,l
		ld	a,(blink_width)
		ld	b,a
bal3:		ld	a,(hl)
		xor	0x80			; Toggle inverse video.
		ld	(hl),a
		inc	hl
		djnz	bal3
		ld	hl,33
		add	hl,de
		ld	b,c
		djnz	bal2
		ld	hl,blink_count
		inc	(hl)
		ret

; *********************************************************************
; Event Poll                                                          *
; *********************************************************************
; On exit: carry is set if event found with
;            a = char (shift is 0xe9)
;            zero is set if event is a release
;          carry is clear if not found

event_poll:	ld	a,(event_queue_start)
		ld	c,a
		ld	a,(event_queue_end)
		cp	c
		jr	nz,epl0
		and	a			; Clear carry flag.
		ret

epl0:		ld	b,0
		ld	hl,(event_queue)
		add	hl,bc
		ld	d,(hl)			; Retrieve char code.
		inc	hl
		ld	e,(hl)			; Retrieve pressed state.

		ld	a,c
		add	a,2			; Increment the queue's
		cp	EVENTS_MAX*2		; start pointer.
		jr	nz,epl1			; Wrap it back to the
		xor	a			; top when required.
epl1:		ld	(event_queue_start),a

		ld	a,e			; If a release then this
		or	a			; will set the zero flag.
		ld	a,d
		scf
		ret

; *********************************************************************
; Events Generate                                                     *
; *********************************************************************
; This compares the contents of keyb_buffer_old to keyb_buffer and
; creates pressed and released events which are added to the event
; queue. It will repeat press events for actions flagged as repeatable.

events_generate:
		call	keyb_buffer_update

		ld	hl,(keyb_buffer)
		ld	de,(keyb_buffer_old)
		ld	b,40
egl0:		ld	a,(hl)
		sla	a
		ex	de,hl
		or	(hl)
		ex	de,hl
		or	a		; No activity at all.
		jr	z,egl11
		push	bc
		push	de
		push	hl

		ld	e,a
		ld	hl,0x007d	; Convert the scancode to a
		ld	a,40		; char using the ROM's
		sub	b		; K-UNSHIFT table.
		ld	b,0
		ld	c,a
		add	hl,bc		; Shift will result in being
		ld	d,(hl)		; 0xe9 (DIM) which will suffice.

		ld	a,e
		cp	1		; Is it a new release?
		jr	z,egl9
		cp	2		; Is it a new press?
		jr	nz,egl6
		ld	hl,(event_repeat_delay_master)
		ld	(event_repeat_delay),hl
		ld	hl,(FRAMES)
		ld	(event_repeat_last),hl
		jr	egl9
egl6:		ld	b,5		; It's a repeat press and we
		ld	hl,action_up	; need to see if it's a char
egl7:		ld	a,(hl)		; that is a repeatable action.
		inc	hl
		cp	d
		jr	nz,egl8		; It's not an action.
		ld	a,(hl)
		or	a
		jr	z,egl8		; It's not repeatable.
		push	bc
		push	hl
		ld	bc,(FRAMES)
		ld	hl,(event_repeat_last)
		and	a		; Clear carry flag.
		sbc	hl,bc
		ld	a,(event_repeat_delay)
		cp	l		; Is it time to repeat?
		pop	hl
		pop	bc
		jr	nc,egl8		; Not yet.
		ld	hl,(event_repeat_interval_master)
		ld	(event_repeat_delay),hl
		ld	hl,(FRAMES)
		ld	(event_repeat_last),hl
		dec	e		; 3 to 2.
		jr	egl9
egl8:		inc	hl
		djnz	egl7
		jr	egl10
egl9:		dec	e		; 0 = released, 1 = pressed.
		call	event_push

egl10:		pop	hl
		pop	de
		pop	bc
egl11:		inc	hl
		inc	de
		djnz	egl0
		ret

; *********************************************************************
; Event Push                                                          *
; *********************************************************************
; This pushes an event onto the event queue.
; 
; On entry: d = char code 0 to 0xff
;           e = state 0 to 0xff (0 = released, 1 = pressed)

event_push:	ld	a,(event_queue_end) 	; Add the event to the
		ld	b,0			; end of the queue.
		ld	c,a
		ld	hl,(event_queue)
		add	hl,bc
		ld	(hl),d			; Store the char code.
		inc	hl
		ld	(hl),e			; Store pressed state.

		ld	a,(event_queue_end)	; Increment the queue's
		add	a,2			; end pointer.
		cp	EVENTS_MAX*2
		jr	nz,evpul0		; Wrap it back to the
		xor	a			; top when required.
evpul0:		ld	(event_queue_end),a

		ld	b,a			; If the end meets the
		ld	a,(event_queue_start)	; start then move the
		cp	b			; start forward which
		jr	nz,evpul2		; will erase the event
		add	a,2			; that was there.
		cp	EVENTS_MAX*2
		jr	nz,evpul1		; Wrap it back to the
		xor	a			; top when required.
evpul1:		ld	(event_queue_start),a

evpul2:		ret

; *********************************************************************
; Event Queue Flush                                                   *
; *********************************************************************
; This [re]initialises the linear event buffer which also flushes it
; of any events.

event_queue_flush:
		xor	a
		ld	(event_queue_start),a
		ld	(event_queue_end),a
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
; |      |---------------|      |
; |      |     Bits      |      |
; |      | 01234   43210 |      |
; +-----------------------------+
; | F7FE | 12345 | 67890 | EFFE |
; | FBFE | QWERT | YUIOP | DFFE | ^ = Shift
; | FDFE | ASDFG | HJKL\ | BFFE | \ = Newline
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

		ld	hl,(keyb_buffer)
		ld	bc,0xfefe
kbul0:		in	a,(c)			; Read the port.
		cpl				; Invert the bits.
		ld	e,b
		ld	b,5
kbul1:		sra	a			; Isolate the bit and
		ld	d,0			; make it a byte.
		rl	d
		ld	(hl),d			; Store 0 for released,
		inc	hl			; 1 for pressed within
		djnz	kbul1			; the keyb_buffer.
		ld	b,e
		rlc	b			; fe becomes fd, fb, f7,
		ld	a,b			; ef, df, bf, 7f and then
		cp	0xfe			; back to fe.
		jr	nz,kbul0
		ret

; *********************************************************************
; Keyboard Buffer Reset                                               *
; *********************************************************************
; This resets both keyb_buffer and keyb_buffer_old to zero (released).

keyb_buffer_reset:
		ld	hl,(keyb_buffer)
		ld	de,(keyb_buffer_old)
		ld	b,40
		xor	a
kbrl0:		ld	(hl),a
		inc	hl
		ld	(de),a
		inc	de
		djnz	kbrl0
		ret

; *********************************************************************
; Board Array Reset                                                   *
; *********************************************************************
; This resets the board array to something resembling a chequerboard
; with the entry and exit pipe pieces randomnly set.

board_array_reset:
		ld	hl,(board_array)
		ld	a,1
		ld	c,0
		ld	b,8
barl0:		push	bc
		ld	b,4
barl1:		ld	(hl),a
		inc	hl
		ld	(hl),c
		inc	hl
		djnz	barl1
		pop	bc
		ld	d,a
		ld	a,c
		ld	c,d
		djnz	barl0

		ld	a,14
barl2:		ld	d,a

		cond	DEBUG_DEMOREC
		jr	barl3
		endc

		ld	a,(state_current)	; For the demo use
		cp	STATE_DEMO		; predetermined offsets.
		jr	nz,barl4
barl3:		ld	a,100
		add	a,d
		add	a,d
		jr	barl5
barl4:		call	get_random_number	; 0 to 255.
barl5:		srl	a
		srl	a		; Now we have the positions
		and	0x38		; down the board left-hand side.
		ld	hl,(board_array)
		ld	b,0
		ld	c,a
		add	hl,bc
		ld	a,d
		cp	14
		jr	nz,barl6
		ld	e,3		; Exit pipe.
		jr	barl7
barl6:		ld	bc,7		; Right-hand side positions.
		add	hl,bc
		ld	e,2		; Entry pipe.
barl7:		ld	(hl),e
		sub	7
		jr	nz,barl2

		ret

; *********************************************************************
; State Change                                                        *
; *********************************************************************
; On entry: a = the new program state (the previous state is recorded)

state_change:	ld	b,a
		ld	a,(state_current)
		ld	(state_previous),a
		ld	a,b		
		ld	(state_current),a
		ret

; *********************************************************************
; Substate Change                                                     *
; *********************************************************************
; On entry: a = the new program substate

substate_change:
		ld	(substate_current),a
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

splash_draw:	ld	a,(screen_redraw)
		or	a
		ret	z

		and	1
		jr	z,sdl10
		ld	a,_SPC
		call	screen_buffer_wipe
		ld	a,WRITE_TO_SCRBUF
		ld	bc,1
		ld	de,pipepanic_data
		call	string_write
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*11+1
		ld	de,splash_logo_data
		call	string_write

sdl10:		ld	a,(screen_redraw)
		and	2
		jr	z,sdl20
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*23+10
		ld	de,txt_press_a_key
		call	string_write
		ld	bc,33*23+9
		ld	de,0x0d01
		call	blink_register

sdl20:		

screen_redraw_reset:
		xor	a
		ld	(screen_redraw),a
		ld	(screen_redraw+1),a
		ret

; *********************************************************************
; Options Draw                                                        *
; *********************************************************************
; This draws the options screen to the screen buffer.

options_draw:	ld	a,(screen_redraw)
		or	a
		ret	z

		and	1
		jr	z,odl10
		ld	a,_SPC
		call	screen_buffer_wipe
		ld	a,WRITE_TO_SCRBUF
		ld	bc,1
		ld	de,pipepanic_data
		call	string_write
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*9+25
		ld	de,version_data
		call	string_write
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*12+6
		ld	de,options_data
		call	string_write
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*12+24
		ld	de,options_menu_data
		call	string_write
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*21+4
		ld	de,options_copyright_data
		call	string_write

odl10:		ld	a,(screen_redraw)
		and	2
		jr	z,odl20
		call	blink_unregister
		ld	bc,(cursor_options_offset)
		ld	hl,(screen_buffer)	; Erase the cursor at
		add	hl,bc			; its current offset.
		ld	(hl),_SPC
		ld	a,WRITE_TO_SCRBUF	; Erase both frames.
		ld	bc,33*11+3
		ld	de,options_frame_nofocus_data
		call	string_write
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*11+21
		ld	de,options_menu_frame_nofocus_data
		call	string_write

odl20:		ld	a,(screen_redraw)
		and	4
		jr	z,odl30
		ld	hl,cursor_options_offsets
		ld	a,(cursor_options_position)
		ld	b,0
		ld	c,a			; Calculate the cursor's
		add	hl,bc			; new offset from its
		add	hl,bc			; updated position
		ld	c,(hl)			; and draw it there.
		inc	hl
		ld	b,(hl)
		ld	(cursor_options_offset),bc
		ld	hl,(screen_buffer)
		add	hl,bc
		ld	(hl),_GTH
		ld	de,0x0101
		call	blink_register
		ld	a,(cursor_options_position)
		or	a			; Draw a frame around
		jr	nz,odl21		; the area currently
		ld	bc,33*11+3		; occupied by the cursor.
		ld	de,options_frame_focus_data
		jr	odl22
odl21:		ld	bc,33*11+21
		ld	de,options_menu_frame_focus_data
odl22:		ld	a,WRITE_TO_SCRBUF
		call	string_write

odl30:		ld	a,(screen_redraw)
		and	8
		jr	z,odl40
		ld	de,action_up		; Draw the control set.
		ld	hl,33*14+15
		ld	b,5
odl31:		push	bc
		push	de
		push	hl
		push	bc
		ld	bc,(screen_buffer)
		add	hl,bc
		ld	a,(de)
		cp	0xe9			; Shift?
		jr	nz,odl33
		ld	(hl),_S
		inc	hl
		ld	(hl),_H
		inc	hl
		ld	(hl),_F
odl32:		ld	de,0x0301
		jr	odl36
odl33:		cp	_SPC			; Space?
		jr	nz,odl34
		ld	(hl),_S
		inc	hl
		ld	(hl),_P
		inc	hl
		ld	(hl),_C
		jr	odl32
odl34:		cp	_NL			; Newline?
		jr	nz,odl35
		ld	(hl),_N
		inc	hl
		ld	(hl),_L
		inc	hl
		ld	(hl),_SPC
		ld	de,0x0201
		jr	odl36
odl35:		ld	(hl),a			; Printable char.
		inc	hl
		ld	(hl),_SPC
		inc	hl
		ld	(hl),_SPC
		ld	de,0x0101
odl36:		pop	bc
		ld	a,(controls_set_idx)	; This will be 0 if
		cp	b			; SUBSTATE_SETCTRLS is
		jr	nz,odl37		; off and so blink 
		pop	bc			; won't be applied.
		push	bc
		call	blink_register
odl37:		pop	hl
		ld	bc,33
		add	hl,bc
		pop	de
		inc	de
		inc	de
		pop	bc
		djnz	odl31

odl40:		
		jp	screen_redraw_reset

; *********************************************************************
; Help Draw                                                           *
; *********************************************************************
; This draws the help screen to the screen buffer.

help_draw:	ld	a,(screen_redraw)
		or	a
		ret	z

		and	1
		jr	z,hdl10
		ld	a,_SPC
		call	screen_buffer_wipe
		ld	a,(help_page)
		sla	a
		ld	b,0
		ld	c,a
		ld	hl,help_page_offsets
		add	hl,bc
		ld	e,(hl)
		inc	hl
		ld	d,(hl)
		ld	a,WRITE_TO_SCRBUF
		ld	bc,0
		call	string_write

hdl10:
		jp	screen_redraw_reset

; *********************************************************************
; Game Draw                                                           *
; *********************************************************************
; This draws the game screen to the screen buffer.

game_draw:	ld	a,(screen_redraw)
		ld	b,a
		ld	a,(screen_redraw+1)
		or	b
		ld	a,b
		ret	z

		and	1
		jr	z,gdl10
		ld	a,_SPC
		call	screen_buffer_wipe
		ld	a,WRITE_TO_SCRBUF
		ld	bc,0
		ld	de,panel_data
		call	string_write		; Draw the panel.
		call	board_draw		; Draw the board.

gdl10:		ld	a,(screen_redraw)
		and	2
		jr	z,gdl20
		ld	bc,(FRAMES)		; If it's time, flag
		ld	hl,(cursor_game_last)	; the game cursor for
		and	a			; animating.
		sbc	hl,bc
		ld	a,(cursor_game_delay)
		cp	l
		jr	nc,gdl20
		ld	(cursor_game_last),bc
		ld	a,(cursor_game_frame)
		xor	1			; Flip the animation
		ld	(cursor_game_frame),a	; frame.
		ld	a,(screen_redraw)
		or	4+8
		ld	(screen_redraw),a

gdl20:		ld	a,(screen_redraw)
		and	4
		jr	z,gdl30
		ld	a,(cursor_game_position) ; Erase the game
		ld	b,0			; cursor by replacing
		ld	c,a			; the pipe piece at
		ld	hl,(board_array)	; its current offset.
		add	hl,bc
		ld	a,(hl)
		ld	hl,(screen_buffer)
		ld	bc,(cursor_game_offset)
		add	hl,bc
		ld	b,h
		ld	c,l
		call	pipe_draw

gdl30:		ld	a,(screen_redraw)
		and	8
		jr	z,gdl40
		ld	a,(cursor_game_position) ; Draw the game cursor.
		srl	a
		srl	a
		srl	a			; Quotient of /8.
		ld	hl,0
		or	a
		jr	z,gdl32
		ld	b,a			; Update the cursor's
		ld	de,33*3			; offset based upon the
gdl31:		add	hl,de			; current value of
		djnz	gdl31			; cursor_game_position.
gdl32:		ld	a,(cursor_game_position)
		and	7			; Remainder of /8.
		ld	b,a
		sla	a
		add	a,b			; Multiplied by 3.
		add	a,8			; Board screen offset.
		ld	b,0
		ld	c,a
		add	hl,bc
		ld	(cursor_game_offset),hl
		ld	a,(cursor_game_frame)
		or	a			; Draw on frame 0 only
		jr	nz,gdl40		; so that it flashes.
		ld	bc,(screen_buffer)
		add	hl,bc
		ld	(hl),0x01
		inc	hl
		inc	hl
		ld	(hl),0x02
		ld	bc,64
		add	hl,bc
		ld	(hl),0x04
		inc	hl
		inc	hl
		ld	(hl),0x87

gdl40:		ld	a,(screen_redraw)
		and	16
		jr	z,gdl50
		call	blink_unregister	; Blink the panel cursor.
		ld	hl,cursor_panel_offsets
		ld	a,(cursor_panel_position)
		ld	d,4
		or	a
		jr	nz,gdl41
		ld	d,7
gdl41:		cp	2
		jr	nz,gdl42
		ld	d,3
gdl42:		ld	b,0
		ld	c,a
		add	hl,bc
		add	hl,bc
		ld	c,(hl)
		inc	hl
		ld	b,(hl)
		ld	(cursor_panel_offset),bc
		ld	e,0x01
		call	blink_register

gdl50:		ld	a,(screen_redraw)
		and	32
		jr	z,gdl60
		ld	hl,(screen_buffer)	; Draw the preview bar
		ld	bc,33*11		; pipes.
		add	hl,bc
		ld	b,h
		ld	c,l
		push	bc
		ld	a,(preview_bar_slot1)
		call	pipe_draw
		pop	bc
		inc	bc
		inc	bc
		inc	bc
		inc	bc
		ld	a,(preview_bar_slot0)
		call	pipe_draw

gdl60:		ld	a,(screen_redraw)
		and	64
		jr	z,gdl70
		ld	a,(score+1)		; Write the score.
		cp	0x80			; I've set negative
		jr	c,gdl62			; numbers between 9999
		ld	b,a			; and 8000 which will
		ld	a,(score)		; suffice.
		ld	c,a
		or	a
		ld	a,0
		jr	z,gdl61
		ld	a,0x99
gdl61:		sub	b
		daa
		ld	d,a
		xor	a
		sub	c
		daa
		ld	e,a
		ld	a,_MNS
		jr	gdl63
gdl62:		ld	a,_SPC
		ld	de,(score)
gdl63:		ld	hl,(screen_buffer)
		ld	bc,33*4+4
		add	hl,bc			; Write a trailing
		ld	(hl),a			; minus sign or space.
		ld	a,WRITE_TO_SCRBUF
		ld	bc,33*4
		ld	hl,0x0004
		call	hex_write

gdl70:		ld	a,(screen_redraw)
		and	128
		jr	z,gdl80
		ld	bc,(FRAMES)		; If it's time, decrement
		ld	hl,(countdown_last)	; the countdown time
		and	a			; and flag it for
		sbc	hl,bc			; redisplaying.
		ld	a,(countdown_delay)
		cp	l
		jr	nc,gdl80
		ld	(countdown_last),bc
		ld	b,1
		ld	hl,countdown_time
		call	bcd_word_subtract	; hl is preserved here.
		ld	a,(hl)
		inc	hl
		or	(hl)
		jr	nz,gdl71
		call	fill_prepare		; Countdown is now 0.
gdl71:		ld	a,(screen_redraw+1)
		or	1
		ld	(screen_redraw+1),a

gdl80:		ld	a,(screen_redraw+1)
		and	1
		jr	z,gdl90
		ld	a,WRITE_TO_SCRBUF	; Write the countdown
		ld	bc,33*7			; time.
		ld	de,(countdown_time)
		ld	hl,0x0003
		call	hex_write

gdl90:		ld	a,(screen_redraw+1)
		and	2
		jr	z,gdl100
		ld	a,(fill_animate_frame)	; Draw the units of
		ld	b,a			; water for the
		ld	hl,(fill_animation_ptr)	; current frame only.
		ld	de,(fill_animation)
gdl91:		ld	a,h			; Since the pointer will
		cp	d			; be positioned following
		jr	nz,gdl92		; the last entry we
		ld	a,l			; must backtrack until
		cp	e			; the frame is older or
		jr	z,gdl100		; no more entries.
gdl92:		dec	hl
		dec	hl
		dec	hl
		ld	a,(hl)
		cp	b
		jr	nz,gdl100
		push	de
		push	hl
		inc	hl
		ld	e,(hl)
		inc	hl
		ld	d,(hl)
		ld	hl,(screen_buffer)
		add	hl,de
		ld	(hl),FILL_CHAR
		pop	hl
		pop	de
		jr	gdl91

gdl100:		ld	a,(screen_redraw+1)
		and	4
		jr	z,gdl110
		ld	hl,(fill_animation)	; Draw all the units of
gdl101:		ld	a,(hl)			; water so far.
		or	a
		jr	z,gdl110		; End of list reached?
		push	hl
		inc	hl
		ld	e,(hl)
		inc	hl
		ld	d,(hl)
		ld	hl,(screen_buffer)
		add	hl,de
		ld	(hl),FILL_CHAR
		pop	hl
		inc	hl
		inc	hl
		inc	hl
		jr	gdl101

gdl110:		ld	a,(screen_redraw+1)
		and	8
		jr	z,gdl120
		ld	bc,(FRAMES)		; If it's time, flash
		ld	hl,(hiscore_last)	; the new hiscore.
		and	a
		sbc	hl,bc
		ld	a,(hiscore_delay)
		cp	l
		jr	nc,gdl120
		ld	(hiscore_last),bc
		ld	a,(hiscore_frame)
		xor	1			; Flip the animation
		ld	(hiscore_frame),a	; frame.
		cp	1
		jr	nz,gdl111
		ld	a,(screen_redraw+1)	; Write hiscore as
		or	16			; normal on frame 1.
		ld	(screen_redraw+1),a
		jr	gdl120
gdl111:		ld	hl,(screen_buffer)	; On frame 0, erase the
		ld	bc,33			; hiscore with spaces.
		add	hl,bc
		ld	b,4
gdl112:		ld	(hl),_SPC
		inc	hl
		djnz	gdl112

gdl120:		ld	a,(screen_redraw+1)
		and	16
		jr	z,gdl130
		ld	a,WRITE_TO_SCRBUF	; Write the hiscore.
		ld	bc,33*1
		ld	de,(hiscore)
		ld	hl,0x0004
		call	hex_write

gdl130:		
		jp	screen_redraw_reset

; *********************************************************************
; Board Draw                                                          *
; *********************************************************************
; This will draw the contents of the board array to the screen buffer.

board_draw:	ld	de,(board_array)
		ld	hl,(screen_buffer)
		ld	bc,8
		add	hl,bc
		ld	b,8
bdl0:		push	bc
		ld	b,8
bdl1:		push	bc
		ld	a,(de)
		ld	b,h
		ld	c,l
		push	de
		push	hl
		call	pipe_draw
		pop	hl
		inc	hl
		inc	hl
		inc	hl
		pop	de
		inc	de
		pop	bc
		djnz	bdl1
		ld	bc,75
		add	hl,bc
		pop	bc
		djnz	bdl0
		ret

; *********************************************************************
; Pipe Get                                                            *
; *********************************************************************
; This returns the next pipe from the pipe array and tops it up when
; necessary with PIPE_ARRAY_MAX additional shuffled pipe pieces.
; 
; On exit: a = next pipe piece

pipe_get:	ld	a,(pipe_array_idx)
		cp	PIPE_ARRAY_MAX
		jr	nz,pgl10

		ld	hl,(pipe_array)		; Refill the pipe_array.
		ex	de,hl
		ld	hl,pipe_array_data
		ld	bc,PIPE_ARRAY_MAX
		ldir

		cond	DEBUG_DEMOREC
		jr	pgl2
		endc

		ld	a,(state_current)	; During demo mode
		cp	STATE_DEMO		; don't shuffle.
		jr	z,pgl2

		ld	hl,(pipe_array)		; Shuffle the pipe_array.
		ld	d,h
		ld	e,l
		ld	b,PIPE_ARRAY_MAX
pgl0:		push	bc
		push	hl
		call	get_random_number	; 0 to 255.
		srl	a
		cp	PIPE_ARRAY_MAX
		jr	c,pgl1
		sub	PIPE_ARRAY_MAX
pgl1:		ld	b,0
		ld	c,a
		add	hl,bc
		ld	a,(hl)
		ex	de,hl
		ld	b,(hl)
		ld	(hl),a
		ex	de,hl
		ld	(hl),b
		pop	hl
		pop	bc
		inc	de
		djnz	pgl0
pgl2:		xor	a

pgl10:		ld	b,0
		ld	c,a
		ld	hl,(pipe_array)
		add	hl,bc
		ld	b,(hl)
		inc	a
		ld	(pipe_array_idx),a
		ld	a,b
		ret

; *********************************************************************
; Pipe Draw                                                           *
; *********************************************************************
; This will draw a pipe piece.
; 
; On entry: a  = the pipe piece 0 to 18
;           bc = destination address
; 
; Key to pipe pieces :-
; 
; +---------------------------------------+
; | 0   2   4   6   8  10  12  14  16  18 |
; +---------------------------------------+
; |   |   | O | O |   |   | O |   | O | O |
; |   |<< | O |OOO|OX | XO|OO | OO|OO | OO|
; |   |   | O | O |   |   |   | O | O | O |
; +---------------------------------------+
; | 1   3   5   7   9  11  13  15  17 |
; +-----------------------------------+
; |///|   |   |   | O |   | O |   | O |
; |///| <<|OOO| X | X |OO | OO|OOO|OOO|
; |///|   |   | O |   | O |   | O |   |
; +-----------------------------------+

pipe_draw:	ld	hl,(pipe_pieces)
		ld	d,b
		ld	e,c
		ld	b,0
		ld	c,a
		sla	c		; Multiply by 9.
		rl	b
		sla	c
		rl	b
		sla	c
		rl	b
		add	hl,bc
		ld	b,0
		ld	c,a
		add	hl,bc
		ld	a,3
pdl1:		ld	bc,3
		ldir
		ex	de,hl
		ld	bc,30
		add	hl,bc
		ex	de,hl
		dec	a
		jr	nz,pdl1
		ret

; *********************************************************************
; Pipe Pieces Create                                                  *
; *********************************************************************

pipe_pieces_create:
		ld	a,WRITE_TO_MEMBLK	; Decompress the pipe
		ld	bc,(pipe_pieces)	; data.
		ld	de,pipe_data
		call	string_write
		ret

; *********************************************************************
; BCD Word Add                                                        *
; *********************************************************************
; Add an amount to a BCD word.
; 
; On entry: b  = amount
;           hl = address of word
;           Only registers a and bc are modified here.

bcd_word_add:	ld	a,(hl)
bcdwal0:	add	a,1
		daa
		jr	nc,bcdwal2
		ld	c,a
		inc	hl
		ld	a,(hl)
		add	a,1
		daa
		ld	(hl),a
		dec	hl
		ld	a,c
bcdwal2:	djnz	bcdwal0
		ld	(hl),a
		ret

; *********************************************************************
; BCD Word Subtract                                                   *
; *********************************************************************
; Subtract an amount from a BCD word.
; 
; On entry: b  = amount
;           hl = address of word
;           Only registers a and bc are modified here.

bcd_word_subtract:
		ld	a,(hl)
bcdwsl0:	sub	1
		daa
		jr	nc,bcdwsl2
		ld	c,a
		inc	hl
		ld	a,(hl)
		sub	1
		daa
		ld	(hl),a
		dec	hl
		ld	a,c
bcdwsl2:	djnz	bcdwsl0
		ld	(hl),a
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
;           h's bit 7 holds the inverse video state
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
		jr	z,hwl10
		cp	1
		ld	a,0
		jr	z,hwl8
hwl7:		add	a,0x1c
hwl8:		bit	7,h
		jr	z,hwl9
		or	0x80
hwl9:		ld	(bc),a
		inc	bc
hwl10:		dec	l
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
; On entry: a  = WRITE_TO_D_FILE to write to the D_FILE
;           a  = WRITE_TO_SCRBUF to write to the screen buffer
;           a  = WRITE_TO_MEMBLK to write to a block of memory
;                (useful for simply decompressing compressed data)
;           bc = destination offset (the D_FILE's initial NL is
;                accounted for so don't include it yourself) or
;                memory block address
;           de = address of data

string_write:	cp	WRITE_TO_D_FILE
		jr	nz,swl0
		ld	hl,(D_FILE)
		inc	hl
		jr	swl2
swl0:		cp	WRITE_TO_SCRBUF
		jr	nz,swl1
		ld	hl,(screen_buffer)
		jr	swl2
swl1:		ld	hl,0
swl2:		add	hl,bc
swl3:		push	hl
swl4:		ld	a,(de)
		inc	de
		cp	0xd0
		jr	nc,swl5		; Jump if >=
		ld	(hl),a		; Normal char.
		inc	hl
		jr	swl4
swl5:		cp	0xe0
		jr	nc,swl7		; Jump if >=
		and	0x0f		; Duplicate next char.
		ld	b,a
		ld	a,(de)		; Get next char.
		inc	de
swl6:		ld	(hl),a
		inc	hl
		djnz	swl6
		jr	swl4
swl7:		cp	0xf0
		jr	nc,swl8		; Jump if >=
		and	0x0f		; Skip columns.
		ld	c,a
		ld	b,0
		add	hl,bc
		jr	swl4
swl8:		jr	nz,swl9		; Jump if a != 0xf0
		pop	hl		; End of data.
		ret
swl9:		pop	hl		; Newlines.
		and	0x0f
		ld	bc,33
swl10:		add	hl,bc
		dec	a
		jr	nz,swl10
		jr	swl3

; *********************************************************************
; Localise                                                            *
; *********************************************************************

localise:	ld	a,(MARGIN)
		cp	55			; PAL or NTSC?
		jr	nz,locall0
		ld	a,ONE_SECOND_PAL
		jr	locall1
locall0:	ld	a,ONE_SECOND_NTSC

locall1:	ld	b,0
		ld	c,a
		sla	c
		rl	b
		sla	c
		rl	b
		sla	c
		rl	b
		sla	c
		rl	b				; 16s
		ld	(demo_start_delay),bc

		ld	(countdown_delay),a		; 1s
		srl	a				; 1/2s
		ld	(blink_delay),a
		ld	(cursor_game_delay),a
		ld	(hiscore_delay),a
		srl	a				; 1/4s
		ld	(event_repeat_delay_master),a
		ld	(fill_remdead_delay),a
		ld	(demo_event_delay),a
		srl	a				; 1/8s
		ld	(event_repeat_interval_master),a
		ld	(fill_animate_delay),a

		xor	a
		ld	(event_repeat_interval_master+1),a
		ld	(event_repeat_delay_master+1),a
		ret

; *********************************************************************
; Multiply8                                                           *
; *********************************************************************
; A general straightforward byte multiplication routine for use when
; speed isn't an issue or when simplicity is more desirable.
; 
; On entry: b * c
;  On exit: bc = result

;multiply8:	push	af	; Redundant: not used.
;		push	de
;		push	hl
;		ld	hl,0
;		xor	a
;		cp	b
;		jr	z,mul8l2
;		cp	c
;		jr	z,mul8l2
;		ld	e,c
;		ld	d,0
;mul8l1:		add	hl,de
;		djnz	mul8l1
;mul8l2:		ld	b,h
;		ld	c,l
;		pop	hl
;		pop	de
;		pop	af
;		ret

; *********************************************************************
; Get Random Number                                                   *
; *********************************************************************
; This uses a linear feedback shift register (LFSR) algorithm with
; a maximal length tap sequence of [8,6,5,4]
; (see http://www.wikipedia.org/wiki/Linear_feedback_shift_register).
; 
; On exit: a = random number between 0 and 255
;          Only registers a and bc are modified here.

get_random_number:
		ld	a,(FRAMES)	; I've found this loop helps to
		and	7		; open up the gaps between the
		inc	a		; results.
		ld	b,a

grnl0:		ld 	a,(rnd_seed)	; Zero will result in no
		or	a		; feedback and must be checked
		jr	nz,grnl1	; for and dealt with.
		dec	a		; The tap sequence relates to
grnl1:		srl	a		; the bit index like this :-
		ld	c,a		; Taps: 12345678
		srl	a		; Bits: 76543210
		srl	a
		xor	c		; tap6 xor tap8
		ld	a,c
		srl	a
		srl	a
		srl	a
		xor	c		; tap5 xor (tap6 xor tap8)
		ld	a,c
		srl	a
		srl	a
		srl	a
		srl	a
		xor	c		; tap4 xor (tap5 xor (tap6 xor tap8))
		and	1
		rrca
		or	c
		ld	(rnd_seed),a

		djnz	grnl0
		ret

; *********************************************************************
; Dump Debug Info                                                     *
; *********************************************************************

dump_debug_info:
		cond	DEBUG_EVENTS
		ld	b,EVENTS_MAX
		ld	hl,(event_queue)	; Dumps the contents
		ld	de,33*3			; of the event queue.
ddil0:		push	bc
		push	de
		ld	b,d
		ld	c,e
		ld	a,(hl)
		inc	hl
		ld	d,a
		ld	a,(hl)
		inc	hl
		ld	e,a
		push	hl
		ld	a,WRITE_TO_SCRBUF
		ld	hl,0x0004
		call	hex_write
		pop	hl
		pop	de
		ex	de,hl
		ld	bc,33
		add	hl,bc
		ex	de,hl
		pop	bc
		djnz	ddil0
		endc
		
		cond	DEBUG_KEYB
		ld	hl,(keyb_buffer)
		ld	bc,33*11+5		; Dumps the contents of
		ld	de,0x0805		; the keyb_buffer.
ddil1:		push	bc
		push	de
		push	hl
		ld	a,WRITE_TO_SCRBUF
		ld	e,(hl)
		ld	hl,0x0001
		call	hex_write
		pop	hl
		pop	de
		pop	bc
		inc	hl
		inc	bc
		dec	e
		jr	nz,ddil1
		ld	e,5
		push	hl
		ld	h,b
		ld	l,c
		ld	bc,28
		add	hl,bc
		ld	b,h
		ld	c,l
		pop	hl
		dec	d
		jr	nz,ddil1
		endc
		
		cond	DEBUG_STACK
		ld	a,WRITE_TO_SCRBUF	; Dump SP to calculate
		ld	bc,33*21+5		; the program's entire
		ld	hl,0			; memory usage.
		add	hl,sp
		ex	de,hl
		ld	hl,0x0004
		call	hex_write
		endc

		cond	DEBUG_FILL
		ld	a,(substate_current)
		cp	SUBSTATE_REMDEAD
		jr	nz,ddil35
		ld	de,(board_array2)	; Show leaky and dead
		ld	hl,(screen_buffer)	; pipes during 
		ld	bc,33+9			; SUBSTATE_REMDEAD.
		add	hl,bc
		ld	b,0
ddil30:		ld	a,(de)
		cp	2
		jr	c,ddil33
		cp	3
		jr	z,ddil33
		and	0x40
		jr	z,ddil31
		ld	a,_LV
		jr	ddil32
ddil31:		ld	a,(de)
		and	0x20
		jr	nz,ddil33
		ld	a,_DV
ddil32:		ld	(hl),a
ddil33:		inc	de
		inc	hl
		inc	hl
		inc	hl
		inc	b
		ld	a,b
		and	7
		jr	nz,ddil34
		push	bc
		ld	bc,99-24
		add	hl,bc
		pop	bc
ddil34:		ld	a,b
		cp	64
		jr	c,ddil30
ddil35:
		endc

		cond	DEBUG_FILL
		ld	a,(substate_current)
		cp	SUBSTATE_FILLANIM
		jr	nz,ddil45
		ld	de,(board_array2)	; Show pipe exits
		ld	hl,(screen_buffer)	; during 
		ld	bc,33+9			; SUBSTATE_FILLANIM.
		add	hl,bc
		ld	b,0
ddil40:		ld	a,(de)
		and	0x1f
		call	pipe_exits_get
		and	0x0f
		add	a,0x9c
		ld	(hl),a
		inc	de
		inc	hl
		inc	hl
		inc	hl
		inc	b
		ld	a,b
		and	7
		jr	nz,ddil41
		push	bc
		ld	bc,99-24
		add	hl,bc
		pop	bc
ddil41:		ld	a,b
		cp	64
		jr	c,ddil40
ddil45:
		endc

		cond	DEBUG_DEMOREC
		ld	a,WRITE_TO_SCRBUF	; Dump demo_actions
		ld	bc,33*22+5		; address so that the
		ld	de,demo_actions		; recorded demo data
		ld	hl,0x0004		; can be extracted
		call	hex_write		; post-save.
		endc

		ret
		
; *********************************************************************
; Data                                                                *
; *********************************************************************

pipe_exits_data: defb	0,0,8,2,5,10,15,4,8,1,2,12,9,3,6,14,13,11,7

;11,11,11,11,11,12,12,12,12,12,13,13,13,13,13,14,14,14,14,14
;15,15,15,15,16,16,16,16,17,17,17,17,18,18,18,18
;4,4,4,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5
;7,7,8,8,9,9,10,10,6,6,6,6

pipe_array_data: defb	14,5,16,18,17,7,5,12,17,5,17,16,5,10,13,15
		defb	17,4,12,5,16,14,4,5,14,4,4,8,18,4,6,8
		defb	14,13,5,15,6,5,4,10,16,11,4,15,5,13,5,15
		defb	4,11,11,12,6,9,18,11,4,4,4,14,7,13,13,9
		defb	11,12,6,12,5,18

fade_data_x:	defb	26,29,10,22,7,1,24,14,16,32,15,4,30,11,28,8
		defb	17,31,5,3,20,9,12,0,25,27,19,2,21,6,18,23
		defb	13

fade_data_y:	defb	6,7,18,13,10,9,0,3,1,21,5,17,23,19,4,8
		defb	15,2,22,20,12,11,14,16,5,17

cursor_options_offsets:
		defw	33*12+4,33*12+22,33*14+22,33*16+22,33*18+22

cursor_panel_offsets:
		defw	0,33*17,33*19,33*21,33*23

help_page_offsets: defw	help_pg0_data,help_pg1_data,help_pg2_data

help_pg0_data:	defb	0xde,0x80,_HV,_EV,_LV,_PV,0xda,0x80,_1V,_SLSV
		defb	_3V,0x80,0xf2
		defb	_I,_N,_S,_T,_R,_U,_C,_T,_I,_O,_N,_S,0xf1
		defb	0xdc,_MNS,0xf2
		defb	_C,_O,_N,_N,_E,_C,_T,_SPC,_T,_H,_E,_SPC,_SPC,_S
		defb	_T,_A,_R,_T,_SPC,_SPC,_P,_I,_P,_E,_SPC,_SPC,_T
		defb	_O,_SPC,_T,_H,_E,0xf1
		defb	_E,_N,_D,_SPC,_P,_I,_P,_E,_SPC,_U,_S,_I,_N,_G
		defb	_SPC,_A,_S,_SPC,_SPC,_M,_A,_N,_Y,_SPC,_P,_I,_P
		defb	_E,_S,_SPC,_A,_S,0xf1
		defb	_P,_O,_S,_S,_I,_B,_L,_E,_SPC,_SPC,_F,_R,_O,_M
		defb	_SPC,_SPC,_T,_H,_E,_SPC,_SPC,_P,_R,_E,_V,_I,_E
		defb	_W,_SPC,_B,_A,_R,0xf1
		defb	_W,_I,_T,_H,_I,_N,_SPC,_SPC,_T,_H,_E,_SPC,_SPC
		defb	_SPC,_S,_H,_O,_R,_T,_E,_S,_T,_SPC,_SPC,_P,_O,_S
		defb	_S,_I,_B,_L,_E,0xf1
		defb	_T,_I,_M,_E,_FST,0xf2
		defb	_T,_H,_E,_SPC,_P,_I,_P,_E,_SPC,_N,_E,_T,_W,_O,_R
		defb	_K,_SPC,_SPC,_W,_I,_L,_L,_SPC,_B,_E,_SPC,_F,_I
		defb	_L,_L,_E,_D,0xf1
		defb	_W,_H,_E,_N,_SPC,_E,_I,_T,_H,_E,_R,_SPC,_T,_H,_E
		defb	_SPC,_T,_I,_M,_E,_SPC,_H,_A,_S,_SPC,_E,_X,_P,_I
		defb	_R,_E,_D,0xf1
		defb	_O,_R,_SPC,_DQT,_F,_I,_L,_L,_DQT,_SPC,_O,_R,_SPC
		defb	_T,_H,_E,_SPC,_S,_T,_A,_R,_T,_SPC,_P,_I,_P,_E
		defb	_SPC,_H,_A,_V,_E,0xf1
		defb	_B,_E,_E,_N,_SPC,_SPC,_S,_E,_L,_E,_C,_T,_E,_D
		defb	_SPC,_MNS,_MNS,_SPC,_F,_I,_L,_L,_I,_N,_G,_SPC
		defb	_SPC,_E,_A,_R,_L,_Y,0xf1
		defb	_R,_E,_W,_A,_R,_D,_S,_SPC,_A,_SPC,_B,_O,_N,_U,_S
		defb	_FST,0xf2
		defb	_T,_H,_E,_SPC,_G,_A,_M,_E,_SPC,_SPC,_W,_I,_L,_L
		defb	_SPC,_SPC,_E,_N,_D,_SPC,_SPC,_P,_R,_E,_M,_A,_T
		defb	_U,_R,_E,_L,_Y,0xf1
		defb	_W,_H,_I,_L,_S,_T,_SPC,_B,_E,_I,_N,_G,_SPC,_SPC
		defb	_F,_I,_L,_L,_E,_D,_SPC,_SPC,_I,_F,_SPC,_A,_SPC
		defb	_L,_E,_A,_K,_Y,0xf1
		defb	_P,_I,_P,_E,_SPC,_I,_S,_SPC,_D,_E,_T,_E,_C,_T,_E
		defb	_D,_FST,0xf2
		defb	_S,_E,_L,_E,_C,_T,_SPC,_H,_I,_S,_C,_O,_R,_E,_SPC
		defb	_SPC,_F,_R,_O,_M,_SPC,_T,_H,_E,_SPC,_I,_N,_MNS
		defb	_G,_A,_M,_E,0xf1
		defb	_P,_A,_N,_E,_L,_SPC,_T,_O,_SPC,_R,_E,_F,_I,_L,_L
		defb	_SPC,_T,_H,_E,_SPC,_W,_I,_N,_N,_I,_N,_G,_SPC,_P
		defb	_I,_P,_E,0xf1
		defb	_N,_E,_T,_W,_O,_R,_K,_FST
		defb	0xf0

help_pg1_data:	defb	0xde,0x80,_HV,_EV,_LV,_PV,0xda,0x80,_2V,_SLSV
		defb	_3V,0x80,0xf2
		defb	_S,_C,_O,_R,_I,_N,_G,0xf1
		defb	0xd7,_MNS,0xf2
		defb	_PLS,_1,_0,_SPC,_F,_O,_R,_SPC,_E,_A,_C,_H,_SPC
		defb	_P,_I,_P,_E,_SPC,_F,_R,_E,_E,_L,_Y,_SPC,_P,_L,_A
		defb	_C,_E,_D,_FST,0xf2
		defb	_MNS,_1,_0,_SPC,_F,_O,_R,_SPC,_E,_A,_C,_H,_SPC
		defb	_P,_I,_P,_E,_SPC,_O,_V,_E,_R,_W,_R,_I,_T,_T,_E
		defb	_N,_FST,0xf2
		defb	_SPC,_PLS,_5,_SPC,_F,_O,_R,_SPC,_E,_A,_C,_H,_SPC
		defb	_S,_E,_C,_O,_N,_D,_SPC,_R,_E,_M,_A,_I,_N,_I,_N
		defb	_G,_SPC,_I,_F,0xf1
		defb	0xd4,_SPC,_T,_H,_E,_SPC,_N,_E,_T,_W,_O,_R,_K,_SPC
		defb	_I,_S,_SPC,_F,_I,_L,_L,_E,_D,_SPC,_E,_A,_R,_L,_Y
		defb	_FST,0xf2
		defb	_MNS,_1,_0,_SPC,_F,_O,_R,_SPC,_E,_A,_C,_H,_SPC
		defb	_U,_N,_C,_O,_N,_N,_E,_C,_T,_E,_D,_SPC,_P,_I,_P
		defb	_E,0xf1
		defb	0xd4,_SPC,_F,_O,_U,_N,_D,_SPC,_W,_H,_E,_N,_SPC
		defb	_F,_I,_L,_L,_I,_N,_G,_FST,0xf2
		defb	_PLS,_5,_0,_SPC,_F,_O,_R,_SPC,_E,_A,_C,_H,_SPC
		defb	_F,_I,_L,_L,_E,_D,_SPC,_P,_I,_P,_E,_FST
		defb	0xf0

help_pg2_data:	defb	0xde,0x80,_HV,_EV,_LV,_PV,0xda,0x80,_3V,_SLSV
		defb	_3V,0x80,0xf2
		defb	_P,_I,_P,_E,_SPC,_F,_R,_E,_Q,_U,_E,_N,_C,_Y,0xf1
		defb	0xde,_MNS,0xf2
		defb	_T,_H,_E,_SPC,_SPC,_P,_I,_P,_E,_S,_SPC,_SPC,_T
		defb	_H,_A,_T,_SPC,_SPC,_A,_P,_P,_E,_A,_R,_SPC,_SPC
		defb	_O,_N,_SPC,_T,_H,_E,0xf1
		defb	_P,_R,_E,_V,_I,_E,_W,_SPC,_B,_A,_R,_SPC,_A,_R,_E
		defb	_SPC,_SPC,_S,_H,_U,_F,_F,_L,_E,_D,_SPC,_F,_R,_O
		defb	_M,_SPC,_A,0xf1
		defb	_S,_E,_T,_SPC,_F,_R,_E,_Q,_U,_E,_N,_C,_Y,_SPC
		defb	_MNS,_MNS,_SPC,_E,_V,_E,_R,_Y,_SPC,_SPC,_7,_0
		defb	_SPC,_P,_I,_P,_E,_S,0xf1
		defb	_W,_I,_L,_L,_SPC,_I,_N,_C,_L,_U,_D,_E,_SPC,_T
		defb	_H,_E,_SPC,_F,_O,_L,_L,_O,_W,_I,_N,_G,_CLN,0xf2
		defb	_SPC,_ASK,_SPC,_2,_0,_SPC,_C,_O,_R,_N,_E,_R,_SPC
		defb	_P,_I,_E,_C,_E,_S,0xf1
		defb	_SPC,_ASK,_SPC,_1,_6,_SPC,_T,_SPC,_P,_I,_E,_C,_E
		defb	_S,0xf1
		defb	_SPC,_ASK,_SPC,_1,_1,_SPC,_V,_E,_R,_T,_I,_C,_A
		defb	_L,_SPC,_P,_I,_P,_E,_S,0xf1
		defb	_SPC,_ASK,_SPC,_1,_1,_SPC,_H,_O,_R,_I,_Z,_O,_N
		defb	_T,_A,_L,_SPC,_P,_I,_P,_E,_S,0xf1
		defb	_SPC,_ASK,_SPC,_SPC,_8,_SPC,_T,_E,_R,_M,_I,_N,_A
		defb	_T,_O,_R,_S,0xf1
		defb	_SPC,_ASK,_SPC,_SPC,_4,_SPC,_C,_R,_O,_S,_S,_E,_S
		defb	0xf0

pipe_data:	defb	0xd9,_SPC
		defb	0xd9,_SLS
		defb	0xd3,_SPC,_LTHV,_LTHV,0xd8
		defb	_SPC,_LTHV,_LTHV,0xd4
		defb	_SPC,0x80,_SPC,_SPC,0x80,_SPC,_SPC,0x80,0xd4
		defb	_SPC,0xd3,0x80,0xd4
		defb	_SPC,0x80,_SPC,0xd3,0x80,_SPC,0x80,0xd5
		defb	_SPC,_XV ,_SPC,_SPC,0x80,0xd4
		defb	_SPC,0x80,_XV ,0xd5
		defb	_SPC,0x80,_SPC,_SPC,_XV ,0xd8
		defb	_SPC,_XV ,0x80,0xd6
		defb	_SPC,0x80,0x80,_SPC,_SPC,0x80,_SPC
		defb	_SPC,0x80,_SPC,0x80,0x80,0xd5
		defb	_SPC,0x80,_SPC,_SPC,0x80,0x80,0xd7
		defb	_SPC,0x80,0x80,_SPC,0x80,0xd4
		defb	_SPC,0xd3,0x80,_SPC,0x80,_SPC
		defb	_SPC,0x80,_SPC,0x80,0x80,_SPC,_SPC,0x80,_SPC
		defb	_SPC,0x80,_SPC,0xd3,0x80,0xd4
		defb	_SPC,0x80,_SPC,_SPC,0x80,0x80,_SPC,0x80,_SPC
		defb	0xf0

options_data:	defb	_S,_E,_T,_SPC,_C,_O,_N,_T,_R,_O,_L,_S,0xf2
		defb	_U,_P,0xe5,_EQU,0xf1
		defb	_D,_O,_W,_N,0xe3,_EQU,0xf1
		defb	_L,_E,_F,_T,0xe3,_EQU,0xf1
		defb	_R,_I,_G,_H,_T,0xe2,_EQU,0xf1
		defb	_S,_E,_L,_E,_C,_T,_SPC,_EQU
		defb	0xf0

options_frame_nofocus_data:
		defb	0xdf,0x01,0x86,0xf1
		defb	0x01,0xee,0x87,0xf1
		defb	0x01,0xee,0x87,0xf1
		defb	0x01,0xee,0x87,0xf1
		defb	0x01,0xee,0x87,0xf1
		defb	0x01,0xee,0x87,0xf1
		defb	0x01,0xee,0x87,0xf1
		defb	0x01,0xee,0x87,0xf1
		defb	0x86,0xdf,0x87
		defb	0xf0

;		defb	_SPC,0xdf,_SPC,0xf1	; Redundant.
;		defb	_SPC,0xee,_SPC,0xf1
;		defb	_SPC,0xee,_SPC,0xf1
;		defb	_SPC,0xee,_SPC,0xf1
;		defb	_SPC,0xee,_SPC,0xf1
;		defb	_SPC,0xee,_SPC,0xf1
;		defb	_SPC,0xee,_SPC,0xf1
;		defb	_SPC,0xee,_SPC,0xf1
;		defb	_SPC,0xdf,_SPC
;		defb	0xf0

options_frame_focus_data:
		defb	0x07,0xde,0x03,0x86,0xf1
		defb	0x05,0xee,0x85,0xf1
		defb	0x05,0xee,0x85,0xf1
		defb	0x05,0xee,0x85,0xf1
		defb	0x05,0xee,0x85,0xf1
		defb	0x05,0xee,0x85,0xf1
		defb	0x05,0xee,0x85,0xf1
		defb	0x05,0xee,0x85,0xf1
		defb	0x86,0xde,0x83,0x81
		defb	0xf0

options_menu_data:
		defb	_P,_L,_A,_Y,0xf2
		defb	_S,_A,_V,_E,0xf2
		defb	_H,_E,_L,_P,0xf2
		defb	_Q,_U,_I,_T
		defb	0xf0

options_menu_frame_nofocus_data:
		defb	0xd7,0x01,0x86,0xf1
		defb	0x01,0xe6,0x87,0xf1
		defb	0x01,0xe6,0x87,0xf1
		defb	0x01,0xe6,0x87,0xf1
		defb	0x01,0xe6,0x87,0xf1
		defb	0x01,0xe6,0x87,0xf1
		defb	0x01,0xe6,0x87,0xf1
		defb	0x01,0xe6,0x87,0xf1
		defb	0x86,0xd7,0x87
		defb	0xf0

;		defb	_SPC,0xd7,_SPC,0xf1	; Redundant.
;		defb	_SPC,0xe6,_SPC,0xf1
;		defb	_SPC,0xe6,_SPC,0xf1
;		defb	_SPC,0xe6,_SPC,0xf1
;		defb	_SPC,0xe6,_SPC,0xf1
;		defb	_SPC,0xe6,_SPC,0xf1
;		defb	_SPC,0xe6,_SPC,0xf1
;		defb	_SPC,0xe6,_SPC,0xf1
;		defb	_SPC,0xd7,_SPC
;		defb	0xf0

options_menu_frame_focus_data:
		defb	0x07,0xd6,0x03,0x86,0xf1
		defb	0x05,0xe6,0x85,0xf1
		defb	0x05,0xe6,0x85,0xf1
		defb	0x05,0xe6,0x85,0xf1
		defb	0x05,0xe6,0x85,0xf1
		defb	0x05,0xe6,0x85,0xf1
		defb	0x05,0xe6,0x85,0xf1
		defb	0x05,0xe6,0x85,0xf1
		defb	0x86,0xd6,0x83,0x81
		defb	0xf0

options_copyright_data:
		defb	_OBR,_C,_CBR,_SPC,_2,_0,_1,_0,_SPC,_T,_H
		defb	_U,_N,_O,_R,_CMA,_SPC,_G,_N,_U,_SPC,_G,_P,_L,0xf2
		defb	0xe3,_A,_N,_G,_L,_I,_A,_S,_O,_F,_T,_FST,_O,_R,_G,_FST,_U,_K
		defb	0xf0

panel_data:	defb	_H,_I,_S,_C,_O,_R,_E,0xf3
		defb	_S,_C,_O,_R,_E,0xf3
		defb	_T,_I,_M,_E,_OBR,_S,_CBR,0xf4
		defb	0xd7,_MNS,0xf1
		defb	0xe3,_GTH,0xf1
		defb	0xe3,_GTH,0xf1
		defb	0xe3,_GTH,0xf1
		defb	0xd7,_MNS,0xf3
		defb	_F,_I,_L,_L,0xf2
		defb	_N,_E,_W,0xf2
		defb	_H,_E,_L,_P,0xf2
		defb	_B,_A,_C,_K
		defb	0xf0

; As an example, compressing this PipePanic logo saved 87 bytes.

pipepanic_data:	defb	0x00,0xd3,0x83,0x87,0x04,0xd8,0x00,0xd3,0x83,0xe7,0x87,0x04,0xf1
		defb	0x85,0x00,0x83,0x00,0x05,0x85,0xe7,0x85,0x00,0x83,0x00,0x05,0xe6,0x05,0x85,0xf1
		defb	0x85,0xe3,0x07,0x01,0xe7,0x85,0xe3,0x05,0xe6,0x02,0x01,0xf1
		defb	0x85,0x00,0x8A,0x8A,0x07,0x86,0xd3,0x03,0x06,0x03,0x03,0x86,0x85,0x00,0x8A,0x8A,0x07,0x03,0x03,0x84,0xd3,0x03,0x06,0x86,0xd3,0x03,0x04,0xf1
		defb	0x85,0x00,0x05,0x00,0x05,0x85,0x00,0x03,0x00,0x05,0x02,0x01,0x85,0x85,0x00,0x05,0x00,0x05,0x02,0x01,0x85,0xe3,0x05,0x85,0x00,0x8A,0x8A,0x01,0xf1
		defb	0x85,0x09,0x05,0x00,0x05,0x85,0x00,0x09,0x09,0x05,0x02,0x03,0x86,0x85,0x09,0x05,0x00,0x05,0x87,0x04,0x85,0x09,0x8A,0x09,0x05,0x85,0xd3,0x09,0x05,0xf1
		defb	0x00,0x03,0xe2,0x02,0x84,0x00,0x07,0x03,0x02,0x03,0x03,0x01,0x00,0x03,0xe2,0x02,0x01,0x02,0x01,0x03,0x00,0x03,0x02,0x01,0xd3,0x03,0xf1
		defb	0x87,0xd3,0x03,0x04,0x85,0x00,0x05,0x87,0xdf,0x03,0xd5,0x03,0x04,0xf1
		defb	0x02,0xd3,0x8A,0x01,0x02,0x8A,0x01,0x02,0xdf,0x8A,0xd5,0x8A,0x01
		defb	0xf0

version_data:	defb	_0,_FST,_1,_FST,_3
		defb	0xf0

; And compressing this splash logo saved 132 bytes.

splash_logo_data:
		defb	0xe8,0x87,0x83,0x83,0xe7,0x83,0x83,0x04,0xf1
		defb	0xe3,0x06,0xd4,0x03,0x84,0x00,0x85,0x03,0x84,0xd3,0x03,0x07,0x03,0x05,0x00,0x07,0xd4,0x03,0x86,0xf1
		defb	0xe2,0x85,0x09,0xe4,0x85,0x00,0x85,0x87,0x01,0x06,0x03,0x86,0x02,0x04,0x05,0x00,0x05,0xe4,0x09,0x05,0xf1
		defb	0xe2,0x85,0x00,0xd4,0x0A,0x85,0x0A,0xd3,0x85,0xe3,0xd3,0x05,0x0A,0x05,0xd4,0x0A,0x00,0x05,0xf1
		defb	0xe2,0x85,0x0A,0xd4,0x09,0x85,0x09,0xd3,0x85,0x81,0x80,0x82,0xd3,0x05,0x09,0x05,0xd4,0x09,0x0A,0x05,0xf1
		defb	0xe3,0x86,0xd4,0x8A,0x81,0x08,0x85,0x81,0x04,0x84,0x80,0x07,0x87,0x82,0x05,0x08,0x82,0xd4,0x8A,0x06,0xf1
		defb	0x00,0x09,0x08,0xd5,0x09,0x02,0x03,0x03,0x00,0x02,0x83,0x80,0x83,0x01,0x00,0x03,0x03,0x01,0xd5,0x09,0x08,0x09,0xf1
		defb	0xda,0x0A,0xe3,0x83,0x80,0x83,0xe3,0xda,0x0A,0xf1
		defb	0xeb,0x83,0xd5,0x80,0x83,0xf1
		defb	0xeb,0x02,0x84,0xd3,0x80,0x07,0x01
		defb	0xf0

txt_press_a_key:
		defb	_P,_R,_E,_S,_S,_SPC,_A,_SPC,_K,_E,_Y
		defb	0xf0

; To record a demo:
; * Delete the default demo_actions data below (not the defs 512).
; * At the top of the program, set DEBUG_DEMOREC to 1.
; * Recompile and run it again and take note of the demo_actions
;   address displayed at the bottom left of the screen.
; * Play a game, go BACK and select SAVE.
; * Extract the data from the demo_actions address using
;   whatever program you have that can do this. On GNU/Linux it's
;   $ hexdump -s 0xnnnn -v -e '16/1 ",%d" "\n"' pipepanic.p
;   where 0xnnnn is demo_actions-0x4009.
; * Insert the data below with a demo_actions label.
; * Tidy-up the end (remove the 2,2,2,5 that was recorded as a result
;   of moving the panel cursor down, down, down and selecting BACK),
;   add some zeroes to do nothing whilst the network is being filled
;   and terminate it with a 6.
; * Set DEBUG_DEMOREC to 0 and recompile.
; * Tweak the trailing zeroes so that the user has enough time to
;   admire your filled network before the demo ends.

; Demo Data Key
; -------------
;  1   0 is nothing
; 3+4  5 is select
;  2   6 is end

		cond	DEBUG_DEMOREC
demo_actions:	defs	512
		endc

demo_actions:	defb	1,5,4,5,4,5,4,5,2,2,3,5,3,1,5,2
		defb	3,3,5,2,2,5,3,5,1,1,5,1,5,1,5,4
		defb	2,5,3,3,2,5,2,2,5,1,4,5,4,4,1,5
		defb	1,1,1,3,5,4,4,5,1,1,3,5,2,4,5,1
		defb	4,5,2,5,4,1,5,2,5,2,5,3,5,4,4,1
		defb	5,2,2,2,3,5,2,2,5,1,5,2,2,2,4,5
		defb	4,4,5,2,2,2,5,4,1,1,1,5,1,1,4,5
		defb	4,4,1,5,2,3,5,1,1,4,4,5,1,1,3,3
		defb	5,2,2,2,1,5,1,1,1,1,4,5,3,3,2,5
		defb	1,5,2,4,5,1,1,4,4,4,5,3,3,5,2,2
		defb	3,3,3,5,2,5,1,5,3,3,1,1,1,1,1,5
		defb	2,2,2,5,3,3,3,1,5,2,5,2,3,3,2,5
		defb	1,1,4,4,4,1,5,4,4,5,1,5,4,4,1,1
		defb	1,5,2,2,2,2,5,1,1,1,1,5,1,5,3,1
		defb	1,3,3,5,5,3,3,5,1,1,1,1,3,5,3,5
		defb	2,2,2,2,4,4,5,5,5,5,5,4,4,4,1,1
		defb	5,1,1,4,5,0,0,0,0,0,0,0,0,0,0,0
		defb	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		defb	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
		defb	0,0,0,6

; End of user machine code program ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

		defb	_NL
basic_0010:	defb	0,10		; 10 LET S=VAL "USR 16514"
		defw	basic_0020-basic_0010-4
		defb	_LET,_S,_EQU,_VAL,_DQT,_USR,_1,_6,_5,_1,_4,_DQT
		defb	_NL
basic_0020:	defb	0,20		; 20 IF NOT S THEN STOP
		defw	basic_0030-basic_0020-4
		defb	_IF,_NOT,_S,_THEN,_STOP
		defb	_NL
basic_0030:	defb	0,30		; 30 SAVE "PIPEPANIC"
		defw	basic_0040-basic_0030-4
		defb	_SAVE,_DQT,_P,_I,_P,_E,_P,_A,_N,_I,_C,_DQT
		defb	_NL
basic_0040:	defb	0,40		; 40 RUN
		defw	basic_end-basic_0040-4
		defb	_RUN
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






