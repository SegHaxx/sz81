#ifndef _ZX81CONFIG_H_
#define _ZX81CONFIG_H_

#define BORDERNONE      0
#define BORDERSMALL     1
#define BORDERNORMAL    2
#define BORDERLARGE     3
#define BORDERFULL      4

typedef struct
{
        int sync_len, sync_valid;
        int scanline_len;
        BYTE scanline[4000];
} SCANLINE;

typedef struct
{
        void (*initialise)(void);
        int (*do_scanline)(SCANLINE *line);
        void (*writebyte)(int Address, int Data);
        BYTE (*readbyte)(int Address);
        BYTE (*opcode_fetch)(int Address);
        void (*writeport)(int Address, int Data, int *tstates);
        BYTE (*readport)(int Address, int *tstates);
        int (*contendmem)(int Address, int states, int time);
        int (*contendio)(int Address, int states, int time);
        void (*reset)(void);
        void (*nmi)(void);
        void (*exit)(void);

        int clockspeed;
        int tperscanline;
        int tperframe;
        int intposition;
        int scanlines;
        char CurRom[256];

} MACHINE;

extern MACHINE machine;

/*
#define readbyte(Addr) (machine.readbyte(Addr))
#define writebyte(Addr,Data) (machine.writebyte(Addr,Data))
#define writeport(Addr,Data,tstates) (machine.writeport(Addr,Data,tstates))
#define readport(Addr, tstates) (machine.readport(Addr, tstates))
#define opcode_fetch(Addr) (machine.opcode_fetch(Addr))
*/

#endif
