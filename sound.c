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
 * sound.c - sound support, based on the beeper/AY code I
 *           wrote for Fuse.
 */

/* ZX81 sound routines seem to be based on using VSYNC; in Basic
 * you might do this by alternating FAST/SLOW mode quickly,
 * for example. So that's what's emulated here. Obviously this
 * won't be perfect, not least as it won't be as noisy... :-)
 */
 
/* some AY details (volume levels, white noise RNG algorithm) based on
 * info from MAME's ay8910.c - MAME's licence explicitly permits free
 * use of info (even encourages it).
 */

/* NB: I know some of this stuff looks fairly CPU-hogging.
 * For example, the AY code tracks changes with sub-frame timing
 * in a rather hairy way, and there's subsampling here and there.
 * But if you measure the CPU use, it doesn't actually seem
 * very high at all. And I speak as a Cyrix owner. :-)
 *
 * (I based that on testing in Fuse, but I doubt it's that much
 * worse in z81. Though in both, the AY code does cause cache oddities
 * on my machine, so I get the bizarre situation of z81 jumping
 * between <=2% CPU use and *30*% CPU use pretty much at random...)
 */

#ifdef OSS_SOUND_SUPPORT

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef SZ81	/* Added by Thunor */
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#endif

#include <sys/stat.h>

#include "common.h"
#include "sound.h"
#include "z80/z80.h"

#ifdef SZ81	/* Added by Thunor */
#include "sdl.h"
#endif

#include "sndrender/sndinterface.h"
/* "Unreal" buffer; times two because ticks within a frame may be varying a little */
unsigned char sndplaybuf[640*4];

// #define SIMPLE_RECORD

#ifdef SIMPLE_RECORD
FILE *sndhandle=NULL;
#endif


/* configuration */
int sound_enabled=0;
int sound_freq=32000;
int sound_stereo=1;
int sound_stereo_acb=1;		/* 1 for ACB stereo, else 0 */
int sound_ay_unreal=0;

/* sound_vsync and sound_ay are in common.c */


/* XXX don't know the clock rate for the QS sound board :-(
 * but this sounds pretty plausible, judging from the rather nice
 * effects in QS Defender...
 */
#define AY_CLOCK_QUICKSILVA	(3250000/4)

/* Bi-Pak Zon X-81, clock rate straight from the manual (thanks Hob!).
 * This is half of 3.25MHz, which is probably not a coincidence. ;-)
 */
#define AY_CLOCK_ZONX		1625000


/* assume all three tone channels together match the beeper volume.
 * Must be <=127 for all channels; 4 x 31 = 124.
 */
#ifdef SZ81	/* Added by Thunor */
#define AMPL_BEEPER (sdl_sound.volume / 4)
#define AMPL_AY_TONE (sdl_sound.volume / 4)
#else
#define AMPL_BEEPER		31
#define AMPL_AY_TONE		31	/* three of these */
#endif

/* full range of beeper volume */
#ifdef SZ81	/* Added by Thunor */
#define VOL_BEEPER (sdl_sound.volume / 2)
#else
#define VOL_BEEPER		(AMPL_BEEPER*2)
#endif

static int sound_framesiz;

static unsigned char ay_tone_levels[16];

static unsigned char *sound_buf=NULL;
static unsigned char *sound_ptr=NULL;
static int sound_oldpos,sound_fillpos,sound_oldval,sound_oldval_orig;

/* timer used for fadeout after beeper-toggle;
 * fixed-point with low 24 bits as fractional part.
 */
static unsigned int beeper_tick,beeper_tick_incr;

static int beeper_last_subpos=0;

struct ay_change_tag
  {
  unsigned long tstates;
  unsigned short ofs;
  unsigned char reg,val;
  };

#ifndef SZ81	/* Added by Thunor */
static int soundfd=-1;
#endif

static int sixteenbit=0;

#ifndef SZ81	/* Added by Thunor */
/* using (8) 64 byte frags for 8kHz, scale up for higher */
#define BASE_SOUND_FRAG_PWR	6
#endif

static unsigned int ay_tick_incr;

/* max. number of sub-frame AY port writes allowed;
 * given the number of port writes theoretically possible in a
 * 50th I think this should be plenty.
 */
#define AY_CHANGE_MAX		8000

