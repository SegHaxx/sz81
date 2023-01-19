// ZXpand_dll.cpp : Defines the exported functions for the DLL application.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "zxpand_emu.h"
#include "zxpandclass.h"

zxpand_t _zxpand;
extern BYTE mode;

extern "C" void ZXpand_Update(int millis)
{
	_zxpand.periodic_update(millis);
}

extern "C" void ZXpand_IO_Write(int addr, int data)
{
      if(addr == 0xff)
      {
         FILE* dbgfile = fopen("dbg.txt", "a");
         if (dbgfile)
         {
            fprintf(dbgfile,"W %02X %d\n", data, mode);
            fclose(dbgfile);
         }
      }

      _zxpand.iowr((BYTE)addr, (BYTE)data);
}

extern "C" int ZXpand_IO_ReadStatus()
{
   return _zxpand.activityFlag;
}

extern "C" int ZXpand_IO_Read(int addr)
{
      return _zxpand.iord(BYTE(addr));
}
