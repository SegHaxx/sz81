/* sz81 Copyright (C) 2007-2010 Thunor <thunorsif@hotmail.com>
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
#define MAX_HOTSPOTS 128

/* Hotspot group IDs */
#define HS_GRP_ROOT 0
#define HS_GRP_LOAD 1
#define HS_GRP_VKEYB 2
#define HS_GRP_CTB 3
#define HS_GRP_RUNOPT0 4
#define HS_GRP_RUNOPT1 5
#define HS_GRP_RUNOPT2 6
#define HS_GRP_RUNOPT3 7

/* Hotspot IDs (hotspots[] indexes which are also zorders) */
#define HS_ROOT_CANVAS 0
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
#define HS_CTB_INVERSE 51
#define HS_CTB_ALPHA_DN 52
#define HS_CTB_ALPHA_UP 53
#define HS_CTB_RUNOPTS 54
#define HS_CTB_SPEED_DN 55
#define HS_CTB_SPEED_UP 56
#define HS_CTB_SAVESTATE 57
#define HS_CTB_LOADSTATE 58
#define HS_CTB_SPARE1 59
#define HS_CTB_SPARE2 60
#define HS_CTB_SPARE3 61
#define HS_RUNOPTS2_RUNOPTS2 62
#define HS_RUNOPTS2_VOLUME_DN 63
#define HS_RUNOPTS2_VOLUME_UP 64
#define HS_RUNOPTS2_KRDELAY_DN 65
#define HS_RUNOPTS2_KRDELAY_UP 66
#define HS_RUNOPTS2_KRINTERVAL_DN 67
#define HS_RUNOPTS2_KRINTERVAL_UP 68
#define HS_RUNOPTS2_FGC_R_DN 69
#define HS_RUNOPTS2_FGC_R_UP 70
#define HS_RUNOPTS2_BGC_R_DN 71
#define HS_RUNOPTS2_BGC_R_UP 72
#define HS_RUNOPTS2_FGC_G_DN 73
#define HS_RUNOPTS2_FGC_G_UP 74
#define HS_RUNOPTS2_BGC_G_DN 75
#define HS_RUNOPTS2_BGC_G_UP 76
#define HS_RUNOPTS2_FGC_B_DN 77
#define HS_RUNOPTS2_FGC_B_UP 78
#define HS_RUNOPTS2_BGC_B_DN 79
#define HS_RUNOPTS2_BGC_B_UP 80
#define HS_RUNOPTS2_SAVE 81
#define HS_RUNOPTS2_EXIT 82
#define HS_RUNOPTS2_NEXT 83
#define HS_RUNOPTS2_SPARE1 84
#define HS_RUNOPTS2_SPARE2 85
#define HS_RUNOPTS2_SPARE3 86
#define HS_RUNOPTS2_SPARE4 87
#define HS_RUNOPTS3_RUNOPTS3 88
#define HS_RUNOPTS3_JDEADZ_DN 89
#define HS_RUNOPTS3_JDEADZ_UP 90
#define HS_RUNOPTS3_JOY_CFG_LTRIG 91
#define HS_RUNOPTS3_JOY_CFG_RTRIG 92
#define HS_RUNOPTS3_JOY_CFG_LEFT 93
#define HS_RUNOPTS3_JOY_CFG_RIGHT 94
#define HS_RUNOPTS3_JOY_CFG_UP 95
#define HS_RUNOPTS3_JOY_CFG_DOWN 96
#define HS_RUNOPTS3_JOY_CFG_SELECT 97
#define HS_RUNOPTS3_JOY_CFG_START 98
#define HS_RUNOPTS3_JOY_CFG_A 99
#define HS_RUNOPTS3_JOY_CFG_B 100
#define HS_RUNOPTS3_JOY_CFG_Y 101
#define HS_RUNOPTS3_JOY_CFG_X 102
#define HS_RUNOPTS3_BACK 103
#define HS_RUNOPTS3_SAVE 104
#define HS_RUNOPTS3_EXIT 105

/* Hotspot properties */
#define HS_PROP_ONOFF 1
#define HS_PROP_STICKY 2
#define HS_PROP_TOGGLE 4
#define HS_PROP_STUCK 8
#define HS_PROP_VISIBLE 16
#define HS_PROP_SELECTED 32
#define HS_PROP_NAVIGABLE 64

/* Variables */
struct hotspot {
	int gid;			/* Group id for easy management */
	int flags;			/* An OR'd combination of HS_PROP_ properties */
	int hit_x, hit_y, hit_w, hit_h;	/* Hit box */
	int hl_x, hl_y, hl_w, hl_h;		/* Highlight box (if all UNDEFINED then use hitbox */
	int remap_id;		/* The main destination control id (could be UNDEFINED) */
};
struct hotspot hotspots[MAX_HOTSPOTS];

/* Function prototypes */
void hotspots_init(void);
void hotspots_resize(void);
void hotspots_update(void);
void hotspots_render(void);
int get_selected_hotspot(int gid);


