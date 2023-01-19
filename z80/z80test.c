#include <stdio.h>

#include "z80.h"
#include "z80_macros.h"
#include "zx81config.h"

BYTE memory[1024 * 1024];

int zx81_contend(int Address, int states, int time)
{
        return(time);
}

BYTE zx81_readbyte(int Address)
{
        BYTE data;
	data = memory[Address];
	return data;
}

void zx81_writebyte(int Address, int Data)
{
}

BYTE zx81_opcode_fetch(int Address)
{
        BYTE data;
	data = zx81_readbyte(Address);
	return data;
}

BYTE zx81_readport(int Address, int *tstates)
{
	return 0xff;
}

void zx81_writeport(int Address, int Data, int *tstates)
{
}

int main()
{
	int ts;

//        machine.initialise = zx81_initialise;
//        machine.do_scanline = zx81_do_scanline;
        machine.writebyte = zx81_writebyte;
        machine.readbyte = zx81_readbyte;
        machine.opcode_fetch = zx81_opcode_fetch;
        machine.readport = zx81_readport;
        machine.writeport = zx81_writeport;
        machine.contendmem = zx81_contend;
        machine.contendio = zx81_contend;
        machine.reset = NULL;
        machine.nmi = NULL;
        machine.exit = NULL;

	z80_init();
	z80_reset();

	printf("PC: %d", PC);

	ts = z80_do_opcode();

	printf(" TS: %d\n",ts);

	return 0;
}
