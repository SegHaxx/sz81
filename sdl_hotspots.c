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

/* Includes */
#include "sdl_engine.h"

/* Defines */

/* Variables */

/* Function prototypes */


/***************************************************************************
 * Hotspots Initialise                                                     *
 ***************************************************************************/
/* This creates the hotspots and initialises their static properties */

void sdl_hotspots_init(void) {
	int count;

	/* Initialise all hotspots with some default values */
	for (count = 0; count < MAX_HOTSPOTS; count++) {
		hotspots[count].gid = UNDEFINED;
		hotspots[count].hit_x = hotspots[count].hit_y = UNDEFINED;
		hotspots[count].hit_w = hotspots[count].hit_h = UNDEFINED;
		hotspots[count].hl_x = hotspots[count].hl_y = UNDEFINED;
		hotspots[count].hl_w = hotspots[count].hl_h = UNDEFINED;
		hotspots[count].flags = 0;
		hotspots[count].flags |= HS_PROP_VISIBLE;
		hotspots[count].flags |= HS_PROP_NAVIGABLE;
		hotspots[count].flags |= HS_PROP_ONOFF;
		hotspots[count].remap_id = UNDEFINED;
	}
	
	/* Initialise root hotspots */
	hotspots[HS_EMU_EMU].gid = HS_GRP_EMU;
	hotspots[HS_EMU_EMU].flags &= ~HS_PROP_NAVIGABLE;
	hotspots[HS_EMU_EMU].remap_id = SDLK_F1;

	/* Initialise file selector hotspots */
	for (count = HS_LOAD_Q; count <= HS_LOAD_SPACE; count++) {
		hotspots[count].gid = HS_GRP_LOAD;
	}
	hotspots[HS_LOAD_Q].remap_id = SDLK_q;
	hotspots[HS_LOAD_A].remap_id = SDLK_a;
	hotspots[HS_LOAD_ENTER].remap_id = SDLK_RETURN;
	hotspots[HS_LOAD_ENTER].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_LOAD_SPACE].remap_id = SDLK_SPACE;

	/* Initialise load file dialog hotspots */
	for (count = HS_LDFILE_LDFILE; count <= HS_LDFILE_EXIT; count++) {
		hotspots[count].gid = HS_GRP_LDFILE;
		if (count <= HS_LDFILE_LIST19) 
			hotspots[count].flags &= ~HS_PROP_NAVIGABLE;
	}
	hotspots[HS_LDFILE_LIST00].remap_id = SDLK_ROW00;
	hotspots[HS_LDFILE_LIST01].remap_id = SDLK_ROW01;
	hotspots[HS_LDFILE_LIST02].remap_id = SDLK_ROW02;
	hotspots[HS_LDFILE_LIST03].remap_id = SDLK_ROW03;
	hotspots[HS_LDFILE_LIST04].remap_id = SDLK_ROW04;
	hotspots[HS_LDFILE_LIST05].remap_id = SDLK_ROW05;
	hotspots[HS_LDFILE_LIST06].remap_id = SDLK_ROW06;
	hotspots[HS_LDFILE_LIST07].remap_id = SDLK_ROW07;
	hotspots[HS_LDFILE_LIST08].remap_id = SDLK_ROW08;
	hotspots[HS_LDFILE_LIST09].remap_id = SDLK_ROW09;
	hotspots[HS_LDFILE_LIST10].remap_id = SDLK_ROW10;
	hotspots[HS_LDFILE_LIST11].remap_id = SDLK_ROW11;
	hotspots[HS_LDFILE_LIST12].remap_id = SDLK_ROW12;
	hotspots[HS_LDFILE_LIST13].remap_id = SDLK_ROW13;
	hotspots[HS_LDFILE_LIST14].remap_id = SDLK_ROW14;
	hotspots[HS_LDFILE_LIST15].remap_id = SDLK_ROW15;
	hotspots[HS_LDFILE_LIST16].remap_id = SDLK_ROW16;
	hotspots[HS_LDFILE_LIST17].remap_id = SDLK_ROW17;
	hotspots[HS_LDFILE_LIST18].remap_id = SDLK_ROW18;
	hotspots[HS_LDFILE_LIST19].remap_id = SDLK_ROW19;
	hotspots[HS_LDFILE_LOAD].remap_id = SDLK_ACCEPT;
	hotspots[HS_LDFILE_LOAD].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_LDFILE_EXIT].remap_id = SDLK_F3;

	/* Initialise save state dialog hotspots */
	hotspots[HS_SSTATE_SSTATE].flags &= ~HS_PROP_NAVIGABLE;
	for (count = HS_SSTATE_SSTATE; count <= HS_SSTATE_EXIT; count++) {
		hotspots[count].gid = HS_GRP_SSTATE;
	}
	hotspots[HS_SSTATE_SLOT0].remap_id = SDLK_1;
	hotspots[HS_SSTATE_SLOT0].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_SSTATE_SLOT1].remap_id = SDLK_2;
	hotspots[HS_SSTATE_SLOT2].remap_id = SDLK_3;
	hotspots[HS_SSTATE_SLOT3].remap_id = SDLK_4;
	hotspots[HS_SSTATE_SLOT4].remap_id = SDLK_5;
	hotspots[HS_SSTATE_SLOT5].remap_id = SDLK_6;
	hotspots[HS_SSTATE_SLOT6].remap_id = SDLK_7;
	hotspots[HS_SSTATE_SLOT7].remap_id = SDLK_8;
	hotspots[HS_SSTATE_SLOT8].remap_id = SDLK_9;
	hotspots[HS_SSTATE_EXIT].remap_id = SDLK_F4;

	/* Initialise virtual keyboard hotspots */
	hotspots[HS_VKEYB_VKEYB].gid = HS_GRP_VKEYB;
	hotspots[HS_VKEYB_VKEYB].flags &= ~HS_PROP_NAVIGABLE;
	for (count = 0; count < 10; count++) {
		hotspots[HS_VKEYB_1 + count].gid = HS_GRP_VKEYB;
		hotspots[HS_VKEYB_Q + count].gid = HS_GRP_VKEYB;
		hotspots[HS_VKEYB_A + count].gid = HS_GRP_VKEYB;
		hotspots[HS_VKEYB_SHIFT + count].gid = HS_GRP_VKEYB;
	}
	hotspots[HS_VKEYB_1].remap_id = SDLK_1;
	hotspots[HS_VKEYB_1 + 1].remap_id = SDLK_2;
	hotspots[HS_VKEYB_1 + 2].remap_id = SDLK_3;
	hotspots[HS_VKEYB_1 + 3].remap_id = SDLK_4;
	hotspots[HS_VKEYB_1 + 4].remap_id = SDLK_5;
	hotspots[HS_VKEYB_1 + 5].remap_id = SDLK_6;
	hotspots[HS_VKEYB_1 + 6].remap_id = SDLK_7;
	hotspots[HS_VKEYB_1 + 7].remap_id = SDLK_8;
	hotspots[HS_VKEYB_1 + 8].remap_id = SDLK_9;
	hotspots[HS_VKEYB_1 + 9].remap_id = SDLK_0;
	hotspots[HS_VKEYB_Q].remap_id = SDLK_q;
	hotspots[HS_VKEYB_Q + 1].remap_id = SDLK_w;
	if (*sdl_emulator.model == MODEL_ZX80) 
		hotspots[HS_VKEYB_Q + 1].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_VKEYB_Q + 2].remap_id = SDLK_e;
	hotspots[HS_VKEYB_Q + 3].remap_id = SDLK_r;
	hotspots[HS_VKEYB_Q + 4].remap_id = SDLK_t;
	hotspots[HS_VKEYB_Q + 5].remap_id = SDLK_y;
	hotspots[HS_VKEYB_Q + 6].remap_id = SDLK_u;
	hotspots[HS_VKEYB_Q + 7].remap_id = SDLK_i;
	hotspots[HS_VKEYB_Q + 8].remap_id = SDLK_o;
	hotspots[HS_VKEYB_Q + 9].remap_id = SDLK_p;
	hotspots[HS_VKEYB_A].remap_id = SDLK_a;
	hotspots[HS_VKEYB_A + 1].remap_id = SDLK_s;
	hotspots[HS_VKEYB_A + 2].remap_id = SDLK_d;
	hotspots[HS_VKEYB_A + 3].remap_id = SDLK_f;
	hotspots[HS_VKEYB_A + 4].remap_id = SDLK_g;
	hotspots[HS_VKEYB_A + 5].remap_id = SDLK_h;
	hotspots[HS_VKEYB_A + 6].remap_id = SDLK_j;
	if (*sdl_emulator.model == MODEL_ZX81) 
		hotspots[HS_VKEYB_A + 6].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_VKEYB_A + 7].remap_id = SDLK_k;
	hotspots[HS_VKEYB_A + 8].remap_id = SDLK_l;
	hotspots[HS_VKEYB_A + 9].remap_id = SDLK_RETURN;
	hotspots[HS_VKEYB_SHIFT].flags &= ~HS_PROP_ONOFF;

	hotspots_vkeyb_shift_init();

	hotspots[HS_VKEYB_SHIFT].remap_id = SDLK_LSHIFT;
	hotspots[HS_VKEYB_SHIFT + 1].remap_id = SDLK_z;
	hotspots[HS_VKEYB_SHIFT + 2].remap_id = SDLK_x;
	hotspots[HS_VKEYB_SHIFT + 3].remap_id = SDLK_c;
	hotspots[HS_VKEYB_SHIFT + 4].remap_id = SDLK_v;
	hotspots[HS_VKEYB_SHIFT + 5].remap_id = SDLK_b;
	hotspots[HS_VKEYB_SHIFT + 6].remap_id = SDLK_n;
	hotspots[HS_VKEYB_SHIFT + 7].remap_id = SDLK_m;
	hotspots[HS_VKEYB_SHIFT + 8].remap_id = SDLK_PERIOD;
	hotspots[HS_VKEYB_SHIFT + 9].remap_id = SDLK_SPACE;

	/* Initialise control bar hotspots */
	hotspots[HS_CTB_CTB].flags &= ~HS_PROP_NAVIGABLE;
	for (count = HS_CTB_CTB; count <= HS_CTB_RUNOPTS; count++) {
		hotspots[count].gid = HS_GRP_CTB;
	}
	hotspots[HS_CTB_EXIT].remap_id = SDLK_F10;
	hotspots[HS_CTB_RESET].remap_id = SDLK_F12;
	hotspots[HS_CTB_AUTOHIDE].remap_id = SDLK_F6;
	hotspots[HS_CTB_SHIFT_TYPE].remap_id = SDLK_F7;
	hotspots[HS_CTB_INVERSE].remap_id = SDLK_F8;
	hotspots[HS_CTB_ALPHA_DN].remap_id = SDLK_HOME;
	hotspots[HS_CTB_ALPHA_UP].remap_id = SDLK_END;
	hotspots[HS_CTB_LDFILE].remap_id = SDLK_F3;
	hotspots[HS_CTB_RUNOPTS].remap_id = SDLK_F2;

	/* Initialise runtime options hotspots */
	hotspots[HS_RUNOPTS0_RUNOPTS0].flags &= ~HS_PROP_NAVIGABLE;
	for (count = HS_RUNOPTS0_RUNOPTS0; count <= HS_RUNOPTS0_NEXT; count++) {
		hotspots[count].gid = HS_GRP_RUNOPTS0;
	}
	hotspots[HS_RUNOPTS0_ZX80].remap_id = SDLK_HOME;
	hotspots[HS_RUNOPTS0_ZX80].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_RUNOPTS0_ZX81].remap_id = SDLK_END;
	hotspots[HS_RUNOPTS0_RAM_DN].remap_id = SDLK_INSERT;
	hotspots[HS_RUNOPTS0_RAM_UP].remap_id = SDLK_DELETE;
	hotspots[HS_RUNOPTS0_FRAMESKIP_DN].remap_id = SDLK_LEFTBRACKET;
	hotspots[HS_RUNOPTS0_FRAMESKIP_UP].remap_id = SDLK_RIGHTBRACKET;
	#ifdef ENABLE_EMULATION_SPEED_ADJUST
		hotspots[HS_RUNOPTS0_SPEED_DN].remap_id = SDLK_1;
		hotspots[HS_RUNOPTS0_SPEED_UP].remap_id = SDLK_2;
	#endif
	hotspots[HS_RUNOPTS0_SAVE].remap_id = SDLK_ACCEPT;
	hotspots[HS_RUNOPTS0_EXIT].remap_id = SDLK_F2;
	hotspots[HS_RUNOPTS0_NEXT].remap_id = SDLK_PAGEDOWN;

	/* Initialise runtime options hotspots */
	hotspots[HS_RUNOPTS1_RUNOPTS1].flags &= ~HS_PROP_NAVIGABLE;
	for (count = HS_RUNOPTS1_RUNOPTS1; count <= HS_RUNOPTS1_NEXT; count++) {
		hotspots[count].gid = HS_GRP_RUNOPTS1;
	}
	hotspots[HS_RUNOPTS1_DEVICE_NONE].remap_id = SDLK_1;
	hotspots[HS_RUNOPTS1_DEVICE_NONE].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_RUNOPTS1_DEVICE_AY].remap_id = SDLK_2;
	hotspots[HS_RUNOPTS1_DEVICE_QUICKSILVA].remap_id = SDLK_3;
	hotspots[HS_RUNOPTS1_DEVICE_ZONX].remap_id = SDLK_4;
	hotspots[HS_RUNOPTS1_ACB_STEREO].remap_id = SDLK_5;
	hotspots[HS_RUNOPTS1_DEVICE_VSYNC].remap_id = SDLK_6;
	hotspots[HS_RUNOPTS1_BACK].remap_id = SDLK_PAGEUP;
	hotspots[HS_RUNOPTS1_SAVE].remap_id = SDLK_ACCEPT;
	hotspots[HS_RUNOPTS1_EXIT].remap_id = SDLK_F2;
	hotspots[HS_RUNOPTS1_NEXT].remap_id = SDLK_PAGEDOWN;

	/* Initialise runtime options hotspots */
	hotspots[HS_RUNOPTS2_RUNOPTS2].flags &= ~HS_PROP_NAVIGABLE;
	for (count = HS_RUNOPTS2_RUNOPTS2; count <= HS_RUNOPTS2_NEXT; count++) {
		hotspots[count].gid = HS_GRP_RUNOPTS2;
	}
	hotspots[HS_RUNOPTS2_VOLUME_DN].remap_id = SDLK_MINUS;
	hotspots[HS_RUNOPTS2_VOLUME_DN].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_RUNOPTS2_VOLUME_UP].remap_id = SDLK_EQUALS;
	hotspots[HS_RUNOPTS2_KRDELAY_DN].remap_id = SDLK_HOME;
	hotspots[HS_RUNOPTS2_KRDELAY_UP].remap_id = SDLK_END;
	hotspots[HS_RUNOPTS2_KRINTERVAL_DN].remap_id = SDLK_INSERT;
	hotspots[HS_RUNOPTS2_KRINTERVAL_UP].remap_id = SDLK_DELETE;
	hotspots[HS_RUNOPTS2_FGC_R_DN].remap_id = SDLK_1;
	hotspots[HS_RUNOPTS2_FGC_R_UP].remap_id = SDLK_2;
	hotspots[HS_RUNOPTS2_FGC_G_DN].remap_id = SDLK_3;
	hotspots[HS_RUNOPTS2_FGC_G_UP].remap_id = SDLK_4;
	hotspots[HS_RUNOPTS2_FGC_B_DN].remap_id = SDLK_5;
	hotspots[HS_RUNOPTS2_FGC_B_UP].remap_id = SDLK_6;
	hotspots[HS_RUNOPTS2_BGC_R_DN].remap_id = SDLK_7;
	hotspots[HS_RUNOPTS2_BGC_R_UP].remap_id = SDLK_8;
	hotspots[HS_RUNOPTS2_BGC_G_DN].remap_id = SDLK_9;
	hotspots[HS_RUNOPTS2_BGC_G_UP].remap_id = SDLK_0;
	hotspots[HS_RUNOPTS2_BGC_B_DN].remap_id = SDLK_LEFTBRACKET;
	hotspots[HS_RUNOPTS2_BGC_B_UP].remap_id = SDLK_RIGHTBRACKET;
	hotspots[HS_RUNOPTS2_BACK].remap_id = SDLK_PAGEUP;
	hotspots[HS_RUNOPTS2_SAVE].remap_id = SDLK_ACCEPT;
	hotspots[HS_RUNOPTS2_EXIT].remap_id = SDLK_F2;
	hotspots[HS_RUNOPTS2_NEXT].remap_id = SDLK_PAGEDOWN;

	/* Initialise runtime options hotspots */
	hotspots[HS_RUNOPTS3_RUNOPTS3].flags &= ~HS_PROP_NAVIGABLE;
	for (count = HS_RUNOPTS3_RUNOPTS3; count <= HS_RUNOPTS3_EXIT; count++) {
		hotspots[count].gid = HS_GRP_RUNOPTS3;
	}
	hotspots[HS_RUNOPTS3_JDEADZ_DN].remap_id = SDLK_HOME;
	hotspots[HS_RUNOPTS3_JDEADZ_DN].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_RUNOPTS3_JDEADZ_UP].remap_id = SDLK_END;
	hotspots[HS_RUNOPTS3_JOY_CFG_LTRIG].remap_id = SDLK_a;
	hotspots[HS_RUNOPTS3_JOY_CFG_RTRIG].remap_id = SDLK_b;
	hotspots[HS_RUNOPTS3_JOY_CFG_LEFT].remap_id = SDLK_c;
	hotspots[HS_RUNOPTS3_JOY_CFG_RIGHT].remap_id = SDLK_d;
	hotspots[HS_RUNOPTS3_JOY_CFG_UP].remap_id = SDLK_e;
	hotspots[HS_RUNOPTS3_JOY_CFG_DOWN].remap_id = SDLK_f;
	hotspots[HS_RUNOPTS3_JOY_CFG_SELECT].remap_id = SDLK_g;
	hotspots[HS_RUNOPTS3_JOY_CFG_START].remap_id = SDLK_h;
	hotspots[HS_RUNOPTS3_JOY_CFG_A].remap_id = SDLK_i;
	hotspots[HS_RUNOPTS3_JOY_CFG_B].remap_id = SDLK_j;
	hotspots[HS_RUNOPTS3_JOY_CFG_Y].remap_id = SDLK_k;
	hotspots[HS_RUNOPTS3_JOY_CFG_X].remap_id = SDLK_l;
	hotspots[HS_RUNOPTS3_BACK].remap_id = SDLK_PAGEUP;
	hotspots[HS_RUNOPTS3_SAVE].remap_id = SDLK_ACCEPT;
	hotspots[HS_RUNOPTS3_EXIT].remap_id = SDLK_F2;

	/* Resize all hotspots for the current screen dimensions */
	hotspots_resize(HS_GRP_ALL);

	/* Update all hotspots to reflect current component states */
	hotspots_update();
}

