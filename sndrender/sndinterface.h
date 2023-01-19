#ifndef __SNDINTERFACE_H_INCLUDED
#define __SNDINTERFACE_H_INCLUDED

void snd_init();
void snd_start_frame();
void snd_end_frame(unsigned clk_ticks);
void snd_select(int chip, unsigned char nreg);
void snd_write(int chip, unsigned timestamp, unsigned char val);
void snd_count();
unsigned snd_fill_buf(unsigned char *sndplaybuf, int volume);

#endif
