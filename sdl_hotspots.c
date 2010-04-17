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

void hotspots_init(void) {
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
	hotspots[HS_ROOT_CANVAS].gid = HS_GRP_ROOT;
	hotspots[HS_ROOT_CANVAS].flags &= ~HS_PROP_NAVIGABLE;
	hotspots[HS_ROOT_CANVAS].remap_id = SDLK_F1;

	/* Initialise file selector hotspots */
	for (count = HS_LOAD_Q; count <= HS_LOAD_SPACE; count++) {
		hotspots[count].gid = HS_GRP_LOAD;
	}
	hotspots[HS_LOAD_Q].remap_id = SDLK_q;
	hotspots[HS_LOAD_A].remap_id = SDLK_a;
	hotspots[HS_LOAD_ENTER].remap_id = SDLK_RETURN;
	hotspots[HS_LOAD_ENTER].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_LOAD_SPACE].remap_id = SDLK_SPACE;

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
	if (zx80) hotspots[HS_VKEYB_Q + 1].flags |= HS_PROP_SELECTED;	/* Default selected */
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
	if (!zx80) hotspots[HS_VKEYB_A + 6].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_VKEYB_A + 7].remap_id = SDLK_k;
	hotspots[HS_VKEYB_A + 8].remap_id = SDLK_l;
	hotspots[HS_VKEYB_A + 9].remap_id = SDLK_RETURN;
	hotspots[HS_VKEYB_SHIFT].flags &= ~HS_PROP_ONOFF;
	if (vkeyb.toggle_shift) {
		hotspots[HS_VKEYB_SHIFT].flags |= HS_PROP_TOGGLE;
	} else {
		hotspots[HS_VKEYB_SHIFT].flags |= HS_PROP_STICKY;
	}
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
	hotspots[HS_CTB_RUNOPTS].remap_id = SDLK_ESCAPE;

	/* Initialise runtime options hotspots */
	hotspots[HS_RUNOPTS0_RUNOPTS0].flags &= ~HS_PROP_NAVIGABLE;
	for (count = HS_RUNOPTS0_RUNOPTS0; count <= HS_RUNOPTS0_NEXT; count++) {
		hotspots[count].gid = HS_GRP_RUNOPT0;
	}
	hotspots[HS_RUNOPTS0_VOLUME_DN].remap_id = SDLK_MINUS;
	hotspots[HS_RUNOPTS0_VOLUME_DN].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_RUNOPTS0_VOLUME_UP].remap_id = SDLK_EQUALS;
	hotspots[HS_RUNOPTS0_KRDELAY_DN].remap_id = SDLK_HOME;
	hotspots[HS_RUNOPTS0_KRDELAY_UP].remap_id = SDLK_END;
	hotspots[HS_RUNOPTS0_KRINTERVAL_DN].remap_id = SDLK_INSERT;
	hotspots[HS_RUNOPTS0_KRINTERVAL_UP].remap_id = SDLK_DELETE;
	hotspots[HS_RUNOPTS0_FGC_R_DN].remap_id = SDLK_1;
	hotspots[HS_RUNOPTS0_FGC_R_UP].remap_id = SDLK_2;
	hotspots[HS_RUNOPTS0_FGC_G_DN].remap_id = SDLK_3;
	hotspots[HS_RUNOPTS0_FGC_G_UP].remap_id = SDLK_4;
	hotspots[HS_RUNOPTS0_FGC_B_DN].remap_id = SDLK_5;
	hotspots[HS_RUNOPTS0_FGC_B_UP].remap_id = SDLK_6;
	hotspots[HS_RUNOPTS0_BGC_R_DN].remap_id = SDLK_7;
	hotspots[HS_RUNOPTS0_BGC_R_UP].remap_id = SDLK_8;
	hotspots[HS_RUNOPTS0_BGC_G_DN].remap_id = SDLK_9;
	hotspots[HS_RUNOPTS0_BGC_G_UP].remap_id = SDLK_0;
	hotspots[HS_RUNOPTS0_BGC_B_DN].remap_id = SDLK_LEFTBRACKET;
	hotspots[HS_RUNOPTS0_BGC_B_UP].remap_id = SDLK_RIGHTBRACKET;
	hotspots[HS_RUNOPTS0_SAVE].remap_id = SDLK_F2;
	hotspots[HS_RUNOPTS0_EXIT].remap_id = SDLK_ESCAPE;
	hotspots[HS_RUNOPTS0_NEXT].remap_id = SDLK_PAGEDOWN;

	/* Initialise runtime options hotspots */
	hotspots[HS_RUNOPTS1_RUNOPTS1].flags &= ~HS_PROP_NAVIGABLE;
	for (count = HS_RUNOPTS1_RUNOPTS1; count <= HS_RUNOPTS1_EXIT; count++) {
		hotspots[count].gid = HS_GRP_RUNOPT1;
	}
	hotspots[HS_RUNOPTS1_JDEADZ_DN].remap_id = SDLK_HOME;
	hotspots[HS_RUNOPTS1_JDEADZ_UP].remap_id = SDLK_END;
	hotspots[HS_RUNOPTS1_START_CFG].remap_id = SDLK_s;
	hotspots[HS_RUNOPTS1_START_CFG].flags |= HS_PROP_SELECTED;	/* Default selected */
	hotspots[HS_RUNOPTS1_CFG_LTRIG].remap_id = SDLK_l;
	hotspots[HS_RUNOPTS1_CFG_RTRIG].remap_id = SDLK_r;
	hotspots[HS_RUNOPTS1_CFG_LEFT].remap_id = SDLK_0;
	hotspots[HS_RUNOPTS1_CFG_RIGHT].remap_id = SDLK_1;
	hotspots[HS_RUNOPTS1_CFG_UP].remap_id = SDLK_2;
	hotspots[HS_RUNOPTS1_CFG_DOWN].remap_id = SDLK_3;
	hotspots[HS_RUNOPTS1_CFG_SELECT].remap_id = SDLK_4;
	hotspots[HS_RUNOPTS1_CFG_START].remap_id = SDLK_5;
	hotspots[HS_RUNOPTS1_CFG_A].remap_id = SDLK_6;
	hotspots[HS_RUNOPTS1_CFG_B].remap_id = SDLK_7;
	hotspots[HS_RUNOPTS1_CFG_Y].remap_id = SDLK_8;
	hotspots[HS_RUNOPTS1_CFG_X].remap_id = SDLK_9;
	hotspots[HS_RUNOPTS1_BACK].remap_id = SDLK_PAGEUP;
	hotspots[HS_RUNOPTS1_SAVE].remap_id = SDLK_F2;
	hotspots[HS_RUNOPTS1_EXIT].remap_id = SDLK_ESCAPE;

	/* Resize all hotspots for the current screen dimensions */
	hotspots_resize();

	/* Update all hotspots to reflect current component states */
	hotspots_update();
}

