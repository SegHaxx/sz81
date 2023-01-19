/* z81/xz81, Linux console and X ZX81/ZX80 emulators.
 * Copyright (C) 1994 Ian Collier. z81 changes (C) 1995-2001 Russell Marks.
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
 *
 *
 * sound.h
 */

#ifdef OSS_SOUND_SUPPORT

extern int sound_enabled;
extern int sound_freq;
extern int sound_stereo;
extern int sound_stereo_acb;
extern int sound_ay_unreal;

#ifdef SZ81	/* Added by Thunor */
extern void sound_ay_setvol(void);
extern int sound_framesiz_init(void);
extern void sound_reset(void);
#endif
extern void sound_init(void);
extern void sound_end(void);
extern void sound_frame(void);
extern void sound_ay_write(int reg,int val, int dev);
extern void sound_fnmi(int Data);
extern void sound_ay_reset(void);
extern void sound_beeper(int on);

#endif
