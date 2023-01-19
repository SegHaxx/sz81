/* sz81 Copyright (C) 2007-2011 Thunor <thunorsif@hotmail.com>
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

/* Defines */
#define MAX_HOTSPOTS 199

/* Hotspot group IDs */
#define HS_GRP_EMU 1
#define HS_GRP_LOAD 2
#define HS_GRP_VKEYB 4
#define HS_GRP_CTB 8
#define HS_GRP_RUNOPTS0 16
#define HS_GRP_RUNOPTS1 32
#define HS_GRP_RUNOPTS2 64
#define HS_GRP_RUNOPTS3 128
#define HS_GRP_RUNOPTS_ALL (HS_GRP_RUNOPTS0 | HS_GRP_RUNOPTS1 | HS_GRP_RUNOPTS2 | HS_GRP_RUNOPTS3)
#define HS_GRP_LDFILE 256
#define HS_GRP_SSTATE 512
#define HS_GRP_DIALOG 1024
#define HS_GRP_ALL ((HS_GRP_DIALOG - 1) | HS_GRP_DIALOG)

/* Hotspot IDs (hotspots[] indexes which are also zorders) */
#define HS_EMU_EMU 0
#define HS_LOAD_Q 1
#define HS_LOAD_A 2
#define HS_LOAD_ENTER 3
#define HS_LOAD_SPACE 4
#define HS_VKEYB_VKEYB 5
#define HS_VKEYB_1 6
#define HS_VKEYB_Q 16
#define HS_VKEYB_A 26
#define HS_VKEYB_NEWLINE 35
#define HS_VKEYB_SHIFT 36
#define HS_CTB_CTB 46
#define HS_CTB_EXIT 47
#define HS_CTB_RESET 48
#define HS_CTB_AUTOHIDE 49
#define HS_CTB_SHIFT_TYPE 50
#define HS_CTB_ALPHA_DN 51
#define HS_CTB_ALPHA_UP 52
#define HS_CTB_INVERSE 53
#define HS_CTB_SSTATE_SAVE 54
#define HS_CTB_SSTATE_LOAD 55
#define HS_CTB_LDFILE 56
#define HS_CTB_RUNOPTS 57
#define HS_RUNOPTS0_RUNOPTS0 61
#define HS_RUNOPTS0_ZX80 62
#define HS_RUNOPTS0_ZX81 63
#define HS_RUNOPTS0_RAM_DN 64
#define HS_RUNOPTS0_RAM_UP 65
#define HS_RUNOPTS0_M1NOT_NO 66
#define HS_RUNOPTS0_M1NOT_YES 67
#define HS_RUNOPTS0_FRAMESKIP_DN 68
#define HS_RUNOPTS0_FRAMESKIP_UP 69
#define HS_RUNOPTS0_SPEED_DN 70
#define HS_RUNOPTS0_SPEED_UP 71
#define HS_RUNOPTS0_WRX_NO 72
#define HS_RUNOPTS0_WRX_YES 73
#define HS_RUNOPTS0_UDG_NO 74
#define HS_RUNOPTS0_UDG_YES 75
#define HS_RUNOPTS0_SAVE 76
#define HS_RUNOPTS0_EXIT 77
#define HS_RUNOPTS0_NEXT 78
#define HS_RUNOPTS1_RUNOPTS1 83
#define HS_RUNOPTS1_DEVICE_NONE 84
#define HS_RUNOPTS1_DEVICE_AY 85
#define HS_RUNOPTS1_DEVICE_QUICKSILVA 86
#define HS_RUNOPTS1_DEVICE_ZONX 87
#define HS_RUNOPTS1_ACB_STEREO 88
#define HS_RUNOPTS1_DEVICE_VSYNC 89
#define HS_RUNOPTS1_BACK 90
#define HS_RUNOPTS1_SAVE 91
#define HS_RUNOPTS1_EXIT 92
#define HS_RUNOPTS1_NEXT 93
#define HS_RUNOPTS2_RUNOPTS2 100
#define HS_RUNOPTS2_VOLUME_DN 101
#define HS_RUNOPTS2_VOLUME_UP 102
#define HS_RUNOPTS2_KRDELAY_DN 103
#define HS_RUNOPTS2_KRDELAY_UP 104
#define HS_RUNOPTS2_KRINTERVAL_DN 105
#define HS_RUNOPTS2_KRINTERVAL_UP 106
#define HS_RUNOPTS2_FGC_R_DN 107
#define HS_RUNOPTS2_FGC_R_UP 108
#define HS_RUNOPTS2_BGC_R_DN 109
#define HS_RUNOPTS2_BGC_R_UP 110
#define HS_RUNOPTS2_FGC_G_DN 111
#define HS_RUNOPTS2_FGC_G_UP 112
#define HS_RUNOPTS2_BGC_G_DN 113
#define HS_RUNOPTS2_BGC_G_UP 114
#define HS_RUNOPTS2_FGC_B_DN 115
#define HS_RUNOPTS2_FGC_B_UP 116
#define HS_RUNOPTS2_BGC_B_DN 117
#define HS_RUNOPTS2_BGC_B_UP 118
#define HS_RUNOPTS2_BACK 119
#define HS_RUNOPTS2_SAVE 120
#define HS_RUNOPTS2_EXIT 121
#define HS_RUNOPTS2_NEXT 122
#define HS_RUNOPTS3_RUNOPTS3 127
#define HS_RUNOPTS3_JDEADZ_DN 128
#define HS_RUNOPTS3_JDEADZ_UP 129
#define HS_RUNOPTS3_JOY_CFG_LTRIG 130
#define HS_RUNOPTS3_JOY_CFG_RTRIG 131
#define HS_RUNOPTS3_JOY_CFG_LEFT 132
#define HS_RUNOPTS3_JOY_CFG_RIGHT 133
#define HS_RUNOPTS3_JOY_CFG_UP 134
#define HS_RUNOPTS3_JOY_CFG_DOWN 135
#define HS_RUNOPTS3_JOY_CFG_SELECT 136
#define HS_RUNOPTS3_JOY_CFG_START 137
#define HS_RUNOPTS3_JOY_CFG_A 138
#define HS_RUNOPTS3_JOY_CFG_B 139
#define HS_RUNOPTS3_JOY_CFG_Y 140
#define HS_RUNOPTS3_JOY_CFG_X 141
#define HS_RUNOPTS3_BACK 142
#define HS_RUNOPTS3_SAVE 143
#define HS_RUNOPTS3_EXIT 144
#define HS_LDFILE_LDFILE 149
#define HS_LDFILE_LIST00 150
#define HS_LDFILE_LIST01 151
#define HS_LDFILE_LIST02 152
#define HS_LDFILE_LIST03 153
#define HS_LDFILE_LIST04 154
#define HS_LDFILE_LIST05 155
#define HS_LDFILE_LIST06 156
#define HS_LDFILE_LIST07 157
#define HS_LDFILE_LIST08 158
#define HS_LDFILE_LIST09 159
#define HS_LDFILE_LIST10 160
#define HS_LDFILE_LIST11 161
#define HS_LDFILE_LIST12 162
#define HS_LDFILE_LIST13 163
#define HS_LDFILE_LIST14 164
#define HS_LDFILE_LIST15 165
#define HS_LDFILE_LIST16 166
#define HS_LDFILE_LIST17 167
#define HS_LDFILE_LIST18 168
#define HS_LDFILE_LIST19 169
#define HS_LDFILE_SBUP 170
#define HS_LDFILE_SBHDLE 171
#define HS_LDFILE_SBPGUP 172
#define HS_LDFILE_SBPGDN 173
#define HS_LDFILE_SBDOWN 174
#define HS_LDFILE_LOAD 175
#define HS_LDFILE_EXIT 176
#define HS_SSTATE_SSTATE 179
#define HS_SSTATE_SLOT0 180
#define HS_SSTATE_SLOT1 181
#define HS_SSTATE_SLOT2 182
#define HS_SSTATE_SLOT3 183
#define HS_SSTATE_SLOT4 184
#define HS_SSTATE_SLOT5 185
#define HS_SSTATE_SLOT6 186
#define HS_SSTATE_SLOT7 187
#define HS_SSTATE_SLOT8 188
#define HS_SSTATE_EXIT 189
#define HS_DIALOG_DIALOG 195
#define HS_DIALOG_BUTTON0 196
#define HS_DIALOG_BUTTON1 197
#define HS_DIALOG_BUTTON2 198

