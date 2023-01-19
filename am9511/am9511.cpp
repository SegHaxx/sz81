/*
	Skeleton for retropc emulator

	Author : Takeda.Toshiya
	Origin : Xep128
	Date   : 2017.05.09-

	[ AM9511 ]
*/

#include "am9511.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define B24 0x1000000

double my_log2(double x) { return log(x)/M_LN2; }

int my_isfinite(double x) { return !isinf(x) && !isnan(x); }


/* Xep128: Minimalistic Enterprise-128 emulator with focus on "exotic" hardware
   Copyright (C)2014,2015,2016 LGB (Gábor Lénárt) <lgblgblgb@gmail.com>
   http://xep128.lgb.hu/

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <math.h>

/* From my JSep emulator:
 *
 * (C)2014 Gábor Lénárt LGB http://ep.lgb.hu/jsep/
 * Part of my JavaScript based Enterprise-128 emulator ("JSep" aka "webemu").
 * Am9511 "APU" FPU emulation, somewhat (ehhh, a lot!) incorrect 
 * 
 * Thanks to Povi for testing APU support.
 * 
 * http://www.hartetechnologies.com/manuals/AMD/AMD%209511%20FPU.pdf
 * http://www.joelowens.org/z80/am9511algorithms.pdf
 * http://www.joelowens.org/z80/am9511fpmanual.pdf
 * 
 * Major problems with my emulation:
 * 
 * Precision: converts data between APU formats and JS numeric, real Am9511 may give a sightly different results in case of floats.
 * Timing: uses constant timings, real APU varies execution times depending on the operands.
 * Stack content: real APU destroys some elements in case of some OPS other than TOS. This is not emulated.
 * APU status: I am not always sure what status flags modified and how.
 * Results: I am not always sure even about the result of ops. Eg: SMUL/SMUU, what happens on signed values, etc, result can be even WRONG.
 * Usage: emulation always assumes Z80 will be stopped, no WAIT/SRV etc (so bit 7 of command does not count either)
 * Cleanness: my code uses pop/push primitives which is often quite expensive, but the code is more compact and it's enough for a few MHz chip emulation in JS still :)
 */

// Note: NEGARG,ZERODIV,LARGE are truly not independent, you should not mix them, but use only one! Others can be "mixed"

#define _APU_F_CARRY		 1
#define _APU_F_OVERFLOW		 2
#define _APU_F_UNDERFLOW	 4
#define _APU_F_NEGARG		 8
#define _APU_F_ZERODIV		16
#define _APU_F_LARGE		24
#define _APU_F_ZERO		32
#define _APU_F_SIGN		64
#define _APU_F_BUSY	       128

void AM9511::reset()
{
	apu_reset();
}


uint8_t AM9511::apu_read_status()
{
	return _apu_status;
}


void AM9511::_apu_move(int n)
{
	_apu_tos = (_apu_tos + n) & 0xF;
}


uint8_t AM9511::_apu_look8(int depth)
{
	return _apu_stack[(_apu_tos - depth) & 0xF];
}


uint8_t AM9511::_apu_pop8()
{
	_apu_move(-1);
	return _apu_look8(-1);
}


uint8_t AM9511::apu_read_data()
{
	return _apu_pop8();
}


void AM9511::_apu_push8(uint8_t data)
{
	_apu_move(1);
	_apu_stack[_apu_tos] = data;
}


void AM9511::apu_write_data(uint8_t data)
{
	_apu_push8(data);
}


int32_t AM9511::_apu_pop_fix16() {
	int data = _apu_pop8() << 8;
	data |= _apu_pop8();
	if (data & 0x8000) data = data - 0x10000; // two's complement correction
	return data;
}

