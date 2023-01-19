#include <stdlib.h>
#include "zxpandclass.h"
#include "usart.h"

bool L_LOW;
bool disabled;
extern void zx_initprocessor(void);
extern void zx_process_write(void);
extern void SetRoot(const char*);
extern void initJoy(void);
extern unsigned char readJoy(void);

BYTE* gdp;
BYTE mode;

BYTE ring_error;

extern void mapJS(unsigned char, unsigned char);

void tryProgramCPLD(const char* p){}
extern void ringReset(void){}

zxpand_t::zxpand_t()
{
   init = zx_initprocessor;
   process = zx_process_write; /* process is only assigned now and here */
   PORTB = 0xf9; /* seems to be unused */

   disabled = false;
   L_LOW = true;

   mapJS(0, 0x23); // 7
   mapJS(1, 0x22); // 6
   mapJS(2, 0x21); // 5
   mapJS(3, 0x24); // 8
   mapJS(4, 0x1b); // 0

   initJoy();

   power_on_reset();

   SetRoot("ZXpand_SD_Card");

   configByte = eeprom[4];
   fsConfig = eeprom[5];
}

bool zxpand_t::isDisabled(void)
{
   return disabled;
}

bool zxpand_t::isLow(void)
{
   return L_LOW;
}