/* tick/incr/periods are all fixed-point with low 16 bits as
 * fractional part, except ay_env_{tick,period} which count as the chip does.
 */
/* AY registers */
/* we have 16 so we can fake an 8910 if needed */

static struct ay_states {
	unsigned char sound_ay_registers[16];
	unsigned int ay_tone_tick[3],ay_noise_tick;
	unsigned int ay_env_tick,ay_env_subcycles;
	unsigned int ay_tone_period[3],ay_noise_period,ay_env_period;
	int env_held, env_alternating;
	int rng, noise_toggle, env_level;
	struct ay_change_tag ay_change[AY_CHANGE_MAX];
	int ay_change_count;
} ays_states[2];

#ifdef SNDPRC
#define SP_TOTAL 114746
int sp_on;
static unsigned char sp_data[SP_TOTAL];
static unsigned int sp_ptrs[64];
static float sp_durs[64];
static int sp_ptr;
static float sp_dt1, sp_dt2, sp_ts1, sp_ts2, sp_t, sp_dur;
#endif

void ay_states_reset(struct ay_states* as)
{
	int count;

	for (count=0; count<16; count++) as->sound_ay_registers[count] = 0;
	for (count=0; count<3; count++) as->ay_tone_tick[count] = 0;
	as->ay_noise_tick = 0;
	as->ay_env_tick = 0;
	as->ay_env_subcycles = 0;
	for (count=0; count<3; count++)	as->ay_tone_period[count] = 0;
	as->ay_noise_period = 0;
	as->ay_env_period = 0;
	as->env_held = 0;
	as->env_alternating = 0;
	as->rng = 1;
	as->noise_toggle = 1;
	as->env_level = 0;
	for (count=0; count<AY_CHANGE_MAX; count++) {
		as->ay_change[count].tstates=0;
		as->ay_change[count].ofs=0;
		as->ay_change[count].reg=0;
		as->ay_change[count].val=0;
	}
	as->ay_change_count=0;
}

#ifndef SZ81	/* Added by Thunor */
/* returns non-zero on success, and adjusts freq/stereo args to reflect
 * what we've actually got.
 */
int osssound_init(int *freqptr,int *stereoptr)
{
int frag,tmp;

if((soundfd=open("/dev/dsp",O_WRONLY))<0)
  return(0);

tmp=AFMT_U8;

if(ioctl(soundfd,SNDCTL_DSP_SETFMT,&tmp)==-1)
  {
  /* try 16-bit - may be a 16-bit-only device */
  tmp=AFMT_S16_LE;
  if((ioctl(soundfd,SNDCTL_DSP_SETFMT,&tmp))==-1)
    {
    close(soundfd);
    return(0);
    }

  sixteenbit=1;
  }

tmp=(*stereoptr)?1:0;
if(ioctl(soundfd,SNDCTL_DSP_STEREO,&tmp)<0)
  {
  /* if it failed, make sure the opposite is ok */
  tmp=(*stereoptr)?0:1;
  if(ioctl(soundfd,SNDCTL_DSP_STEREO,&tmp)<0)
    {
    close(soundfd);
    return(0);
    }
  *stereoptr=tmp;
  }

frag=(0x80000|BASE_SOUND_FRAG_PWR);

if(ioctl(soundfd,SNDCTL_DSP_SPEED,freqptr)<0)
  {
  close(soundfd);
  return(0);
  }

if(*freqptr>8250) frag++;
if(*freqptr>16500) frag++;
if(*freqptr>33000) frag++;
if(*stereoptr) frag++;
if(sixteenbit) frag++;

if(ioctl(soundfd,SNDCTL_DSP_SETFRAGMENT,&frag)<0)
  {
  close(soundfd);
  return(0);
  }

return(1);	/* success */
}
#endif


#ifndef SZ81	/* Added by Thunor */
void osssound_end(void)
{
if(soundfd!=-1)
  close(soundfd);
}
#endif