/***************************************************************************
 * Hotspots Virtual Keyboard Shift Initialise                              *
 ***************************************************************************/
/* This sets the necessary flags on the vkeyb's shift key */

void hotspots_vkeyb_shift_init(void) {
	if (vkeyb.toggle_shift) {
		hotspots[HS_VKEYB_SHIFT].flags &= ~HS_PROP_STICKY;
		hotspots[HS_VKEYB_SHIFT].flags |= HS_PROP_TOGGLE;
	} else {
		hotspots[HS_VKEYB_SHIFT].flags &= ~HS_PROP_TOGGLE;
		hotspots[HS_VKEYB_SHIFT].flags |= HS_PROP_STICKY;
	}
}

/***************************************************************************
 * Hotspots Resize                                                         *
 ***************************************************************************/
/* This repositions and resizes one or all groups of hotspots for the current
 * dimensions of the screen. It requires hotspots-init to have been called at
 * least once and it's called at sz81 start, when the screen is resized and
 * when the vkeyb is switched.
 * 
 * On entry: gid = on or more OR'd group IDs */

void hotspots_resize(int gid) {
	int count;

	if (gid & HS_GRP_EMU) {
		/* Resize root hotspots */
		hotspots[HS_EMU_EMU].hit_x = 0;
		hotspots[HS_EMU_EMU].hit_y = 0;
		hotspots[HS_EMU_EMU].hit_w = video.screen->w;
		hotspots[HS_EMU_EMU].hit_h = video.screen->h;
	}

	if (gid & HS_GRP_LOAD) {
		/* Resize the ZX81's file selector hotspots */
		if (*sdl_emulator.model == MODEL_ZX81) {
			hotspots[HS_LOAD_Q].hit_x = sdl_emulator.xoffset + 36 * video.scale;
			hotspots[HS_LOAD_Q].hit_y = sdl_emulator.yoffset + 176 * video.scale;
			hotspots[HS_LOAD_Q].hit_w = 40 * video.scale;
			hotspots[HS_LOAD_Q].hit_h = 16 * video.scale;
			hotspots[HS_LOAD_A].hit_x = sdl_emulator.xoffset + 76 * video.scale;
			hotspots[HS_LOAD_A].hit_y = sdl_emulator.yoffset + 176 * video.scale;
			hotspots[HS_LOAD_A].hit_w = 40 * video.scale;
			hotspots[HS_LOAD_A].hit_h = 16 * video.scale;
			hotspots[HS_LOAD_ENTER].hit_x = sdl_emulator.xoffset + 116 * video.scale;
			hotspots[HS_LOAD_ENTER].hit_y = sdl_emulator.yoffset + 176 * video.scale;
			hotspots[HS_LOAD_ENTER].hit_w = 88 * video.scale;
			hotspots[HS_LOAD_ENTER].hit_h = 16 * video.scale;
			hotspots[HS_LOAD_SPACE].hit_x = sdl_emulator.xoffset + 204 * video.scale;
			hotspots[HS_LOAD_SPACE].hit_y = sdl_emulator.yoffset + 176 * video.scale;
			hotspots[HS_LOAD_SPACE].hit_w = 80 * video.scale;
			hotspots[HS_LOAD_SPACE].hit_h = 16 * video.scale;
			/* Set-up hl_x/y/w/h */
			for (count = HS_LOAD_Q; count <= HS_LOAD_ENTER; count++) {
				hotspots[count].hl_x = hotspots[count].hit_x;
				hotspots[count].hl_y = hotspots[count].hit_y;
				hotspots[count].hl_w = hotspots[count].hit_w + video.scale;
				hotspots[count].hl_h = hotspots[count].hit_h;
			}
		}
	}

	if (gid & HS_GRP_LDFILE) {
		/* Resize load file dialog hotspots */
		hotspots[HS_LDFILE_LDFILE].hit_x = load_file_dialog.xoffset;
		hotspots[HS_LDFILE_LDFILE].hit_y = load_file_dialog.yoffset;
		hotspots[HS_LDFILE_LDFILE].hit_w = 256 * video.scale;
		hotspots[HS_LDFILE_LDFILE].hit_h = 192 * video.scale;
		for (count = 0; count <= 19; count++) {
			hotspots[HS_LDFILE_LIST00 + count].hit_x = load_file_dialog.xoffset;
			hotspots[HS_LDFILE_LIST00 + count].hit_y = 
				load_file_dialog.yoffset + (count + 2) * 8 * video.scale;
			if (count < load_file_dialog.dirlist_count) {
				hotspots[HS_LDFILE_LIST00 + count].hit_w = 
					strlen(load_file_dialog.dirlist + load_file_dialog.dirlist_sizeof * 
					(load_file_dialog.dirlist_top + count)) * 8 * video.scale;
				if (hotspots[HS_LDFILE_LIST00 + count].hit_w > 32 * 8 * video.scale)
					hotspots[HS_LDFILE_LIST00 + count].hit_w = 32 * 8 * video.scale;
				hotspots[HS_LDFILE_LIST00 + count].hit_h = 1 * 8 * video.scale;
			} else {
				hotspots[HS_LDFILE_LIST00 + count].hit_w = 0;
				hotspots[HS_LDFILE_LIST00 + count].hit_h = 0;
			}
		}
		hotspots[HS_LDFILE_LOAD].hit_x = load_file_dialog.xoffset + 10 * 8 * video.scale;
		hotspots[HS_LDFILE_LOAD].hit_y = load_file_dialog.yoffset + 22.5 * 8 * video.scale;
		hotspots[HS_LDFILE_LOAD].hit_w = 4 * 8 * video.scale;
		hotspots[HS_LDFILE_LOAD].hit_h = 2 * 8 * video.scale;
		hotspots[HS_LDFILE_EXIT].hit_x = load_file_dialog.xoffset + 18 * 8 * video.scale;
		hotspots[HS_LDFILE_EXIT].hit_y = load_file_dialog.yoffset + 22.5 * 8 * video.scale;
		hotspots[HS_LDFILE_EXIT].hit_w = 4 * 8 * video.scale;
		hotspots[HS_LDFILE_EXIT].hit_h = 2 * 8 * video.scale;
		/* Set-up hl_x/y/w/h */
		for (count = HS_LDFILE_LOAD; count <= HS_LDFILE_EXIT; count++) {
			hotspots[count].hl_x = hotspots[count].hit_x;
			hotspots[count].hl_y = hotspots[count].hit_y + 0.5 * 8 * video.scale;
			hotspots[count].hl_w = hotspots[count].hit_w;
			hotspots[count].hl_h = hotspots[count].hit_h - 1 * 8 * video.scale;
		}
	}

	if (gid & HS_GRP_SSTATE) {
		/* Resize save state dialog hotspots */
		hotspots[HS_SSTATE_SSTATE].hit_x = save_state_dialog.xoffset;
		hotspots[HS_SSTATE_SSTATE].hit_y = save_state_dialog.yoffset;
		hotspots[HS_SSTATE_SSTATE].hit_w = 14 * 8 * video.scale;
		hotspots[HS_SSTATE_SSTATE].hit_h = 16.5 * 8 * video.scale;
		for (count = 0; count < 3; count++) {
			hotspots[HS_SSTATE_SLOT0 + count].hit_x = 
				hotspots[HS_SSTATE_SLOT3 + count].hit_x = 
				hotspots[HS_SSTATE_SLOT6 + count].hit_x = 
				save_state_dialog.xoffset + 4 * video.scale + 
				count * 4.5 * 8 * video.scale;
			hotspots[HS_SSTATE_SLOT0 + count * 3].hit_y = 
				hotspots[HS_SSTATE_SLOT1 + count * 3].hit_y = 
				hotspots[HS_SSTATE_SLOT2 + count * 3].hit_y = 
				save_state_dialog.yoffset + 1.5 * 8 * video.scale + 
				count * 4.5 * 8 * video.scale;
			hotspots[HS_SSTATE_SLOT0 + count].hit_w = 
				hotspots[HS_SSTATE_SLOT3 + count].hit_w = 
				hotspots[HS_SSTATE_SLOT6 + count].hit_w = 
				hotspots[HS_SSTATE_SLOT0 + count].hit_h = 
				hotspots[HS_SSTATE_SLOT3 + count].hit_h = 
				hotspots[HS_SSTATE_SLOT6 + count].hit_h =
				4 * 8 * video.scale;
		}
		hotspots[HS_SSTATE_EXIT].hit_x = save_state_dialog.xoffset + 5 * 8 * video.scale;
		hotspots[HS_SSTATE_EXIT].hit_y = save_state_dialog.yoffset + 14.5 * 8 * video.scale;
		hotspots[HS_SSTATE_EXIT].hit_w = 4 * 8 * video.scale;
		hotspots[HS_SSTATE_EXIT].hit_h = 2 * 8 * video.scale;
		/* Set-up hl_x/y/w/h */
		for (count = HS_SSTATE_EXIT; count <= HS_SSTATE_EXIT; count++) {
			hotspots[count].hl_x = hotspots[count].hit_x;
			hotspots[count].hl_y = hotspots[count].hit_y + 0.5 * 8 * video.scale;
			hotspots[count].hl_w = hotspots[count].hit_w;
			hotspots[count].hl_h = hotspots[count].hit_h - 1 * 8 * video.scale;
		}
	}

	if (gid & HS_GRP_VKEYB) {
		/* Resize the virtual keyboard hotspots */
		hotspots[HS_VKEYB_VKEYB].hit_x = vkeyb.xoffset;
		hotspots[HS_VKEYB_VKEYB].hit_y = vkeyb.yoffset;
		hotspots[HS_VKEYB_VKEYB].hit_w = vkeyb.scaled->w;
		hotspots[HS_VKEYB_VKEYB].hit_h = vkeyb.scaled->h;
		if (*sdl_emulator.model == MODEL_ZX80) {
			for (count = 0; count < 40; count++) {
				hotspots[HS_VKEYB_1 + count].hit_x = vkeyb.xoffset + 9 *
					video.scale + (count % 10) * 24 * video.scale;
				hotspots[HS_VKEYB_1 + count].hit_y = vkeyb.yoffset + 33 *
					video.scale + (count / 10) * 26 * video.scale;
				hotspots[HS_VKEYB_1 + count].hit_w = 22 * video.scale;
				hotspots[HS_VKEYB_1 + count].hit_h = 16 * video.scale;
			}
		} else {
			for (count = 0; count < 40; count++) {
				hotspots[HS_VKEYB_1 + count].hit_x = vkeyb.xoffset + 9 *
					video.scale + (count % 10) * 24 * video.scale;
				hotspots[HS_VKEYB_1 + count].hit_y = vkeyb.yoffset + 9 *
					video.scale + (count / 10) * 32 * video.scale;
				hotspots[HS_VKEYB_1 + count].hit_w = 22 * video.scale;
				hotspots[HS_VKEYB_1 + count].hit_h = 16 * video.scale;
			}
		}
	}

	if (gid & HS_GRP_CTB) {
		/* Resize the control bar hotspots */
		hotspots[HS_CTB_CTB].hit_x = control_bar.xoffset;
		hotspots[HS_CTB_CTB].hit_y = control_bar.yoffset;
		hotspots[HS_CTB_CTB].hit_w = control_bar.scaled->w;
		hotspots[HS_CTB_CTB].hit_h = control_bar.scaled->h;
		/* Set-up hit_x */
		hotspots[HS_CTB_EXIT].hit_x = control_bar.xoffset + video.scale;
		hotspots[HS_CTB_RESET].hit_x = control_bar.xoffset + video.scale + 17 * 2 * video.scale;
		hotspots[HS_CTB_AUTOHIDE].hit_x = control_bar.xoffset + video.scale + 17 * 4 * video.scale;
		hotspots[HS_CTB_SHIFT_TYPE].hit_x = control_bar.xoffset + video.scale + 17 * 5 * video.scale;
		hotspots[HS_CTB_INVERSE].hit_x = control_bar.xoffset + video.scale + 17 * 6 * video.scale;
		hotspots[HS_CTB_ALPHA_DN].hit_x = control_bar.xoffset + video.scale + 17 * 7 * video.scale;
		hotspots[HS_CTB_ALPHA_UP].hit_x = control_bar.xoffset + video.scale + 17 * 8 * video.scale;
		hotspots[HS_CTB_LDFILE].hit_x = control_bar.xoffset + video.scale + 17 * 12 * video.scale;
		hotspots[HS_CTB_RUNOPTS].hit_x = control_bar.xoffset + video.scale + 17 * 13 * video.scale;
		/* Set-up hit_y/w/h */
		for (count = HS_CTB_EXIT; count <= HS_CTB_RUNOPTS; count++) {
			hotspots[count].hit_y = control_bar.yoffset + video.scale;
			hotspots[count].hit_w = hotspots[count].hit_h = 16 * video.scale;
		}
		/* Set-up hl_x/y/w/h */
		for (count = HS_CTB_EXIT; count <= HS_CTB_RUNOPTS; count++) {
			hotspots[count].hl_x = hotspots[count].hit_x + video.scale;
			hotspots[count].hl_y = hotspots[count].hit_y + video.scale;
			hotspots[count].hl_w = hotspots[count].hit_w - 2 * video.scale;
			hotspots[count].hl_h = hotspots[count].hit_h - 2 * video.scale;
		}
	}

	if (gid & HS_GRP_RUNOPTS_ALL) {
		/* Resize runtime options hotspots */
		for (count = HS_RUNOPTS0_RUNOPTS0; count <= HS_RUNOPTS0_NEXT; count++) {
			hotspots[count].hit_x = runtime_options[0].xoffset;
			hotspots[count].hit_y = runtime_options[0].yoffset;
		}
		hotspots[HS_RUNOPTS0_RUNOPTS0].hit_w = 256 * video.scale;
		hotspots[HS_RUNOPTS0_RUNOPTS0].hit_h = 192 * video.scale;
		hotspots[HS_RUNOPTS0_ZX80].hit_x += 2.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_ZX80].hit_y += 3.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_ZX80].hit_w = 7.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_ZX81].hit_x += 12.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_ZX81].hit_y += 3.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_ZX81].hit_w = 7.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_RAM_DN].hit_x += 9.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_RAM_DN].hit_y += 5.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_RAM_UP].hit_x += 16.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_RAM_UP].hit_y += 5.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_FRAMESKIP_DN].hit_x += 9.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_FRAMESKIP_DN].hit_y += 7.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_FRAMESKIP_UP].hit_x += 16.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_FRAMESKIP_UP].hit_y += 7.5 * 8 * video.scale;
		#ifdef ENABLE_EMULATION_SPEED_ADJUST
			hotspots[HS_RUNOPTS0_SPEED_DN].hit_x += 9.5 * 8 * video.scale;
			hotspots[HS_RUNOPTS0_SPEED_DN].hit_y += 9.5 * 8 * video.scale;
			hotspots[HS_RUNOPTS0_SPEED_UP].hit_x += 16.5 * 8 * video.scale;
			hotspots[HS_RUNOPTS0_SPEED_UP].hit_y += 9.5 * 8 * video.scale;
		#endif
		hotspots[HS_RUNOPTS0_SAVE].hit_x += 10 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_SAVE].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_SAVE].hit_w = 4 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_EXIT].hit_x += 18 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_EXIT].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_EXIT].hit_w = 4 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_NEXT].hit_x += 25 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_NEXT].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS0_NEXT].hit_w = 6.5 * 8 * video.scale;
		/* Set-up hit_w/h */
		for (count = HS_RUNOPTS0_ZX80; count <= HS_RUNOPTS0_NEXT; count++) {
			hotspots[count].hit_h = 2 * 8 * video.scale;
			if (count >= HS_RUNOPTS0_RAM_DN && count < HS_RUNOPTS0_SAVE) 
				hotspots[count].hit_w = 2 * 8 * video.scale;
		}
		/* Set-up hl_x/y/w/h */
		for (count = HS_RUNOPTS0_ZX80; count <= HS_RUNOPTS0_NEXT; count++) {
			hotspots[count].hl_x = hotspots[count].hit_x;
			hotspots[count].hl_y = hotspots[count].hit_y;
			hotspots[count].hl_w = hotspots[count].hit_w;
			hotspots[count].hl_h = hotspots[count].hit_h;
		}
		for (count = HS_RUNOPTS0_ZX80; count <= HS_RUNOPTS0_NEXT; count++) {
			hotspots[count].hl_y += 0.5 * 8 * video.scale;
			hotspots[count].hl_h -= 1 * 8 * video.scale;
		}

		/* Resize runtime options hotspots */
		for (count = HS_RUNOPTS1_RUNOPTS1; count <= HS_RUNOPTS1_NEXT; count++) {
			hotspots[count].hit_x = runtime_options[1].xoffset;
			hotspots[count].hit_y = runtime_options[1].yoffset;
		}
		hotspots[HS_RUNOPTS1_RUNOPTS1].hit_w = 256 * video.scale;
		hotspots[HS_RUNOPTS1_RUNOPTS1].hit_h = 192 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_NONE].hit_x += 2.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_NONE].hit_y += 3.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_NONE].hit_w = 7.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_AY].hit_x += 2.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_AY].hit_y += 5.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_AY].hit_w = 16.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_QUICKSILVA].hit_x += 5.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_QUICKSILVA].hit_y += 7.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_QUICKSILVA].hit_w = 25.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_ZONX].hit_x += 5.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_ZONX].hit_y += 9.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_ZONX].hit_w = 18.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_ACB_STEREO].hit_x += 5.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_ACB_STEREO].hit_y += 11.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_ACB_STEREO].hit_w = 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_VSYNC].hit_x += 2.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_VSYNC].hit_y += 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_DEVICE_VSYNC].hit_w = 21.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_BACK].hit_x += 0.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_BACK].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_BACK].hit_w = 6.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_SAVE].hit_x += 10 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_SAVE].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_SAVE].hit_w = 4 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_EXIT].hit_x += 18 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_EXIT].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_EXIT].hit_w = 4 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_NEXT].hit_x += 25 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_NEXT].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS1_NEXT].hit_w = 6.5 * 8 * video.scale;
		/* Set-up hit_w/h */
		for (count = HS_RUNOPTS1_DEVICE_NONE; count <= HS_RUNOPTS1_NEXT; count++) {
			hotspots[count].hit_h = 2 * 8 * video.scale;
		}
		/* Set-up hl_x/y/w/h */
		for (count = HS_RUNOPTS1_DEVICE_NONE; count <= HS_RUNOPTS1_NEXT; count++) {
			hotspots[count].hl_x = hotspots[count].hit_x;
			hotspots[count].hl_y = hotspots[count].hit_y;
			hotspots[count].hl_w = hotspots[count].hit_w;
			hotspots[count].hl_h = hotspots[count].hit_h;
		}
		for (count = HS_RUNOPTS1_DEVICE_NONE; count <= HS_RUNOPTS1_NEXT; count++) {
			hotspots[count].hl_y += 0.5 * 8 * video.scale;
			hotspots[count].hl_h -= 1 * 8 * video.scale;
		}

		/* Resize runtime options hotspots */
		for (count = HS_RUNOPTS2_RUNOPTS2; count <= HS_RUNOPTS2_NEXT; count++) {
			hotspots[count].hit_x = runtime_options[2].xoffset;
			hotspots[count].hit_y = runtime_options[2].yoffset;
		}
		hotspots[HS_RUNOPTS2_RUNOPTS2].hit_w = 256 * video.scale;
		hotspots[HS_RUNOPTS2_RUNOPTS2].hit_h = 192 * video.scale;
		hotspots[HS_RUNOPTS2_VOLUME_DN].hit_x += 10.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_VOLUME_DN].hit_y += 1.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_VOLUME_UP].hit_x += 18.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_VOLUME_UP].hit_y += 1.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_KRDELAY_DN].hit_x += 10.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_KRDELAY_DN].hit_y += 5.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_KRDELAY_UP].hit_x += 18.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_KRDELAY_UP].hit_y += 5.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_KRINTERVAL_DN].hit_x += 10.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_KRINTERVAL_DN].hit_y += 7.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_KRINTERVAL_UP].hit_x += 18.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_KRINTERVAL_UP].hit_y += 7.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_R_DN].hit_x += 8.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_R_DN].hit_y += 11.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_R_UP].hit_x += 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_R_UP].hit_y += 11.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_G_DN].hit_x += 8.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_G_DN].hit_y += 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_G_UP].hit_x += 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_G_UP].hit_y += 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_B_DN].hit_x += 8.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_B_DN].hit_y += 15.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_B_UP].hit_x += 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_FGC_B_UP].hit_y += 15.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_R_DN].hit_x += 23.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_R_DN].hit_y += 11.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_R_UP].hit_x += 28.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_R_UP].hit_y += 11.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_G_DN].hit_x += 23.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_G_DN].hit_y += 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_G_UP].hit_x += 28.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_G_UP].hit_y += 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_B_DN].hit_x += 23.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_B_DN].hit_y += 15.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_B_UP].hit_x += 28.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BGC_B_UP].hit_y += 15.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BACK].hit_x += 0.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BACK].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_BACK].hit_w = 6.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_SAVE].hit_x += 10 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_SAVE].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_SAVE].hit_w = 4 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_EXIT].hit_x += 18 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_EXIT].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_EXIT].hit_w = 4 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_NEXT].hit_x += 25 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_NEXT].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS2_NEXT].hit_w = 6.5 * 8 * video.scale;
		/* Set-up hit_w/h */
		for (count = HS_RUNOPTS2_VOLUME_DN; count <= HS_RUNOPTS2_NEXT; count++) {
			hotspots[count].hit_h = 2 * 8 * video.scale;
			if (count >= HS_RUNOPTS2_VOLUME_DN && count < HS_RUNOPTS2_BACK) 
				hotspots[count].hit_w = 2 * 8 * video.scale;
		}
		/* Set-up hl_x/y/w/h */
		for (count = HS_RUNOPTS2_VOLUME_DN; count <= HS_RUNOPTS2_NEXT; count++) {
			hotspots[count].hl_x = hotspots[count].hit_x;
			hotspots[count].hl_y = hotspots[count].hit_y;
			hotspots[count].hl_w = hotspots[count].hit_w;
			hotspots[count].hl_h = hotspots[count].hit_h;
		}
		for (count = HS_RUNOPTS2_VOLUME_DN; count <= HS_RUNOPTS2_NEXT; count++) {
			hotspots[count].hl_y += 0.5 * 8 * video.scale;
			hotspots[count].hl_h -= 1 * 8 * video.scale;
		}

		/* Resize runtime options hotspots */
		for (count = HS_RUNOPTS3_RUNOPTS3; count <= HS_RUNOPTS3_EXIT; count++) {
			hotspots[count].hit_x = runtime_options[3].xoffset;
			hotspots[count].hit_y = runtime_options[3].yoffset;
		}
		hotspots[HS_RUNOPTS3_RUNOPTS3].hit_w = 256 * video.scale;
		hotspots[HS_RUNOPTS3_RUNOPTS3].hit_h = 192 * video.scale;
		hotspots[HS_RUNOPTS3_JDEADZ_DN].hit_x += 14.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JDEADZ_DN].hit_y += 1.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JDEADZ_DN].hit_w = 2 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JDEADZ_UP].hit_x += 20.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JDEADZ_UP].hit_y += 1.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JDEADZ_UP].hit_w = 2 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_LTRIG].hit_x += 4 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_LTRIG].hit_y += 8.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_LTRIG].hit_w = 3 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_RTRIG].hit_x += 15 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_RTRIG].hit_y += 8.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_RTRIG].hit_w = 3 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_LEFT].hit_x += 3 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_LEFT].hit_y += 12.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_RIGHT].hit_x += 6 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_RIGHT].hit_y += 12.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_UP].hit_x += 4.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_UP].hit_y += 11 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_DOWN].hit_x += 4.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_DOWN].hit_y += 14 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_SELECT].hit_x += 8.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_SELECT].hit_y += 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_START].hit_x += 11.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_START].hit_y += 13.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_A].hit_x += 14 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_A].hit_y += 12.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_B].hit_x += 17 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_B].hit_y += 12.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_Y].hit_x += 15.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_Y].hit_y += 11 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_X].hit_x += 15.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JOY_CFG_X].hit_y += 14 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_BACK].hit_x += 0.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_BACK].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_BACK].hit_w = 6.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_SAVE].hit_x += 10 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_SAVE].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_SAVE].hit_w = 4 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_EXIT].hit_x += 18 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_EXIT].hit_y += 22.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_EXIT].hit_w = 4 * 8 * video.scale;
		/* Set-up hit_w/h */
		for (count = HS_RUNOPTS3_JDEADZ_DN; count <= HS_RUNOPTS3_EXIT; count++) {
			hotspots[count].hit_h = 2 * 8 * video.scale;
			if (count >= HS_RUNOPTS3_JOY_CFG_LEFT && count < HS_RUNOPTS3_BACK) 
				hotspots[count].hit_w = 2 * 8 * video.scale;
		}
		/* Set-up hl_x/y/w/h */
		for (count = HS_RUNOPTS3_JDEADZ_DN; count <= HS_RUNOPTS3_EXIT; count++) {
			hotspots[count].hl_x = hotspots[count].hit_x;
			hotspots[count].hl_y = hotspots[count].hit_y;
			hotspots[count].hl_w = hotspots[count].hit_w;
			hotspots[count].hl_h = hotspots[count].hit_h;
		}
		hotspots[HS_RUNOPTS3_JDEADZ_DN].hl_y += 0.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JDEADZ_DN].hl_h -= 1 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JDEADZ_UP].hl_y += 0.5 * 8 * video.scale;
		hotspots[HS_RUNOPTS3_JDEADZ_UP].hl_h -= 1 * 8 * video.scale;
		for (count = HS_RUNOPTS3_BACK; count <= HS_RUNOPTS3_EXIT; count++) {
			hotspots[count].hl_y += 0.5 * 8 * video.scale;
			hotspots[count].hl_h -= 1 * 8 * video.scale;
		}
	}
}

