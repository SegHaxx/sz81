#ifndef __SNDBUFFER_H_INCLUDED
#define __SNDBUFFER_H_INCLUDED

#ifdef USE_SND_EXTERNAL_BUFFER // enable external buffer

#include <inttypes.h>

#define SNDBUFSIZE 16384
extern uint32_t sndbuf[SNDBUFSIZE];

union SNDSAMPLE;
#define SND_EXTERNAL_BUFFER ((SNDSAMPLE*)sndbuf)
#define SND_EXTERNAL_BUFFER_SIZE SNDBUFSIZE

#else

#undef SND_EXTERNAL_BUFFER

#endif // USE_SND_EXTERNAL_BUFFER

#endif // __SNDBUFFER_H_INCLUDED