// push fix16 format, also updates the status (zero, sign, overflow)
void AM9511::_apu_push_fix16(int32_t data) {
	if (data == 0) _apu_status |= _APU_F_ZERO; // zero flag
	else if (data < 0) {
		_apu_status |= _APU_F_SIGN; // negative flag
		data += (USHRT_MAX+1); // two's complement correction
	}
	if (data > USHRT_MAX || data < 0) _apu_status |= _APU_F_OVERFLOW; // overflow flag [WTF]
	_apu_push8(data);
	_apu_push8(data >> 8);
}

int64_t AM9511::_apu_pop_fix32() {
	int64_t data = _apu_pop8() << 24;
	data |= _apu_pop8() << 16;
	data |= _apu_pop8() << 8;
	data |= _apu_pop8();
	if (data > INT_MAX) data = data - (UINT_MAX+1); // two's complement correction
	return data;
}

void AM9511::_apu_push_fix32(int64_t data) {
	if (data == 0) _apu_status |= _APU_F_ZERO;
	else if (data < 0) {
		_apu_status |= _APU_F_SIGN;
		data += (UINT_MAX+1);
	}
	if (data > UINT_MAX || data < 0) _apu_status |= _APU_F_OVERFLOW;
	_apu_push8(data);
	_apu_push8(data >> 8);
	_apu_push8(data >> 16);
	_apu_push8(data >> 24);
}

/* Foreword for FLOAT handling: I use natural float (well, double ...)
 * numberic format of C, using pop/push APU functions to convert from/to.
 * This is kinda messy, and not bit-exact emulation of Am9511.
 * Even my lame push/pop functions can be done much better!!
 */


double AM9511::_apu_pop_float()
{
	int exp = _apu_pop8();
	int data = _apu_pop8() << 16;
	double fdata;
	data |= _apu_pop8() << 8;
	data |= _apu_pop8();
	if (!(data & 0x800000)) return 0.0; // MSB of mantissa must be 1 always, _except_ for the value zero, where all bytes should be zero (including the MSB of mantissa)
	if (exp & 128) data = -data;
	if (exp & 64) exp = (exp & 63) - 64; else exp &= 63;
	fdata = pow(2.0, exp) * ((double)data / B24);
	//DEBUG("APU: float is internally pop'ed: %f" NL, fdata);
	return fdata;
}


void AM9511::_apu_push_float(double data)
{
	int neg, exp , i;

//	printf("_apu_push_float: %f\n", data);

	if (!my_isfinite(data)) { // this should be true for the whole condition of argument is NaN of Infinity ...
		_apu_push8(0); // bad result for NaN, but something should be there (_apu_move() would be better one to "rollback" the stack?!)
		_apu_push8(0);
		_apu_push8(0);
		_apu_push8(0);
		_apu_status |= _APU_F_LARGE;
		return;
	}
	if (data == 0) { // if value is zero, we handle it as a special case, as logarithm function would panic on value of zero.
		_apu_push8(0);
		_apu_push8(0);
		_apu_push8(0);
		_apu_push8(0);
		_apu_status |= _APU_F_ZERO; // zero flag
		return;
	}
	neg = data < 0; // remember the sign of the value (bool)
	data = fabs(data);
	exp = my_log2(data);
	data = data / pow(2.0, exp);
	i = (data * B24);
	if (i >= B24) {
		// ehm, not normalized mantissa or such a problem?
		i >>= 1;
		exp++;
	} else if (i == 0) {
		exp = 0;
		_apu_status |= _APU_F_ZERO | _APU_F_UNDERFLOW; // since we handled zero case at the begining, zero value here means the underflow-gap, I guess
	}
	if (exp > 63) {
		exp &= 63;
		_apu_status |= _APU_F_OVERFLOW;
	} else if (exp < -64) {
		//exp = -((-exp) & 63); // WRONG! TODO, FIXME, HELP, ETC :D
		exp = ((64 + exp) & 63) | 64;
		_apu_status |= _APU_F_OVERFLOW;
	} else if (exp < 0) {
		exp = ((64 + exp) & 63) | 64;
	}
	if (neg) {
		exp |= 128;
		_apu_status |= _APU_F_SIGN; // negative flag
	}
	//if (data && (!(data & 0x800000)))
	//	DEBUG("APU: warning: irregular manitssa: ", data);
	// Pushing 8 bit bytes onto the APU stack
	_apu_push8(i);
	_apu_push8(i >> 8);
	_apu_push8(i >> 16);
	_apu_push8(exp); // this byte holds the exponent, and also the sign of the mantissa
	//if (data == 0) _apu_status |= _APU_F_UNDERFLOW; // hmmm. zero case is handled at the beginning, so if it's zero we are in the underflow-gap of the format. or whatever :D
}


