#ifndef _FLOAT16_H
#define _FLOAT16_H

/*
 * This was translated from base/float16.jl in the Julia distribution. 
 * License is MIT: http://julialang.org/license
 */

#include <inttypes.h>

typedef uint16_t float16;

#ifdef __cplusplus
extern "C" {
#endif

float f16tof32 (const float16 val);
void fill_float16_tables();
float16 f32tof16 (const float val);

#ifdef __cplusplus
}
#endif

#endif /* _FLOAT16_H */