/***************************************************************************
 * Hotspots Update                                                         *
 ***************************************************************************/
/* This sets hotspots [in]visible depending on the states of the components
 * that affect them */

void hotspots_update(void) {
	int count, index;
	
	/* Update the ZX81's file selector hotspots */
	if (load_selector_state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_LOAD) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_LOAD) hotspots[count].flags &= ~HS_PROP_VISIBLE;
	}

	/* Update the load file dialog hotspots */
	if (load_file_dialog.state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_LDFILE) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_LDFILE) hotspots[count].flags &= ~HS_PROP_VISIBLE;
	}

	/* Update the save state dialog hotspots */
	if (save_state_dialog.state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_SSTATE) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_SSTATE) hotspots[count].flags &= ~HS_PROP_VISIBLE;
	}

	/* Update the virtual keyboard hotspots */
	if (vkeyb.state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_VKEYB) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_VKEYB) hotspots[count].flags &= ~HS_PROP_VISIBLE;
	}

	/* Update the control bar hotspots */
	if (vkeyb.state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_CTB) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_CTB) hotspots[count].flags &= ~HS_PROP_VISIBLE;
	}

	/* Update the runtime options hotspots */
	for (index = 0; index < MAX_RUNTIME_OPTIONS; index++) {
		if (runtime_options[index].state) {
			for (count = 0; count < MAX_HOTSPOTS; count++)
				if (hotspots[count].gid == (HS_GRP_RUNOPTS0 << index))
					hotspots[count].flags |= HS_PROP_VISIBLE;
		} else {
			for (count = 0; count < MAX_HOTSPOTS; count++)
				if (hotspots[count].gid == (HS_GRP_RUNOPTS0 << index))
					hotspots[count].flags &= ~HS_PROP_VISIBLE;
		}
	}
}

