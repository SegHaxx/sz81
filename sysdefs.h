#ifndef _SYSDEFS_H_
#define _SYSDEFS_H_

#include <stddef.h>
#include <inttypes.h>

typedef int16_t FMSAMPLE;

const int fmsoundon0 = 0;

struct dummy_st {
  int SSGclock, clock, rate;
};

struct dummy_opn {
  struct dummy_st ST;
};

class YM2203 {
public:
  struct dummy_opn OPN;
};

struct sound_chip_configuration {
  int ay_chip, ay, fq, ayfq;
};

extern struct sound_configuration {
  int soundfilter;
  struct sound_chip_configuration sound;
} conf;

YM2203 *YM2203Init(void *p1, int p2, int p3, int p4);
void YM2203ResetChip(void* p1);
void YM2203Write(YM2203 *p1, int p2, unsigned char p3);
void YM2203UpdateOne(YM2203 *p1, FMSAMPLE *p2, int p3);
void OPNPrescaler_w(struct dummy_opn *p1, int p2, int p3);

#endif