void osssound_frame(unsigned char *data,int len)
{
static unsigned char buf16[8192];
#ifndef SZ81	/* Added by Thunor */
int ret=0,ofs=0;
#endif

if(sixteenbit)
  {
  unsigned char *src,*dst;
  int f;

  src=data; dst=buf16;
  for(f=0;f<len;f++)
    {
    *dst++=128;
    *dst++=*src++-128;
    }

  data=buf16;
  len<<=1;
  }

#ifdef SZ81	/* Added by Thunor */
 sdl_sound_frame((Uint16 *)data, len/2);
#else
while(len)
  {
  ret=write(soundfd,data+ofs,len);
  if(ret>0)
    ofs+=ret,len-=ret;
  }
#endif
}


#ifdef SZ81	/* Added by Thunor */
void sound_ay_setvol(void)
{
int f;
double v;

/* logarithmic volume levels, 3dB per step */
v=AMPL_AY_TONE;
for(f=15;f>0;f--)
  {
  ay_tone_levels[f]=(int)(v+0.5);
  /* 10^3/20 = 3dB */
  v/=1.4125375446;
  }
ay_tone_levels[0]=0;
}
#endif


void sound_ay_init(void)
{
int f,clock;
double v;

/* logarithmic volume levels, 3dB per step */
v=AMPL_AY_TONE;
for(f=15;f>0;f--)
  {
  ay_tone_levels[f]=(int)(v+0.5);
  /* 10^3/20 = 3dB */
  v/=1.4125375446;
  }
ay_tone_levels[0]=0;

ay_states_reset(&ays_states[0]);
ay_states_reset(&ays_states[1]);

switch(sound_ay_type)
  {
  case AY_TYPE_QUICKSILVA:
    clock=AY_CLOCK_QUICKSILVA;
    break;
  case AY_TYPE_ZONX:
    clock=AY_CLOCK_ZONX;
    break;
  default:
    fprintf(stderr,"AY type not specified - can't happen!\n");
    sound_ay=0;
    return;
  }

ay_tick_incr=(int)(65536.*clock/sound_freq);

}


#ifdef SZ81	/* Added by Thunor */
int sound_framesiz_init(void)
{
if(sound_buf) {
  free(sound_buf);
  sound_buf=NULL;
}

sound_framesiz=sound_freq/(1000/sdl_emulator.speed);

if((sound_buf=malloc(sound_framesiz*(sound_stereo+1)))==NULL)
  return 1;

sound_ptr=sound_buf;	/* sound_ptr isn't used anyway */

return 0;
}
#endif

#ifdef SNDPRC

int sp_hash(char *s)
{
	int k=0;

	k = *s-96;
	k += *(s+1)-96;

	return k;
}

void sp_load(char *name, unsigned int *p, unsigned int *i, float d)
{
	char filnam[256];
	FILE *f;
	struct stat buf;
	unsigned int n;

	fprintf(stderr, "%s",name);

	strcpy(filnam, PACKAGE_DATA_DIR);
	strcat(filnam, "/allophones/");
	strcat(filnam, name);
	strcat(filnam, ".wav");

	f = fopen(filnam, "rb");
	if (!f) {
	  fprintf(stderr, "sp_load: error opening \"%s\"\n", filnam);
	  return;
	}
	fstat(fileno(f), &buf);
	n = buf.st_size-44;
	fread(filnam, 44, 1, f);
	fread(sp_data+(*p), n, 1, f);
	fclose(f);

	sp_ptrs[*i] = *p;
	sp_durs[*i] = d;
	*p += n;
	++(*i);
}

