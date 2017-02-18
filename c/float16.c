/*
 * This was translated from base/float16.jl in the Julia distribution. 
 * License is MIT: http://julialang.org/license
 */

#include <inttypes.h>
#include "float16.h"

float 
f16tof32 (const float16 val)
{
    uint32_t ival, sign, exp, sig, ret;
    ival = (uint16_t)val;
    sign = (ival & 0x8000) >> 15;
    exp  = (ival & 0x7c00) >> 10;
    sig  = (ival & 0x3ff) >> 0;

    if (exp == 0) {
        if (sig == 0) {
            sign = sign << 31;
            ret = sign | exp | sig;
        } else {
            int n_bit = 1;
            int bit = 0x0200;
            while ((bit & sig) == 0) {
                n_bit = n_bit + 1;
                bit = bit >> 1;
            }
            sign = sign << 31;
            exp = (-14 - n_bit + 127) << 23;
            sig = ((sig & (~bit)) << n_bit) << (23 - 10);
            ret = sign | exp | sig;
        }
    } else if (exp == 0x1f) {
        if (sig == 0) { // Inf
            if (sign == 0)
                ret = 0x7f800000;
            else
                ret = 0xff800000;
        } else  // NaN
            ret = 0x7fc00000 | (sign<<31);
    } else {
        sign = sign << 31;
        exp  = (exp - 15 + 127) << 23;
        sig  = sig << (23 - 10);
        ret = sign | exp | sig;
    }
    return (float)ret;
}

//float -> float16 algorithm from:
//   "Fast Half Float Conversion" by Jeroen van der Zijp
//   ftp://ftp.fox-toolkit.org/pub/fasthalffloatconversion.pdf

static uint16_t basetable[512];
static uint8_t shifttable[512];


void 
fill_tables()
{
    for (int i = 0; i <= 255; ++i) {
        int e = i - 127;
        if (e < -24)  { // Very small numbers map to zero
            basetable[i|0x000+1] = 0x0000;
            basetable[i|0x100+1] = 0x8000;
            shifttable[i|0x000+1] = 24;
            shifttable[i|0x100+1] = 24;
        } else if (e < -14) {  // Small numbers map to denorms
            basetable[i|0x000+1] = (0x0400>>(-e-14));
            basetable[i|0x100+1] = (0x0400>>(-e-14)) | 0x8000;
            shifttable[i|0x000+1] = -e-1;
            shifttable[i|0x100+1] = -e-1;
        } else if (e <= 15) {  // Normal numbers just lose precision
            basetable[i|0x000+1] = ((e+15)<<10);
            basetable[i|0x100+1] = ((e+15)<<10) | 0x8000;
            shifttable[i|0x000+1] = 13;
            shifttable[i|0x100+1] = 13;
        } else if (e < 128) {  // Large numbers map to Infinity
            basetable[i|0x000+1] = 0x7C00;
            basetable[i|0x100+1] = 0xFC00;
            shifttable[i|0x000+1] = 24;
            shifttable[i|0x100+1] = 24;
        } else { // Infinity and NaN's stay Infinity and NaN's
            basetable[i|0x000+1] = 0x7C00;
            basetable[i|0x100+1] = 0xFC00;
            shifttable[i|0x000+1] = 13;
            shifttable[i|0x100+1] = 13;
        }
    }
}

float16
f32tof16 (const float val)
{
    uint32_t f = (uint32_t)val;
    int i = (f >> 23) & 0x1ff + 1;
    uint8_t sh = shifttable[i];
    f &= 0x007fffff;
    uint16_t h = basetable[i] + (f >> sh);
    // round
    // NOTE: we maybe should ignore NaNs here, but the payload is
    // getting truncated anyway so "rounding" it might not matter
    uint32_t nextbit = (f >> (sh-1)) & 1;
    if (nextbit != 0)
        if ((h&1) == 1 ||  // round halfway to even
            (f & ((1<<(sh-1))-1)) != 0)  // check lower bits
            h += 1;
    return (float16)h;
}