// set S and Z flags of status on TOS, interpreting it as fixed 16 format
void AM9511:: _apu_sz_fix16() {
	if (_apu_look8(0) & 128) _apu_status |= _APU_F_SIGN;
	if (_apu_look8(0) + _apu_look8(1) == 0) _apu_status |= _APU_F_ZERO; // this testing method for zeroness works as apu_look8() gives back only unsigned bytes ...
}
void AM9511::_apu_sz_fix32() {
	if (_apu_look8(0) & 128) _apu_status |= _APU_F_SIGN;
	if (_apu_look8(0) + _apu_look8(1) + _apu_look8(2) + _apu_look8(3) == 0) _apu_status |= _APU_F_ZERO;
}
void AM9511::_apu_sz_float() {
	if (_apu_look8(0) & 128) _apu_status |= _APU_F_SIGN;
	if ((_apu_look8(1) & 128) == 0) _apu_status |= _APU_F_ZERO; // we use only a single bit to test the zeroness of a float.
}


void AM9511::_apu_xchg(int d1, int d2) {
	uint8_t n = _apu_look8(d1);
	_apu_stack[(_apu_tos - d1) & 0xF] = _apu_look8(d2);
	_apu_stack[(_apu_tos - d2) & 0xF] = n;
}
void AM9511::_apu_copy(int from, int to) {
	_apu_stack[(_apu_tos - to) & 0xF] = _apu_look8(from);
}


/* Note, call of this function should be AFTER calling _apu_push* functions as those may set overflow flag we want to keep as cleared here ... 
 * I am still not sure about the difference of overflow and underflow, also not the over-/underflow and carry. For the second problem:
 * it's said that the maximal (or minimal value) can be extended by the carry flag, so there are three cases basically: a number can
 * be represented without overflow and carry, the number can be represented as carry to be thought of the extension of the result,
 * and the overflow, when the result can't represented even with the extended result size by the carry bit. Hmmm. But then, should
 * carry to be set in case of overflow, or not?
 * */
void AM9511::_apu_carry (int64_t val, int64_t limit)
{	
	if (val >= limit * 2 || val < -limit * 2) {
		_apu_status |= _APU_F_OVERFLOW;
		// should carry set here????????????????
		_apu_status |= _APU_F_CARRY;
	} else if (val >= limit || val < -limit) {
		_apu_status &= 255 - _APU_F_OVERFLOW;
		_apu_status |= _APU_F_CARRY;
	}
}


/* Note: most of the command emulation uses the fix32/fix16/float POP/PUSH functions.
 * In some cases it's not the optimal solution (performance) but it's much simpler.
 * However in case of floats it can cause some odd things, ie APU-float<->C-double conversion
 * rounding problems on POP/PUSH ... Well maybe I will deal with this in later versions,
 * now the short solution ... */
