/*
   sound resampling core for Unreal Speccy project
   created under public domain license by SMT, jan.2006
*/

#ifndef _SNDRENDER_H_INCLUDED
#define _SNDRENDER_H_INCLUDED

#include "sndbuffer.h"
#include "../sysdefs.h"

#ifdef SND_EXTERNAL_BUFFER
#if ((SND_EXTERNAL_BUFFER_SIZE & (SND_EXTERNAL_BUFFER_SIZE-1)) != 0)
#pragma error("SND_EXTERNAL_BUFFER_SIZE must be power of 2")
#endif
#endif

union SNDSAMPLE;
struct SNDOUT;

//const unsigned SNDR_DEFAULT_SYSTICK_RATE = 3500000; // ZX-Spectrum Z80 clock
const unsigned SNDR_DEFAULT_SYSTICK_RATE = 3250000; // ZX-81 Z80 clock
const unsigned SNDR_DEFAULT_SAMPLE_RATE = 32000;
const unsigned MULT_C = 12;   // fixed point precision for 'system tick -> sound tick'

#ifdef SND_EXTERNAL_BUFFER
 typedef unsigned bufptr_t;
#else
 typedef SNDSAMPLE *bufptr_t;
#endif

class SNDRENDER
{
   friend class SNDCOUNTER;

 public:

   void set_timings(unsigned clock_rate, unsigned sample_rate);

   // 'render' is a function that converts array of DAC inputs into PCM-buffer
   unsigned render(SNDOUT *src, unsigned srclen, unsigned clk_ticks, bufptr_t dst);

   // set of functions that fills buffer in emulation progress
   void start_frame(bufptr_t dst);

   __inline void update(unsigned timestamp, unsigned l, unsigned r)
{
   if (!((l ^ mix_l) | (r ^ mix_r))) return;

   unsigned endtick = (timestamp * mult_const) >> MULT_C;
//   uint64_t endtick = (timestamp * (uint64_t)sample_rate * TICK_F) / clock_rate;
   flush( (unsigned) (base_tick + endtick) );
   mix_l = l, mix_r = r;
}

   unsigned end_frame(unsigned clk_ticks);
   unsigned end_empty_frame(unsigned clk_ticks);

   // new start position as previous end position
   // (continue to fill buffer)
   void start_frame() { start_frame(dstpos); }

   SNDRENDER();

 protected:

   unsigned mix_l, mix_r;
   bufptr_t dstpos, dst_start;
   unsigned clock_rate, sample_rate; //Alone Coder

 private:

   unsigned tick, base_tick;
   unsigned s1_l, s1_r;
   unsigned s2_l, s2_r;
   unsigned firstsmp; //Alone Coder
   int oldleft,useleft,olduseleft,oldfrmleft; //Alone Coder
   int oldright,useright,olduseright,oldfrmright; //Alone Coder

//   unsigned clock_rate, sample_rate;
   uint64_t passed_clk_ticks, passed_snd_ticks;
   unsigned mult_const;

   void flush(unsigned endtick);
};

union SNDSAMPLE
{
   uint32_t sample; // left/right channels in low/high WORDs
   struct { uint16_t left, right; } ch; // or left/right separately
};

struct SNDOUT
{
   unsigned timestamp; // in 'system clock' ticks
   SNDSAMPLE newvalue;
};

#endif // _SNDRENDER_H_INCLUDED