void sp_init()
{
	unsigned int p=0, i=0;
	
	sp_load("pa1", &p, &i, 0.0064); // 00
	sp_load("pa2", &p, &i, 0.0256);
	sp_load("pa3", &p, &i, 0.0448);
	sp_load("pa4", &p, &i, 0.0960);
	sp_load("pa5", &p, &i, 0.1984);
	sp_load("oy", &p, &i,  0.2912);
	sp_load("ay", &p, &i,  0.1729);
	sp_load("eh", &p, &i,  0.0546);

	sp_load("kk3", &p, &i, 0.0768); // 08
	sp_load("pp", &p, &i,  0.1472);
	sp_load("jh", &p, &i,  0.0984);
	sp_load("nn1", &p, &i, 0.1729);
	sp_load("ih", &p, &i,  0.0455);
	sp_load("tt2", &p, &i, 0.0960);
	sp_load("rr1", &p, &i, 0.1274);
	sp_load("ax", &p, &i,  0.0546);

	sp_load("mm", &p, &i,  0.1820); // 10
	sp_load("tt1", &p, &i, 0.0768);
	sp_load("dh1", &p, &i, 0.1365);
	sp_load("iy", &p, &i,  0.1729);
	sp_load("ey", &p, &i,  0.2002);
	sp_load("dd1", &p, &i, 0.0455);
	sp_load("uw1", &p, &i, 0.0637);
	sp_load("ao", &p, &i,  0.0728);

	sp_load("aa", &p, &i,  0.0637); // 18
	sp_load("yy2", &p, &i, 0.1274);
	sp_load("ae", &p, &i,  0.0819);
	sp_load("hh1", &p, &i, 0.0896);
	sp_load("bb1", &p, &i, 0.0364);
	sp_load("th", &p, &i,  0.1280);
	sp_load("uh", &p, &i,  0.0728);
	sp_load("uw2", &p, &i, 0.1729);

	sp_load("aw", &p, &i,  0.2548); // 20
	sp_load("dd2", &p, &i, 0.0721);
	sp_load("gg3", &p, &i, 0.1105);
	sp_load("vv", &p, &i,  0.1274);
	sp_load("gg1", &p, &i, 0.0721);
	sp_load("sh", &p, &i,  0.1984);
	sp_load("zh", &p, &i,  0.1341);
	sp_load("rr2", &p, &i, 0.0819);

	sp_load("ff", &p, &i,  0.1088); // 28
	sp_load("kk2", &p, &i, 0.1344);
	sp_load("kk1", &p, &i, 0.1152);
	sp_load("zz", &p, &i,  0.1486);
	sp_load("ng", &p, &i,  0.2002);
	sp_load("ll", &p, &i,  0.0819);
	sp_load("ww", &p, &i,  0.1456);
	sp_load("xr", &p, &i,  0.2457);

	sp_load("wh", &p, &i,  0.1452); // 30
	sp_load("yy1", &p, &i, 0.0910);
	sp_load("ch", &p, &i,  0.1472);
	sp_load("er1", &p, &i, 0.1092);
	sp_load("er2", &p, &i, 0.2093);
	sp_load("ow", &p, &i,  0.1729);
	sp_load("dh2", &p, &i, 0.1820);
	sp_load("ss", &p, &i,  0.0640);

	sp_load("nn2", &p, &i, 0.1365); // 38
	sp_load("hh2", &p, &i, 0.1260);
	sp_load("or", &p, &i,  0.2360);
	sp_load("ar", &p, &i,  0.2002);
	sp_load("yr", &p, &i,  0.2457);
	sp_load("gg2", &p, &i, 0.0694);
	sp_load("el", &p, &i,  0.1365);
	sp_load("bb2", &p, &i, 0.0502);

	sp_on = 0;

	sp_dt1 = 1./11025;
	sp_dt2 = 1./sound_freq;

}

void sp_start(int idx)
{

	sp_ptr = sp_ptrs[idx];
	sp_dur = sp_durs[idx];

	sp_ts1 = 0.;
	sp_ts2 = sp_dt1;
	sp_t = 0.;

	sp_on = 1;
}

unsigned char sp_sample()
{
	unsigned char s1, s2, s;

	if (sp_t > sp_ts2) {
	  sp_ptr++;
	  sp_ts1 += sp_dt1;
	  sp_ts2 += sp_dt1;
	}

	s1 = sp_data[sp_ptr];
	s2 = sp_data[sp_ptr+1];

	s = (unsigned char) ((float)s1 + (s2-s1)*(sp_t-sp_ts1)/sp_dt1);

	sp_t += sp_dt2;

	if (sp_t >= sp_dur) sp_on = 0;

	return s;
}

#endif