/***************************************************************************
 * Hotspots Resize                                                         *
 ***************************************************************************/
/* This repositions and resizes all the hotspots for the current dimensions
 * of the screen. It's done once at the start and again when the screen is
 * resized and it requires that hotspots-init has been called at least once */

void hotspots_resize(void) {
	int count;

	/* Resize root hotspots */
	hotspots[HS_ROOT_CANVAS].hit_x = 0;
	hotspots[HS_ROOT_CANVAS].hit_y = 0;
	hotspots[HS_ROOT_CANVAS].hit_w = video.screen->w;
	hotspots[HS_ROOT_CANVAS].hit_h = video.screen->h;

	/* Resize the ZX81's file selector hotspots */
	if (!zx80) {
		hotspots[HS_LOAD_Q].hit_x = emulator.xoffset + 36 * video.scale;
		hotspots[HS_LOAD_Q].hit_y = emulator.yoffset + 176 * video.scale;
		hotspots[HS_LOAD_Q].hit_w = 40 * video.scale;
		hotspots[HS_LOAD_Q].hit_h = 16 * video.scale;
		hotspots[HS_LOAD_A].hit_x = emulator.xoffset + 76 * video.scale;
		hotspots[HS_LOAD_A].hit_y = emulator.yoffset + 176 * video.scale;
		hotspots[HS_LOAD_A].hit_w = 40 * video.scale;
		hotspots[HS_LOAD_A].hit_h = 16 * video.scale;
		hotspots[HS_LOAD_ENTER].hit_x = emulator.xoffset + 116 * video.scale;
		hotspots[HS_LOAD_ENTER].hit_y = emulator.yoffset + 176 * video.scale;
		hotspots[HS_LOAD_ENTER].hit_w = 88 * video.scale;
		hotspots[HS_LOAD_ENTER].hit_h = 16 * video.scale;
		hotspots[HS_LOAD_SPACE].hit_x = emulator.xoffset + 204 * video.scale;
		hotspots[HS_LOAD_SPACE].hit_y = emulator.yoffset + 176 * video.scale;
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

	/* Resize the virtual keyboard hotspots */
	hotspots[HS_VKEYB_VKEYB].hit_x = vkeyb.xoffset;
	hotspots[HS_VKEYB_VKEYB].hit_y = vkeyb.yoffset;
	hotspots[HS_VKEYB_VKEYB].hit_w = vkeyb.scaled->w;
	hotspots[HS_VKEYB_VKEYB].hit_h = vkeyb.scaled->h;
	if (zx80) {
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

	/* Resize the runtime options hotspots */
	for (count = HS_RUNOPTS0_RUNOPTS0; count <= HS_RUNOPTS0_NEXT; count++) {
		hotspots[count].hit_x = runtime_options0.xoffset;
		hotspots[count].hit_y = runtime_options0.yoffset;
	}
	hotspots[HS_RUNOPTS0_RUNOPTS0].hit_w = 256 * video.scale;
	hotspots[HS_RUNOPTS0_RUNOPTS0].hit_h = 192 * video.scale;
	hotspots[HS_RUNOPTS0_VOLUME_DN].hit_x += 6.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_VOLUME_DN].hit_y += 1.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_VOLUME_UP].hit_x += 12.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_VOLUME_UP].hit_y += 1.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_KRDELAY_DN].hit_x += 7.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_KRDELAY_DN].hit_y += 5.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_KRDELAY_UP].hit_x += 15.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_KRDELAY_UP].hit_y += 5.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_KRINTERVAL_DN].hit_x += 10.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_KRINTERVAL_DN].hit_y += 7.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_KRINTERVAL_UP].hit_x += 18.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_KRINTERVAL_UP].hit_y += 7.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_R_DN].hit_x += 8.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_R_DN].hit_y += 11.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_R_UP].hit_x += 13.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_R_UP].hit_y += 11.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_G_DN].hit_x += 8.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_G_DN].hit_y += 13.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_G_UP].hit_x += 13.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_G_UP].hit_y += 13.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_B_DN].hit_x += 8.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_B_DN].hit_y += 15.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_B_UP].hit_x += 13.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_FGC_B_UP].hit_y += 15.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_R_DN].hit_x += 23.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_R_DN].hit_y += 11.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_R_UP].hit_x += 28.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_R_UP].hit_y += 11.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_G_DN].hit_x += 23.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_G_DN].hit_y += 13.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_G_UP].hit_x += 28.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_G_UP].hit_y += 13.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_B_DN].hit_x += 23.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_B_DN].hit_y += 15.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_B_UP].hit_x += 28.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_BGC_B_UP].hit_y += 15.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_SAVE].hit_x += 10 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_SAVE].hit_y += 22.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_SAVE].hit_w = 4 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_EXIT].hit_x += 18 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_EXIT].hit_y += 22.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_EXIT].hit_w = 4 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_NEXT].hit_x += 25 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_NEXT].hit_y += 22.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS0_NEXT].hit_w = 7 * 8 * video.scale;
	/* Set-up hit_w/h */
	for (count = HS_RUNOPTS0_VOLUME_DN; count <= HS_RUNOPTS0_NEXT; count++) {
		hotspots[count].hit_h = 2 * 8 * video.scale;
		if (count >= HS_RUNOPTS0_VOLUME_DN && count <= HS_RUNOPTS0_BGC_B_UP) 
			hotspots[count].hit_w = 2 * 8 * video.scale;
	}
	/* Set-up hl_x/y/w/h */
	for (count = HS_RUNOPTS0_VOLUME_DN; count <= HS_RUNOPTS0_NEXT; count++) {
		hotspots[count].hl_x = hotspots[count].hit_x;
		hotspots[count].hl_y = hotspots[count].hit_y;
		hotspots[count].hl_w = hotspots[count].hit_w;
		hotspots[count].hl_h = hotspots[count].hit_h;
	}
	for (count = HS_RUNOPTS0_VOLUME_DN; count <= HS_RUNOPTS0_NEXT; count++) {
		hotspots[count].hl_y += 0.5 * 8 * video.scale;
		hotspots[count].hl_h -= 1 * 8 * video.scale;
	}

	/* Resize the runtime options hotspots */
	for (count = HS_RUNOPTS1_RUNOPTS1; count <= HS_RUNOPTS1_EXIT; count++) {
		hotspots[count].hit_x = runtime_options1.xoffset;
		hotspots[count].hit_y = runtime_options1.yoffset;
	}
	hotspots[HS_RUNOPTS1_RUNOPTS1].hit_w = 256 * video.scale;
	hotspots[HS_RUNOPTS1_RUNOPTS1].hit_h = 192 * video.scale;
	hotspots[HS_RUNOPTS1_JDEADZ_DN].hit_x += 18.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_JDEADZ_DN].hit_y += 1.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_JDEADZ_DN].hit_w = 2 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_JDEADZ_UP].hit_x += 24.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_JDEADZ_UP].hit_y += 1.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_JDEADZ_UP].hit_w = 2 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_START_CFG].hit_x += 2 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_START_CFG].hit_y += 5.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_START_CFG].hit_w = 18 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_LTRIG].hit_x += 4 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_LTRIG].hit_y += 7.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_LTRIG].hit_w = 3 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_RTRIG].hit_x += 15 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_RTRIG].hit_y += 7.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_RTRIG].hit_w = 3 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_LEFT].hit_x += 3 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_LEFT].hit_y += 11.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_RIGHT].hit_x += 6 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_RIGHT].hit_y += 11.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_UP].hit_x += 4.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_UP].hit_y += 10 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_DOWN].hit_x += 4.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_DOWN].hit_y += 13 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_SELECT].hit_x += 8.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_SELECT].hit_y += 12.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_START].hit_x += 11.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_START].hit_y += 12.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_A].hit_x += 14 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_A].hit_y += 11.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_B].hit_x += 17 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_B].hit_y += 11.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_Y].hit_x += 15.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_Y].hit_y += 10 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_X].hit_x += 15.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_CFG_X].hit_y += 13 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_BACK].hit_x += 0 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_BACK].hit_y += 22.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_BACK].hit_w = 7 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_SAVE].hit_x += 10 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_SAVE].hit_y += 22.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_SAVE].hit_w = 4 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_EXIT].hit_x += 18 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_EXIT].hit_y += 22.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_EXIT].hit_w = 4 * 8 * video.scale;
	/* Set-up hit_w/h */
	for (count = HS_RUNOPTS1_JDEADZ_DN; count <= HS_RUNOPTS1_EXIT; count++) {
		hotspots[count].hit_h = 2 * 8 * video.scale;
		if (count >= HS_RUNOPTS1_CFG_LEFT && count <= HS_RUNOPTS1_CFG_X) 
			hotspots[count].hit_w = 2 * 8 * video.scale;
	}
	/* Set-up hl_x/y/w/h */
	for (count = HS_RUNOPTS1_JDEADZ_DN; count <= HS_RUNOPTS1_EXIT; count++) {
		hotspots[count].hl_x = hotspots[count].hit_x;
		hotspots[count].hl_y = hotspots[count].hit_y;
		hotspots[count].hl_w = hotspots[count].hit_w;
		hotspots[count].hl_h = hotspots[count].hit_h;
	}
	hotspots[HS_RUNOPTS1_JDEADZ_DN].hl_y += 0.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_JDEADZ_DN].hl_h -= 1 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_JDEADZ_UP].hl_y += 0.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_JDEADZ_UP].hl_h -= 1 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_START_CFG].hl_y += 0.5 * 8 * video.scale;
	hotspots[HS_RUNOPTS1_START_CFG].hl_h -= 1 * 8 * video.scale;
	for (count = HS_RUNOPTS1_SAVE; count <= HS_RUNOPTS1_EXIT; count++) {
		hotspots[count].hl_y += 0.5 * 8 * video.scale;
		hotspots[count].hl_h -= 1 * 8 * video.scale;
	}

}