/* Hotspot properties */
#define HS_PROP_ONOFF 1				/* The normal type: press on, release off */
#define HS_PROP_STICKY 2			/* Press on to stick, press and release it or something else off */
#define HS_PROP_TOGGLE 4			/* Press on to stick, press off */
#define HS_PROP_STUCK 8				/* Sticky and toggle hotspots become stuck on */
#define HS_PROP_VISIBLE 16			/* Invisible also equates to disabled */
#define HS_PROP_SELECTED 32
#define HS_PROP_DRAGGABLE 64		/* Under normal operation, if a hotspot doesn't have an UNDEFINED
									 * gid and is visible then it can process pressed or released
									 * events, but this property enables additional motion events */
/* Variables */
typedef struct hotspot {
	int gid;						/* Group id for easy management */
	int flags;						/* An OR'd combination of HS_PROP_ properties */
	int hit_x, hit_y, hit_w, hit_h;	/* Hit box */
	int hl_x, hl_y, hl_w, hl_h;		/* Highlight box (if all UNDEFINED then use hitbox */
	int remap_id;					/* The main destination control id (could be UNDEFINED) */
} hotspot_;

extern hotspot_ hotspots[MAX_HOTSPOTS];

/* Function prototypes */
void hotspots_vkeyb_shift_init(void);
void hotspots_resize(int gid);
void hotspots_update(void);
void hotspots_render(void);
int get_selected_hotspot(int gid);


