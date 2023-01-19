#include "sndchip.h"
#include "sndcounter.h"

YM2203 *YM2203Init(void *p1, int p2, int p3, int p4) { return NULL; }
void YM2203ResetChip(void* p1) {}
void YM2203Write(YM2203 *p1, int p2, unsigned char p3) {}
void YM2203UpdateOne(YM2203 *p1, FMSAMPLE *p2, int p3) {}
void OPNPrescaler_w(struct dummy_opn *p1, int p2, int p3) {}

struct sound_configuration conf;

SNDCHIP ay1, ay2;
SNDCOUNTER cnt;

static unsigned n_samples;
static bufptr_t start_offset;

extern "C" void snd_init()
{
	conf.soundfilter = 0;
	conf.sound.ay_chip = SNDCHIP::CHIP_AY;
	conf.sound.fq = SNDR_DEFAULT_SAMPLE_RATE;
}

extern "C" void snd_start_frame()
{
	ay1.start_frame();
	ay2.start_frame();
}

extern "C" void snd_end_frame(unsigned clk_ticks)
{
	ay1.end_frame(clk_ticks);
	ay2.end_frame(clk_ticks);
}

extern "C" void snd_select(int chip, unsigned char nreg)
{
	if (chip==0) ay1.select(nreg);
	else ay2.select(nreg);
}

extern "C" void snd_write(int chip, unsigned timestamp, unsigned char val)
{
	if (chip==0) ay1.write(timestamp, val);
	else ay2.write(timestamp, val);
}

extern "C" void snd_count()
{
	cnt.begin();
	cnt.count(ay1);
	cnt.count(ay2);
	cnt.end(start_offset, n_samples);
}

extern "C" unsigned snd_fill_buf(unsigned char *sndplaybuf, int volume)
{
	unsigned k, k2;
	float vl, vr;
	static float ml=0., mr=0.;
	static float alpha = 1./(1.+2*3.14/44100.);

	for (k=0, k2=0; k<n_samples; k++, start_offset++) {

	  vl = sndbuf[start_offset & (SNDBUFSIZE-1)] & 0x0000ffff;
	  vl *= volume / 64.;
	  ml = alpha*ml + (1-alpha)*vl;
	  sndplaybuf[k2++] = (unsigned char) (128. + (vl-ml)/256.);

	  vr = (sndbuf[start_offset & (SNDBUFSIZE-1)] & 0xffff0000) >> 16;
	  vr *= volume / 64.;
	  mr = alpha*mr + (1-alpha)*vr;
	  sndplaybuf[k2++] = (unsigned char) (128. + (vr-mr)/256.);

	  sndbuf[start_offset & (SNDBUFSIZE-1)] = 0;
	}

	return n_samples;
}