/***************************************************************************
 * Hotspots Update                                                         *
 ***************************************************************************/
/* This sets hotspots [in]visible depending on the states of the components
 * that affect them */

void hotspots_update(void) {
	int count;
	
	/* Update the ZX81's file selector hotspots */
	if (load_selector_state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_LOAD) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_LOAD) hotspots[count].flags &= ~HS_PROP_VISIBLE;
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
	if (runtime_options0.state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_RUNOPT0) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_RUNOPT0) hotspots[count].flags &= ~HS_PROP_VISIBLE;
	}

	/* Update the runtime options hotspots */
	if (runtime_options1.state) {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_RUNOPT1) hotspots[count].flags |= HS_PROP_VISIBLE;
	} else {
		for (count = 0; count < MAX_HOTSPOTS; count++)
			if (hotspots[count].gid == HS_GRP_RUNOPT1) hotspots[count].flags &= ~HS_PROP_VISIBLE;
	}

}

/***************************************************************************
 * Hotspots Render                                                         *
 ***************************************************************************/
/* This renders visible hotspots over everything else and is called towards
 * the end of sdl-video-refresh. What the hotspot will look like is decided
 * here and depends especially on the gid and occasionally on the flags.
 * It's efficient since it will attempt to reuse a previously created and
 * filled highlight if possible */

