#include <stdio.h>
#include <stdint.h>
#include "float16.h"


void
test_float16( float x)
{
        float y,z;
        uint16_t i;
        uint32_t *ux = (uint32_t*)&x;
        i = float_to_float16(x);
        y = float16_to_float(i);
        z = float16_to_float(float_to_float16(y));
        printf("%20f%20x%20x%20f%20f\n", x, *ux, i, y, z);
}



int
main ()
{
    float test_arr[] = {3.14f, 1.f, 1.e6, -1e6, 1e-6, 1e4, 1e-4, -1e3, -1e-3}; 
    int i;
    for (i = 0; i < sizeof(test_arr)/sizeof(float); ++i)
        test_float16(test_arr[i]);
    return 0;
}
