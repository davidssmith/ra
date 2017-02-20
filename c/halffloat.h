/*
 * This chooses between 'ties to even' and 'ties away from zero'.
 */
#define float16_ROUND_TIES_TO_EVEN 1

/*
 ********************************************************************
 *                   HALF-PRECISION ROUTINES                        *
 ********************************************************************
 */

typedef uint16_t float16;

float float16_to_float(float16 h);
double float16_to_double(float16 h);
float16 npy_float_to_half(float f);
float16 npy_double_to_half(double d);

uint16_t npy_floatbits_to_halfbits(uint32_t f);
uint16_t npy_doublebits_to_halfbits(uint64_t d);
uint32_t float16bits_to_floatbits(uint16_t h);
uint64_t float16bits_to_doublebits(uint16_t h);