void sound_init(void)
{
#ifdef SZ81	/* Added by Thunor */
if (sdl_sound_init(sound_freq, &sound_stereo, &sixteenbit)) return;
#else
if(!osssound_init(&sound_freq,&sound_stereo))
  {
  /* XXX a bit vague, maybe I should put this check in osssound_init() */
  fprintf(stderr,"z81: warning: "
          "couldn't initialise sound device, sound disabled.\n");
  return;
  }
#endif

/* important to override this if not using stereo */
if(!sound_stereo)
  sound_stereo_acb=0;

sound_enabled=1;

#ifdef SZ81	/* Added by Thunor */
if(sound_framesiz_init())
  {
  sound_end();
  return;
  }
#else
sound_framesiz=sound_freq/50;

if((sound_buf=malloc(sound_framesiz*(sound_stereo+1)))==NULL)
  {
  sound_end();
  return;
  }
#endif

sound_oldval=sound_oldval_orig=128;
sound_oldpos=-1;
sound_fillpos=0;
sound_ptr=sound_buf;

beeper_tick=0;
beeper_tick_incr=(1<<24)/sound_freq;

#ifdef SIMPLE_RECORD
sndhandle = fopen("sound.raw", "wb");
#endif

#ifdef SNDPRC
 sp_init();
#endif

 if(sound_ay) {
   sound_ay_init();
   snd_init();
   snd_start_frame();
 }

}


void sound_end(void)
{
if(sound_enabled)
  {
  sound_enabled=0;
  if(sound_buf) {
    free(sound_buf);
    sound_buf=NULL;
  }
#ifdef SZ81	/* Added by Thunor */
  sdl_sound_end();
#else
  osssound_end();
#endif
  }
#ifdef SIMPLE_RECORD
 if (sndhandle) {
   fclose(sndhandle);
   sndhandle=NULL;
 }
#endif
}



/* not great having this as a macro to inline it, but it's only
 * a fairly short routine, and it saves messing about.
 * (XXX ummm, possibly not so true any more :-))
 */
#define AY_GET_SUBVAL(tick,period) \
  (level*2*(tick-period)/ay_tick_incr)

#define AY_OVERLAY_TONE(ptr,chan,level) \
  was_high=0;								\
  if(level)								\
    {									\
    if(dev->ay_tone_tick[chan]>=dev->ay_tone_period[chan])		\
      (*(ptr))+=(level),was_high=1;					\
    else								\
      (*(ptr))-=(level);						\
    }									\
  									\
  dev->ay_tone_tick[chan]+=ay_tick_incr;				\
  if(level && !was_high && dev->ay_tone_tick[chan]>=dev->ay_tone_period[chan])	\
    (*(ptr))+=AY_GET_SUBVAL(dev->ay_tone_tick[chan],dev->ay_tone_period[chan]);	\
  									\
  if(dev->ay_tone_tick[chan]>=dev->ay_tone_period[chan]*2)		\
    {									\
    dev->ay_tone_tick[chan]-=dev->ay_tone_period[chan]*2;		\
    /* sanity check needed to avoid making samples sound terrible */ 	\
    if(level && dev->ay_tone_tick[chan]<dev->ay_tone_period[chan]) 	\
      (*(ptr))-=AY_GET_SUBVAL(dev->ay_tone_tick[chan],0);		\
    }


