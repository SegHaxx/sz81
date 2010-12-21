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

/* Message box manager function IDs */
#define MSG_BOX_SHOW 1
#define MSG_BOX_KILL 2

/* Message box timeouts in ms */
#define MSG_BOX_TIMEOUT_RUNOPTS_SAVE 1500
#define MSG_BOX_TIMEOUT_SOUND_VOLUME 1500
#define MSG_BOX_TIMEOUT_CONTROL_REMAPPER 750

/* Variables */
struct MSG_Box {
	char title[33];		/* The title bar text */
	char text[33];		/* A single line message */
	int timeout;		/* In ms */
};

/* Function prototypes */
void message_box_manager(int funcid, struct MSG_Box *msg_box);