void AM9511::apu_write_command (uint8_t cmd)
{
	int32_t i;
	int64_t l;
	double f;

//	printf("apu_write_command: %02x\n", cmd);

	_apu_status = 0; // assume no error

	switch (cmd & 0x7F) { // note, SR (bit7) field of command is currently ignored!
		/* --------------------------------------- */
		/* ---- 16 bit fixed point operations ---- */
		/* --------------------------------------- */
		case 0x6C: // SADD: Add TOS to NOS. Result to NOS. Pop Stack.
			i = _apu_pop_fix16() + _apu_pop_fix16();
			_apu_push_fix16(i);
			_apu_carry(i, USHRT_MAX+1);
			break;
		case 0x6D: // SSUB: Substract TOS from NOS. Result to NOS. Pop Stack.
			i = _apu_pop_fix16();
			i = _apu_pop_fix16() - i;
			_apu_push_fix16(i);
			_apu_carry(i, USHRT_MAX+1);
			break;
		case 0x6E: // SMUL: Multiply NOS by TOS. Lower result to NOS. Pop Stack.
			i = _apu_pop_fix16() * _apu_pop_fix16();
			_apu_push_fix16(i);
			break;
		case 0x76: // SMUU: Multiply NOS by TOS. Upper result to NOS. Pop Stack.
			i = _apu_pop_fix16() * _apu_pop_fix16();
			_apu_push_fix16(i >> 16);
			break;
		case 0x6F: // SDIV: Divide NOS by TOS. Result to NOS. Pop Stack.
			i = _apu_pop_fix16(); // TOS
			if (i) {
				_apu_push_fix16(_apu_pop_fix16() / i);
			} else { // TOS = 0, divide by zero error
				// TOS = 0 case, APU simply puts old NOS as result, that is, leave the original NOS, which is now the TOS
				_apu_status |= _APU_F_ZERODIV;
			}
			break;
		/* --------------------------------------- */
		/* ---- 32 bit fixed point operations ---- */
		/* --------------------------------------- */
		case 0x2C: // DADD: Add TOS to NOS. Result to NOS. Pop Stack.
			l = _apu_pop_fix32() + _apu_pop_fix32();
			_apu_push_fix32(l);
			_apu_carry(l, UINT_MAX+1);
			break;
		case 0x2D: // DSUB: Substract TOS from NOS. Result to NOS. Pop Stack.
			l = _apu_pop_fix32();
			l = _apu_pop_fix32() - l;
			_apu_push_fix32(l);
			_apu_carry(l, UINT_MAX+1);
			break;
		case 0x2E: // DMUL: Multiply NOS by TOS. Lower result to NOS. Pop Stack.
			l = _apu_pop_fix32() * _apu_pop_fix32();
			_apu_push_fix32(l);
			break;
		case 0x36: // DMUU: Multiply NOS by TOS. Upper result to NOS. Pop Stack.
			l = _apu_pop_fix32() * _apu_pop_fix32();
			_apu_push_fix32(l >> 32);
			break;
		case 0x2F: // DDIV: Divide NOS by TOS. Result to NOS. Pop Stack.
			l = _apu_pop_fix32(); // TOS
			if (l) {
				_apu_push_fix32(_apu_pop_fix32() / l);
			} else { // TOS = 0, divide by zero error
				// TOS = 0 case, APU simply puts old NOS as result, that is, leave the original NOS, which is now the TOS
				_apu_status |= _APU_F_ZERODIV;
			}
			break;
		/* -------------------------------------------------- */
		/* ---- 32 bit floating point primary operations ---- */
		/* -------------------------------------------------- */
		case 0x10: // FADD: Add TOS to NOS. Result to NOS. Pop Stack.
			f = _apu_pop_float();
			_apu_push_float(_apu_pop_float() + f);
			break;
		case 0x11: // FSUB: Substract TOS from NOS. Result to NOS. Pop Stack.
			f = _apu_pop_float();
			_apu_push_float(_apu_pop_float() - f);
			break;
		case 0x12: // FMUL: Multiply NOS by TOS. Result to NOS. Pop Stack.
			_apu_push_float(_apu_pop_float() * _apu_pop_float());
			break;
		case 0x13: // FDIV: Divide NOS by TOS. Result to NOS. Pop Stack.
			f = _apu_pop_float();
			if (f) {
				_apu_push_float(_apu_pop_float() / f);
			} else { // TOS = 0, divide by zero error
				// TOS = 0 case, APU simply puts old NOS as result, that is, leave the original NOS, which is now the TOS
				_apu_status |= _APU_F_ZERODIV;
			}
			break;
		/* -------------------------------------------------- */
		/* ---- 32 bit floating point derived operations ---- */
		/* -------------------------------------------------- */
		case 0x01: // SQRT: Square Root of TOS. Result to TOS.
			f = _apu_pop_float();
			_apu_push_float(sqrt(fabs(f))); // we still want to do something with negative number ..., so use fabs() but set the error status on the next line too
			if (f < 0) _apu_status |= _APU_F_NEGARG; // negative argument signal
			break;
		case 0x02: // SIN: Sine of TOS. Result to TOS.
			_apu_push_float(sin(_apu_pop_float()));
			break;
		case 0x03: // COS: Cosine of TOS. Result to TOS.
			_apu_push_float(cos(_apu_pop_float()));
			break;
		case 0x04: // TAN: Tangent of TOS. Result to TOS.
			_apu_push_float(tan(_apu_pop_float()));
			break;
		case 0x05: // ASIN: Inverse Sine of TOS. Result to TOS.
			_apu_push_float(asin(_apu_pop_float()));
			break;
		case 0x06: // ACOS: Inverse Cosine of TOS. Result to TOS.
			_apu_push_float(acos(_apu_pop_float()));
			break;
		case 0x07: // ATAN: Inverse Tangent of TOS. Result to TOS.
			_apu_push_float(atan(_apu_pop_float()));
			break;
		case 0x08: // LOG: Common Logarithm of TOS. Result to TOS.
			f = _apu_pop_float();
			if (f > 0) {
				_apu_push_float(log10(f));
			} else {
				_apu_status |= _APU_F_NEGARG;
				_apu_move(4);
			}
			break;
		case 0x09: // LN: Natural Logarithm of TOS. Result to TOS.
			f = _apu_pop_float();
			if (f > 0) {
				_apu_push_float(log(f));
			} else {
				_apu_status |= _APU_F_NEGARG;
				_apu_move(4);
			}
			break;
		case 0x0A: // EXP: "e" raised to power in TOS. Result to TOS.
			f = _apu_pop_float();
			_apu_push_float(pow(M_E, f));
			break;
		case 0x0B: // PWR: NOS raised to power in TOS. Result to TOS. Pop Stack.
			f = _apu_pop_float();
			_apu_push_float(pow(_apu_pop_float(), f));
			break;
		/* ------------------------------------------------ */
		/* ---- data and stack manipulation operations ---- */
		/* ------------------------------------------------ */
		case 0x00: // NOP: does nothing (but clears status)
			break;
		case 0x1F: // FIXS: Convert TOS from floating point format to fixed point format (16 bit).
			_apu_push_fix16(_apu_pop_float());
			break;
		case 0x1E: // FIXD: Convert TOS from floating point format to fixed point format (32 bit).
			_apu_push_fix32(_apu_pop_float());
			break;
		case 0x1D: // FLTS: Convert TOS from fixed point format (16 bit) to floating point format.
			_apu_push_float(_apu_pop_fix16());
			break;
		case 0x1C: // FLTD: Convert TOS from fixed point format (32 bit) to floating point format.
			_apu_push_float(_apu_pop_fix32());
			break;
		case 0x74: // CHSS: Change sign of fixed point (16 bit) operand on TOS.
			_apu_push_fix16(-_apu_pop_fix16());
			break;
		case 0x34: // CHSD: Change sign of fixed point (32 bit) operand on TOS.
			_apu_push_fix32(-_apu_pop_fix32());
			break;
		case 0x15: // CHSF: Change sign of floating point operand on TOS. Note: that does not seem to be a big issue, as a single bit should be modified??
			if (_apu_look8(1) & 128) { // if number is not zero
				_apu_stack[_apu_tos] ^= 128;
				if (_apu_stack[_apu_tos] & 128) _apu_status |= _APU_F_SIGN;
			} else // if number is zero, nothing happens (but we set zero flag)
				_apu_status |= _APU_F_ZERO;
			break;
		case 0x77: // PTOS: Push stack. Duplicate NOS to TOS.
			_apu_move(2);
			_apu_copy(2, 0);
			_apu_copy(3, 1);
			_apu_sz_fix16();
			break;
		case 0x37: // PTOD: Push stack. Duplicate NOS to TOS.
			_apu_move(4);
			_apu_copy(4, 0);
			_apu_copy(5, 1);
			_apu_copy(6, 2);
			_apu_copy(7, 3);
			_apu_sz_fix32();
			break;
		case 0x17: // PTOF: Push stack. Duplicate NOS to TOS.
			_apu_move(4);
			_apu_copy(4, 0);
			_apu_copy(5, 1);
			_apu_copy(6, 2);
			_apu_copy(7, 3);
			_apu_sz_float();
			break;
		case 0x78: // POPS: Pop stack. Old NOS becomes new TOS, old TOS rotates to bottom.
			_apu_move(-2);
			_apu_sz_fix16(); // set "sz" (S and Z status flags) by inspecting (new) TOS
			break;
		case 0x38: // POPD: Pop stack. Old NOS becomes new TOS, old TOS rotates to bottom.
			_apu_move(-4);
			_apu_sz_fix32();
			break;
		case 0x18: // POPF: Pop stack. Old NOS becomes new TOS, old TOS rotates to bottom.
			_apu_move(-4);
			_apu_sz_float();
			break;
		case 0x79: // XCHS: Exchange NOS and TOS. (16 bit fixed)
			_apu_xchg(0, 2);
			_apu_xchg(1, 3);
			_apu_sz_fix16();
			break;
		case 0x39: // XCHD: Exchange NOS and TOS. (32 bit fixed)
			_apu_xchg(0, 4);
			_apu_xchg(1, 5);
			_apu_xchg(2, 6);
			_apu_xchg(3, 7);
			_apu_sz_fix32();
			break;
		case 0x19: // XCHF: Exchange NOS and TOS. (float stuff)
			_apu_xchg(0, 4);
			_apu_xchg(1, 5);
			_apu_xchg(2, 6);
			_apu_xchg(3, 7);
			_apu_sz_float();
			break;
		case 0x1A: // PUPI: Push floating point constant PI onto TOS. Previous TOS becomes NOS.
			_apu_push8(0xDA);
			_apu_push8(0x0F);
			_apu_push8(0xC9);
			_apu_push8(0x02);
			break;
		default:
			fprintf(stderr,"APU: not implemented/unknown Am9511 command: %02Xh\n", cmd);
			break;
	}
}


void AM9511::apu_reset()
{
	_apu_status = 0;
	_apu_tos = 0;
	memset(_apu_stack, 0, sizeof(_apu_stack));
}

void AM9511::write_io8(uint8_t addr, uint8_t data)
{
	if (addr & 0x80)
		apu_write_command(data);
	else
		apu_write_data(data);
}

uint8_t AM9511::read_io8(uint8_t addr)
{
	if (addr & 0x80)
		return apu_read_status();
	else
		return apu_read_data();
}

AM9511 AMinstance;

extern "C" {

void am_reset()
{
	AMinstance.reset();
}

void am_write_io8(uint8_t addr, uint8_t data)
{
//	printf("W %02x %02x\n", addr, data);
	AMinstance.write_io8(addr, data);
}

uint8_t am_read_io8(uint8_t addr)
{
	uint8_t data;
	data = AMinstance.read_io8(addr);
//	printf("R %02x %02x\n", addr, data);
	return data;
}

}