static void sound_ay_overlay(void)
{
struct ay_states *dev;
int tone_level[3];
int mixer,envshape;
int f,g,level;
int v=0;
unsigned char tmp;
unsigned char *ptr;
struct ay_change_tag *change_ptr;
int changes_left;
int reg,r;
int was_high;
int channels=(sound_stereo?2:1);
int count;

/* If no AY chip(s), don't produce any AY sound (!) */
if (!sound_ay) return;

for (count=0; count<2; count++) {

dev = &ays_states[count];
change_ptr = dev->ay_change;
changes_left = dev->ay_change_count;

/* convert change times to sample offsets */
for(f=0;f<changes_left;f++)
	change_ptr[f].ofs=(change_ptr[f].tstates*sound_freq)/3250000;

for(f=0,ptr=sound_buf;f<sound_framesiz;f++,ptr+=channels)
  {
  /* update ay registers. All this sub-frame change stuff
   * is pretty hairy, but how else would you handle the
   * samples in Robocop? :-) It also clears up some other
   * glitches.
   *
   * Ok, maybe that's no big deal on the ZX81, but even so. :-)
   * (Though, due to tstate-changing games in z80.c, we can
   * rarely `lose' one this way - hence "f!=.." bit below
   * to catch any that slip through.)
   */

    while (changes_left) {

    if (f<change_ptr->ofs && f!=sound_framesiz-1) break;

    dev->sound_ay_registers[reg=change_ptr->reg] = change_ptr->val;
    change_ptr++; changes_left--;

    /* fix things as needed for some register changes */
    switch(reg)
      {
      case 0: case 1: case 2: case 3: case 4: case 5:
        r=reg>>1;
        dev->ay_tone_period[r] = (8*(dev->sound_ay_registers[reg&~1]|
                                    (dev->sound_ay_registers[reg|1]&15)<<8))<<16;

        /* important to get this right, otherwise e.g. Ghouls 'n' Ghosts
         * has really scratchy, horrible-sounding vibrato.
         */
        if (dev->ay_tone_period[r] && dev->ay_tone_tick[r]>=dev->ay_tone_period[r]*2)
		dev->ay_tone_tick[r]%=dev->ay_tone_period[r]*2;
        break;
      case 6:
        dev->ay_noise_tick = 0;
        dev->ay_noise_period = (16*(dev->sound_ay_registers[reg]&31))<<16;
        break;
      case 11: case 12:
        /* this one *isn't* fixed-point */
        dev->ay_env_period = dev->sound_ay_registers[11]|(dev->sound_ay_registers[12]<<8);
        break;
      case 13:
        dev->ay_env_tick = dev->ay_env_subcycles = 0;
        dev->env_held = dev->env_alternating = 0;
        dev->env_level = 0;
        break;
      }
    }

  /* the tone level if no enveloping is being used */
  for(g=0;g<3;g++)
    tone_level[g] = ay_tone_levels[dev->sound_ay_registers[8+g]&15];

  /* envelope */
  envshape = dev->sound_ay_registers[13];
  if(dev->ay_env_period)
    {
    if(!dev->env_held)
      {
      v=((int)dev->ay_env_tick*15)/dev->ay_env_period;
      if (v<0) v=0;
      if (v>15) v=15;
      if ((envshape&4)==0) v=15-v;
      if (dev->env_alternating) v=15-v;
      dev->env_level=ay_tone_levels[v];
      }
    }
  
  for(g=0;g<3;g++)
    if(dev->sound_ay_registers[8+g]&16)
      tone_level[g]=dev->env_level;

  if(dev->ay_env_period)
    {
    /* envelope gets incr'd every 256 AY cycles */
    dev->ay_env_subcycles+=ay_tick_incr;
    if(dev->ay_env_subcycles>=(256<<16))
      {
      dev->ay_env_subcycles-=(256<<16);
      
      dev->ay_env_tick++;
      if(dev->ay_env_tick>=dev->ay_env_period)
        {
        dev->ay_env_tick -= dev->ay_env_period;
        if(!dev->env_held && ((envshape&1) || (envshape&8)==0))
          {
          dev->env_held=1;
          if((envshape&2) || (envshape&0xc)==4)
            dev->env_level=ay_tone_levels[15-v];
          }
        if(!dev->env_held && (envshape&2))
          dev->env_alternating=!dev->env_alternating;
        }
      }
    }

  /* generate tone+noise */
  /* channel C first (used to make ACB easier) */
  mixer=dev->sound_ay_registers[7];
  if((mixer&4)==0 || (mixer&0x20)==0)
    {
    level=(dev->noise_toggle || (mixer&0x20))?tone_level[2]:0;
    tmp = *ptr;
    AY_OVERLAY_TONE(ptr,2,level);
    if(sound_stereo && sound_stereo_acb)
      ptr[1] += *ptr-tmp;
    }
  if((mixer&1)==0 || (mixer&0x08)==0)
    {
    level=(dev->noise_toggle || (mixer&0x08))?tone_level[0]:0;
    AY_OVERLAY_TONE(ptr,0,level);
    }
  if((mixer&2)==0 || (mixer&0x10)==0)
    {
    level=(dev->noise_toggle || (mixer&0x10))?tone_level[1]:0;
    AY_OVERLAY_TONE(ptr+sound_stereo_acb,1,level);
    }
  
  if(sound_stereo && !sound_stereo_acb)
    ptr[1]=*ptr;

  /* update noise RNG/filter */
  dev->ay_noise_tick+=ay_tick_incr;
  if (dev->ay_noise_tick >= dev->ay_noise_period)
    {
    if((dev->rng&1)^((dev->rng&2)?1:0))
      dev->noise_toggle=!dev->noise_toggle;
    
    /* rng is 17-bit shift reg, bit 0 is output.
     * input is bit 0 xor bit 2.
     */
    dev->rng|=((dev->rng&1)^((dev->rng&4)?1:0))?0x20000:0;
    dev->rng>>=1;
    
    dev->ay_noise_tick -= dev->ay_noise_period;
    }
  }
}
}


