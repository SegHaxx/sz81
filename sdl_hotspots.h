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
#define MAX_HOTSPOTS 96

/* Hotspot group IDs */
#define HS_GRP_ROOT 0
#define HS_GRP_LOAD 1
#define HS_GRP_VKEYB 2
#define HS_GRP_CTB 3
#define HS_GRP_RUNOPT0 4
#define HS_GRP_RUNOPT1 5

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
#define HS_RUNOPT0_RUNOPT0 54
#define HS_RUNOPT0_JDEADZ_DN 55
#define HS_RUNOPT0_JDEADZ_UP 56
#define HS_RUNOPT0_SAVE 57
#define HS_RUNOPT0_EXIT 58
#define HS_RUNOPT0_NEXT 59
#define HS_RUNOPT1_RUNOPT1 60

/* Hotspot properties */
#define HS_PROP_ONOFF 1
#define HS_PROP_STICKY 2
#define HS_PROP_TOGGLE 4
#define HS_PROP_STUCK 8
#define HS_PROP_VISIBLE 16
#define HS_PROP_SELECTED 32
#define HS_PROP_NAVIGABLE 64

/* Variables */
SDL_Surface *highlight;

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


