#ifndef _ZX81_H_
#define _ZX81_H_

extern unsigned char scrnbmp[],scrnbmpc[];
extern unsigned long frames;

#define CFGBYTE char

#define MACHINEZX80     0
#define MACHINEZX81     1
#define MACHINELAMBDA   4

#define HIRESDISABLED   0
#define HIRESWRX        1
#define HIRESG007       2
#define HIRESMEMOTECH   3

#define CHRGENSINCLAIR  0
#define CHRGENDK        1
#define CHRGENQS        2
#define CHRGENCHR16     3
#define CHRGENLAMBDA    4

#define COLOURDISABLED  0
#define COLOURLAMBDA    2
#define COLOURCHROMA    6

/* Should be in sound.h; known by EightyOne but not (yet) by SZ81 */

#define AY_TYPE_FULLER 3
#define AY_TYPE_ACE 4

/* Used to be in zx81config.h */

#define SYNCTYPEH       1
#define SYNCTYPEV       2

typedef struct
{
        CFGBYTE emuid, major,minor,testver;
        CFGBYTE dirtydisplay;
        CFGBYTE machine;
        CFGBYTE extfont;
        CFGBYTE shadowROM;
        CFGBYTE RAM816k;
        CFGBYTE protectROM;
        CFGBYTE truehires;
        CFGBYTE NTSC;
        CFGBYTE inverse;
        CFGBYTE aysound;
        CFGBYTE aytype;
        CFGBYTE single_step;
        CFGBYTE vsyncsound;
        CFGBYTE beepersound;
        CFGBYTE ts2050;
        CFGBYTE ace96k;
        CFGBYTE TZXin, TZXout;
        CFGBYTE audioout,audioin;
        CFGBYTE colour;
        CFGBYTE debug1, debug2;
        CFGBYTE autoload;
        CFGBYTE wobble;
        CFGBYTE chrgen, enableqschrgen;
        CFGBYTE bordersize;
        CFGBYTE simpleghost;
        CFGBYTE maxireg;
        CFGBYTE zxprinter;
        int RAMTOP;
        int ROMTOP;
        int m1not;
        int romcrc;
        int frameskip;
        int speedup;
        int Chroma81;
        int chromamode;
        int rsz81mem;
        int wsz81mem;
} ZX81;

void zx81_initialise(void);
void zx81_writeport(int Address, int Data, int *tstates);
void mainloop();

#endif
