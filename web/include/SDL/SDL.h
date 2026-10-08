/* Minimal SDL 1.2 stand-in for the browser build.
 * The desktop headers pull in the C library; zx81.c relies on that. */
#ifndef WEB_SDL_SDL_H
#define WEB_SDL_SDL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;
typedef int SDLKey;
typedef unsigned int SDL_TimerID;

void SDL_Delay(Uint32 ms);

#define SDLK_BACKSPACE 8
#define SDLK_RETURN    13
#define SDLK_ESCAPE    27
#define SDLK_SPACE     32
#define SDLK_COMMA     44
#define SDLK_PERIOD    46
#define SDLK_0         48
#define SDLK_1         49
#define SDLK_2         50
#define SDLK_3         51
#define SDLK_4         52
#define SDLK_5         53
#define SDLK_6         54
#define SDLK_7         55
#define SDLK_8         56
#define SDLK_9         57
#define SDLK_a         97
#define SDLK_b         98
#define SDLK_c         99
#define SDLK_d         100
#define SDLK_e         101
#define SDLK_f         102
#define SDLK_g         103
#define SDLK_h         104
#define SDLK_i         105
#define SDLK_j         106
#define SDLK_k         107
#define SDLK_l         108
#define SDLK_m         109
#define SDLK_n         110
#define SDLK_o         111
#define SDLK_p         112
#define SDLK_q         113
#define SDLK_r         114
#define SDLK_s         115
#define SDLK_t         116
#define SDLK_u         117
#define SDLK_v         118
#define SDLK_w         119
#define SDLK_x         120
#define SDLK_y         121
#define SDLK_z         122
#define SDLK_UP        273
#define SDLK_DOWN      274
#define SDLK_RIGHT     275
#define SDLK_LEFT      276
#define SDLK_RSHIFT    303
#define SDLK_LSHIFT    304
#define SDLK_RCTRL     305
#define SDLK_LCTRL     306

#endif
