/*
	Tables used by dZ80
*/

#include <stdio.h>
#include <stdlib.h>

#include "dissz80p.h"

const char *VersionString  = "2.0";                         /* Disassembler version */

const char *IMModes[4] 	= {"0", "0", "1", "2"};

const char *Conditions[8]=
{
	"nz",
	"z",
	"nc",
	"c",
	"po",
	"pe",
	"p",
	"m"
};

const char *AccRotType[8]=
{
	"rlca",
	"rrca",
	"rla",
	"rra",
	"daa",
	"cpl",
	"scf",
	"ccf"
};

const char *CBRotType[8] =
{
	"rlc",
	"rrc",
	"rl",
	"rr",
	"sla",
	"sra",
	"sll",
	"srl"
};

const char *Reg8Idx[8] =
{
	"b",
	"c",
	"d",
	"e",
	"h",
	"l",
	"(hl)",
	"a"    
};

const char *Reg8AFIdx[8] =
{
	"b",
	"c",
	"d",
	"e",
	"h",
	"l",
	"f",
	"a"    
};


const char *Reg16Idx[5] =
{
	"bc",
	"de",
	"hl",
	"sp",
	"af"
};

const char *BasicOps[8] =
{
	"add",
	"adc",
	"sub",
	"sbc",
	"and",
	"xor",
	"or",
	"cp"
};

const char *RepeatOps[16] =
{
	"ldi",
	"cpi",
	"ini",
	"outi",

	"ldd",
	"cpd",
	"ind",
	"outd",

	"ldir",
	"cpir",
	"inir",
	"otir",

	"lddr",
	"cpdr",
	"indr",
	"otdr"
};


const char *Z180RepeatOps[4] =
{
	"otim",
	"otdm",
	"otimr",
	"otdmr"
};


const char *dZ80CpuTypeNames[DCPU_TOTAL] =
{
	"Z80GB",
	"Z80",
	"Z180"
};

const char *dZ80ErrorMsgs[DERR_TOTAL] =
{
	"No Error",
	"Out of Memory",
	"Couldn't create file",
	"Couldn't write to file",
	"Bad opcode length",
	"Invalid parameters",
	"Script error",
	"Wrong argument type",
	"File not found",
	"Scripting not available"
};