/***************************************************************************
 * Hotspots Render                                                         *
 ***************************************************************************/
/* This renders visible hotspots over everything else and is called towards
 * the end of sdl-video-refresh. What the hotspot will look like is decided
 * here and depends especially on the gid and occasionally on the flags */

void hotspots_render(void) {
	int hl_x, hl_y, hl_w, hl_h;
	int count, surround, alpha;
	SDL_Surface *highlight;
	int selected, pressed;
	SDL_Rect dstrect;
	Uint32 colour;

	/* Highlight any hotspots that are currently visible that have IDs
	 * that are recorded as being pressed, and selected hotspots if applicable */
	for (count = 0; count < MAX_HOTSPOTS; count++) {
		if (hotspots[count].gid != UNDEFINED && hotspots[count].flags & HS_PROP_VISIBLE &&
			hotspots[count].gid != HS_GRP_EMU && hotspots[count].remap_id != UNDEFINED) {

			pressed = keyboard_buffer[hotspots[count].remap_id];
			selected = hotspots[count].flags & HS_PROP_SELECTED;

			/* If the control remapper state is active then blink the selected highlight */
			if (selected && ctrl_remapper.state && ctrl_remapper.interval < 
				ctrl_remapper.master_interval / 2) selected = FALSE;

		#ifndef SDL_DEBUG_HOTSPOTS
			if (pressed || (joystick && selected) || 
				(hotspots[count].gid & HS_GRP_RUNOPTS_ALL && selected)) {
		#endif
				/* Set the size of the hotspot's highlight.
				 * If hl_x/y/w/h are all UNDEFINED then use hit_x/y/w/h instead */
				if (hotspots[count].hl_x == UNDEFINED && hotspots[count].hl_y == UNDEFINED &&
					hotspots[count].hl_w == UNDEFINED && hotspots[count].hl_h == UNDEFINED) {
					hl_x = hotspots[count].hit_x; hl_y = hotspots[count].hit_y;
					hl_w = hotspots[count].hit_w; hl_h = hotspots[count].hit_h;
				} else {
					hl_x = hotspots[count].hl_x; hl_y = hotspots[count].hl_y;
					hl_w = hotspots[count].hl_w; hl_h = hotspots[count].hl_h;
				}
				/* Choose a suitable colour (pressed overrides selected) */
				if (hotspots[count].gid == HS_GRP_LOAD || 
					hotspots[count].gid == HS_GRP_LDFILE || 
					hotspots[count].gid == HS_GRP_SSTATE) {
					if (pressed) {
						colour = colours.hs_load_pressed;
					} else {
						colour = colours.hs_load_selected;
					}
				} else if (hotspots[count].gid == HS_GRP_VKEYB) {
					if (hotspots[count].flags & HS_PROP_TOGGLE &&
						hotspots[count].flags & HS_PROP_STUCK) {
						/* If it's stuck it must be pressed */
						if (*sdl_emulator.model == MODEL_ZX80) {
							colour = colours.hs_vkeyb_zx80_toggle_pressed;
						} else {
							colour = colours.hs_vkeyb_zx81_toggle_pressed;
						}
					} else {
						if (pressed) {
							if (*sdl_emulator.model == MODEL_ZX80) {
								colour = colours.hs_vkeyb_zx80_pressed;
							} else {
								colour = colours.hs_vkeyb_zx81_pressed;
							}
						} else {
							if (*sdl_emulator.model == MODEL_ZX80) {
								colour = colours.hs_vkeyb_zx80_selected;
							} else {
								colour = colours.hs_vkeyb_zx81_selected;
							}
						}
					}
				} else if (hotspots[count].gid == HS_GRP_CTB) {
					if (pressed) {
						colour = colours.hs_ctb_pressed;
					} else {
						colour = colours.hs_ctb_selected;
					}
				} else if (hotspots[count].gid & HS_GRP_RUNOPTS_ALL) {
					if (pressed) {
						colour = colours.hs_options_pressed;
					} else {
						colour = colours.hs_options_selected;
					}
				} else {
					colour = UNDEFINED;
				}
				#ifdef SDL_DEBUG_HOTSPOTS	/* Ooh, pretty Christmas Lights Edition(tm) ;) */
					colour = (rand() % 256) << 16 | (rand() % 256) << 8 | rand() % 256;
				#endif
				/* Create an RGB surface to accommodate the highlight */
				highlight = SDL_CreateRGBSurface(SDL_SWSURFACE, hl_w, hl_h,
					video.screen->format->BitsPerPixel,
					video.screen->format->Rmask, video.screen->format->Gmask,
					video.screen->format->Bmask, video.screen->format->Amask);
				if (highlight == NULL) {
					fprintf(stderr, "%s: Cannot create RGB surface: %s\n", __func__, 
						SDL_GetError ());
					exit(1);
				}
				/* Fill the highlight */
				if (SDL_FillRect(highlight, NULL, SDL_MapRGB(video.screen->format,
					colour >> 16 & 0xff, colour >> 8 & 0xff, colour & 0xff)) < 0) {
					fprintf(stderr, "%s: FillRect error: %s\n", __func__,
						SDL_GetError ());
					exit(1);
				}
				/* For the vkeyb make the hotspot corners transparent */
				if (hotspots[count].gid == HS_GRP_VKEYB) {
					colour = SDL_MapRGB(video.screen->format, colours.colour_key >> 16 & 0xff,
						colours.colour_key >> 8 & 0xff, colours.colour_key & 0xff);
					dstrect.w = video.scale; dstrect.h = video.scale;
					for (surround = 0; surround < 4; surround++) {
						if (surround == 0) {
							dstrect.x = 0; dstrect.y = 0;
						} else if (surround == 1) {
							dstrect.x = hl_w - video.scale; dstrect.y = 0;
						} else if (surround == 2) {
							dstrect.x = hl_w - video.scale; dstrect.y = hl_h - video.scale;
						} else if (surround == 3) {
							dstrect.x = 0; dstrect.y = hl_h - video.scale;
						}
						if (SDL_FillRect(highlight, &dstrect, colour) < 0) {
							fprintf(stderr, "%s: FillRect error: %s\n", __func__,
								SDL_GetError ());
							exit(1);
						}
					}
					/* Set the transparent colour */
					if (SDL_SetColorKey (highlight, SDL_SRCCOLORKEY, colour) < 0) {
						fprintf(stderr, "%s: Cannot set surface colour key: %s\n", __func__,
							SDL_GetError ());
						exit(1);
					}
				}
				/* Choose a suitable alpha */
				if (hotspots[count].gid == HS_GRP_VKEYB) {
					alpha = 128 - (256 - vkeyb.alpha) / 4;
				} else {
					alpha = 128;
				}
				/* Set the alpha for the entire surface */
				if (SDL_SetAlpha(highlight, SDL_SRCALPHA, alpha) < 0) {
					fprintf(stderr, "%s: Cannot set surface alpha: %s\n", __func__,
						SDL_GetError());
					exit(1);
				}
				/* Blit it to the screen */
				dstrect.x = hl_x; dstrect.y = hl_y; dstrect.w = hl_w; dstrect.h = hl_h;
				if (SDL_BlitSurface (highlight, NULL, video.screen, &dstrect) < 0) {
					fprintf(stderr, "%s: BlitSurface error: %s\n", __func__,
						SDL_GetError ());
					exit(1);
				}
				SDL_FreeSurface(highlight);
				/* For the load selector, add interior left and right
				 * solid bars using the current background colour */
				if (hotspots[count].gid == HS_GRP_LOAD) {
					if (!sdl_emulator.invert) {
						colour = colours.emu_bg;
					} else {
						colour = colours.emu_fg;
					}
					#ifdef SDL_DEBUG_HOTSPOTS
						colour = (rand() % 256) << 16 | (rand() % 256) << 8 | rand() % 256;
					#endif
					dstrect.w = video.scale; dstrect.h = hl_h;
					for (surround = 0; surround < 2; surround++) {
						if (surround == 0) {
							dstrect.x = hl_x; dstrect.y = hl_y;
						} else {
							dstrect.x = hl_x + hl_w - video.scale; dstrect.y = hl_y;
						}
						if (SDL_FillRect(video.screen, &dstrect, SDL_MapRGB(video.screen->format,
							colour >> 16 & 0xff, colour >> 8 & 0xff, colour & 0xff)) < 0) {
							fprintf(stderr, "%s: FillRect error: %s\n", __func__,
								SDL_GetError ());
							exit(1);
						}
					}
				}
		#ifndef SDL_DEBUG_HOTSPOTS
			}
		#endif
		}
	}
}

/***************************************************************************
 * Get Selected Hotspot                                                    *
 ***************************************************************************/
/* This returns the currently selected hotspot for group visible or not.
 * 
 * On entry: gid = group id
 *  On exit: returns the currently selected hotspot for group on success or
 *           returns MAX_HOTSPOTS if not found */

int get_selected_hotspot(int gid) {
	int count;

	for (count = 0; count < MAX_HOTSPOTS; count++)
		if (hotspots[count].gid == gid && 
			hotspots[count].flags & HS_PROP_SELECTED) break;
	
	return count;
}