void hotspots_render(void) {
	static Uint32 last_colour = UNDEFINED;
	int hl_x, hl_y, hl_w, hl_h;
	int count, surround, alpha;
	Uint32 colour;
	int selected, pressed;
	SDL_Rect dstrect;

	/* Highlight any hotspots that are currently visible that have ids
	 * that are recorded as being pressed, and also selected hotspots
	 * if a joystick is enabled */
	for (count = 0; count < MAX_HOTSPOTS; count++) {
		if (hotspots[count].gid != UNDEFINED &&
			hotspots[count].flags & HS_PROP_VISIBLE &&
			hotspots[count].gid != HS_GRP_ROOT &&
			hotspots[count].remap_id != UNDEFINED) {

			pressed = keyboard_buffer[keysym_to_scancode(FALSE, hotspots[count].remap_id)];
			selected = hotspots[count].flags & HS_PROP_SELECTED;

			/* If the control remapper state is active then blink the selected highlight */
			if (selected && ctrl_remapper.state && ctrl_remapper.interval < 
				ctrl_remapper.master_interval / 2) selected = FALSE;

		#ifndef SDL_DEBUG_HOTSPOTS
			if (pressed || (joystick && selected)) {
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
				if (hotspots[count].gid == HS_GRP_LOAD) {
					if (pressed) {
						colour = colours.hs_load_pressed;
					} else {
						colour = colours.hs_load_selected;
					}
				} else if (hotspots[count].gid == HS_GRP_VKEYB) {
					if (hotspots[count].flags & HS_PROP_TOGGLE &&
						hotspots[count].flags & HS_PROP_STUCK) {
						/* If it's stuck it must be pressed */
						if (zx80) {
							colour = colours.hs_vkeyb_zx80_toggle_pressed;
						} else {
							colour = colours.hs_vkeyb_zx81_toggle_pressed;
						}
					} else {
						if (pressed) {
							if (zx80) {
								colour = colours.hs_vkeyb_zx80_pressed;
							} else {
								colour = colours.hs_vkeyb_zx81_pressed;
							}
						} else {
							if (zx80) {
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
				} else if (hotspots[count].gid == HS_GRP_RUNOPT0 ||
					hotspots[count].gid == HS_GRP_RUNOPT1) {
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
				/* Create an RGB surface to accommodate the highlight.
				 * The chances are that we can reuse the last highlight created */
				if (!highlight ||
					(highlight && (highlight->w != hl_w || highlight->h != hl_h))) {
					/* Free the surface first if it exists */
					if (highlight) SDL_FreeSurface(highlight);
					/* Undefine the last fill colour */
					last_colour = UNDEFINED;
					highlight = SDL_CreateRGBSurface(SDL_SWSURFACE, hl_w, hl_h,
						video.screen->format->BitsPerPixel,
						video.screen->format->Rmask, video.screen->format->Gmask,
						video.screen->format->Bmask, video.screen->format->Amask);
					if (highlight == NULL) {
						fprintf(stderr, "%s: Cannot create RGB surface: %s\n", __func__, 
							SDL_GetError ());
						exit(1);
					}
				}
				/* Fill the highlight if colour is not the last colour used */
				if (colour != last_colour) {
					last_colour = colour;
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
				/* For the load selector, add interior left and right
				 * solid bars using the current background colour */
				if (hotspots[count].gid == HS_GRP_LOAD) {
					if (!invert_screen) {
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