/* don't make the change immediately; record it for later,
 * to be made by sound_frame() (via sound_ay_overlay()).
 */
void sound_ay_write(int reg,int val,int dev)
{
int count;

if(!sound_enabled || !sound_ay) return;

if (sound_ay_unreal) {
  snd_select(dev, reg);
  snd_write(dev, tstates, val);
}

/* accept r15, in case of the two-I/O-port 8910 */
if(reg>=16) return;

count = ays_states[dev].ay_change_count;

if(tstates>=0 && count<AY_CHANGE_MAX)
  {
  ays_states[dev].ay_change[count].tstates=tstates;
  ays_states[dev].ay_change[count].reg=reg;
  ays_states[dev].ay_change[count].val=val;
  ays_states[dev].ay_change_count++;
  }
}

#ifdef FNMI

/* 50 notes music interface" by Gaetano Marano, implementation by Stefano Bodrato */

void sound_fnmi(int Data)
{
        float freq;
        unsigned int val;

	/* keep only audio channel 2/c */
	sound_ay_write(7,251,0);
	/* set volume to zero if "magic key" found to disable audio */
	if ((Data & 15) == 15) {sound_ay_write(10,0,0);}
	else {
	  /* enable the 555 oscillator */
	  freq=(60.0 + 5.4*(float)(~Data>>3&1) + 9.4*(float)(~Data>>2&1) + 13.2*(float)(~Data>>1&1) + 22.1*(float)(~Data&1) + 17.5*(float)(Data>>4&1) + 42.1*(float)(Data>>5&1)) * (float)(1<<(Data>>6));
	  val=(1625000/(16*freq));
	  /* raise volume */
	  sound_ay_write(10,7,0);
	  sound_ay_write(4,(val)&255,0);
	  sound_ay_write(5,(val>>8)&15,0);
	  sound_ay_write(6,0,0);
	}
}

#endif

/* no need to call this initially, but should be called
 * on reset otherwise.
 */
void sound_ay_reset(void)
{
int f;

 for(f=0;f<16;f++) {
	sound_ay_write(f,0,0);
	sound_ay_write(f,0,1);
 }
}


/* XXX currently using speccy beeper code verbatim for VSYNC.
 * Not sure how plausible this is, but for now it'll do.
 * It does *sound* pretty plausible.
 */

/* it should go without saying that the beeper was hardly capable of
 * generating perfect square waves. :-) What seems to have happened is
 * that after the `click' in the intended direction away from the rest
 * (zero) position, it faded out gradually over about 1/50th of a second
 * back down to zero - the bulk of the fade being over after about
 * a 1/500th.
 *
 * The true behaviour is awkward to model accurately, but a compromise
 * emulation (doing a linear fadeout over 1/150th) seems to work quite
 * well and IMHO sounds a little more like a real speccy than most
 * emulations. It also has the considerable advantage of having a zero
 * rest position, which I'm a lot happier with. :-)
 */

#define BEEPER_FADEOUT	(((1<<24)/150)/AMPL_BEEPER)

#define BEEPER_OLDVAL_ADJUST \
  beeper_tick+=beeper_tick_incr;	\
  if(beeper_tick>=BEEPER_FADEOUT)	\
    {					\
    beeper_tick-=BEEPER_FADEOUT;	\
    if(sound_oldval>128)		\
      sound_oldval--;			\
    else				\
      if(sound_oldval<128)		\
        sound_oldval++;			\
    }


void sound_frame(void)
{
unsigned char *ptr;
int f;
int r;
#ifdef SNDPRC
int i, j;
unsigned char s;
#endif

if(!sound_enabled) return;

if (sound_ay && sound_ay_unreal) {
     snd_end_frame(tstates);
     snd_count();
     r = snd_fill_buf(sndplaybuf, sdl_sound.volume);
#ifdef SIMPLE_RECORD
     fwrite(sndplaybuf,r*2,1,sndhandle);
#endif

#ifdef SNDPRC
     if (sp_on) { 
       j = 0;
       for (i=0; i<r; i++) {
	  s = sp_sample();
	  sndplaybuf[j] = s;
	  ++j;
	  sndplaybuf[j] = s;
	  ++j;
       }
     }
#endif

     sdl_sound_frame((Uint16 *)sndplaybuf,r);
     snd_start_frame();
     return;
}

if(sound_vsync)
  {
  ptr=sound_buf+(sound_stereo?sound_fillpos*2:sound_fillpos);
  for(f=sound_fillpos;f<sound_framesiz;f++)
    {
    BEEPER_OLDVAL_ADJUST;
    *ptr++=sound_oldval;
    if(sound_stereo)
      *ptr++=sound_oldval;
    }
  } else
 /* must be AY then, so `zero' buffer ready for it */
 memset(sound_buf,128,sound_framesiz*(sound_stereo+1));

 if (sound_ay) sound_ay_overlay();

 osssound_frame(sound_buf,sound_framesiz*(sound_stereo+1));

 sound_oldpos=-1;
 sound_fillpos=0;
 sound_ptr=sound_buf;

 ays_states[0].ay_change_count=0;
 ays_states[1].ay_change_count=0;
}


void sound_beeper(int on)
{
unsigned char *ptr;
int newpos,subpos;
int val,subval;
int f;
extern unsigned long tsmax;

if(!sound_enabled || !sound_vsync) return;

val=(on?128+AMPL_BEEPER:128-AMPL_BEEPER);

if(val==sound_oldval_orig) return;

/* XXX a lookup table might help here... */
newpos=(tstates*sound_framesiz)/tsmax;
subpos=(tstates*sound_framesiz*VOL_BEEPER)/tsmax-VOL_BEEPER*newpos;

/* if we already wrote here, adjust the level.
 */
if(newpos==sound_oldpos)
  {
  /* adjust it as if the rest of the sample period were all in
   * the new state. (Often it will be, but if not, we'll fix
   * it later by doing this again.)
   */
  if(on)
    beeper_last_subpos+=VOL_BEEPER-subpos;
  else
    beeper_last_subpos-=VOL_BEEPER-subpos;
  }
else
  beeper_last_subpos=(on?VOL_BEEPER-subpos:subpos);

subval=128-AMPL_BEEPER+beeper_last_subpos;

if(newpos>=0)
  {
  /* fill gap from previous position */
  ptr=sound_buf+(sound_stereo?sound_fillpos*2:sound_fillpos);
  for(f=sound_fillpos;f<newpos && f<sound_framesiz;f++)
    {
    BEEPER_OLDVAL_ADJUST;
    *ptr++=sound_oldval;
    if(sound_stereo)
      *ptr++=sound_oldval;
    }

  if(newpos<sound_framesiz)
    {
    /* newpos may be less than sound_fillpos, so... */
    ptr=sound_buf+(sound_stereo?newpos*2:newpos);
    
    /* limit subval in case of faded beeper level,
     * to avoid slight spikes on ordinary tones.
     */
    if((sound_oldval<128 && subval<sound_oldval) ||
       (sound_oldval>=128 && subval>sound_oldval))
      subval=sound_oldval;

    /* write subsample value */
    *ptr=subval;
    if(sound_stereo)
      *++ptr=subval;
    }
  }

sound_oldpos=newpos;
sound_fillpos=newpos+1;
sound_oldval=sound_oldval_orig=val;
}


#ifdef SZ81	/* Added by Thunor */
void sound_reset(void)
{
int count;

/* Reinitialise all variables at the top of sound.c */
sound_enabled=0;
sound_freq=32000;
sound_framesiz=0;
for(count=0;count<16;count++)
  ay_tone_levels[count]=0;
sound_buf=NULL;
sound_ptr=NULL;
sound_oldpos=0;
sound_fillpos=0;
sound_oldval=0;
sound_oldval_orig=0;

ay_states_reset(&ays_states[0]);
ay_states_reset(&ays_states[1]);

beeper_tick=0;
beeper_tick_incr=0;
beeper_last_subpos=0;

sixteenbit=0;
}
#endif

#endif	/* OSS_SOUND_SUPPORT */
