
/* Converts RA to Berkeley Advanced Reconstruction Toolbox CFL */

/*
  This file is part of the RA package (http://github.com/davidssmith/ra).

  The MIT License (MIT)

  Copyright (c) 2015 David Smith

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "ra.h"


int
main ()
{
    float2 *r, *s;
    ra_t a,b;
    int k;
    uint64_t N = 12*sizeof(float2);
    printf("test data is %llu floats\n", N/sizeof(float2));
    r = (float2*)malloc(N);
    if (r == NULL)
        printf("could not allocate memory for test data\n");
    for (k = 0; k < N/sizeof(float2); ++k) {
        r[k].x = k;
        r[k].y = -1/(float)k;
    }
    a.flags = 0;
    a.eltype = RA_TYPE_COMPLEX;
    a.elbyte = sizeof(float2);
    a.size = N;
    a.ndims = 2;
    a.dims = (uint64_t*)malloc(a.ndims*sizeof(uint64_t));
    a.dims[0] = 3;
    a.dims[1] = 4;
    a.data = (void*)r;
    ra_write(&a, "test.ra");
    ra_read(&b, "test.ra");
    s = (float2*)b.data;
    for (k = 0; k < b.size/sizeof(float2); ++k) {
        if (r[k].x != s[k].x)
            printf("%f != %f\n",r[k].x, s[k].x);
    }
    for (k = 0; k < 10; ++k)
        printf("%f+%fim\n", s[k].x, s[k].y);
    printf("TESTS PASSED!\n");
    ra_free(&a);
    ra_free(&b);
    ra_query("test.ra");
    return 0;
}



void
ra2cfl (ra_file *r)
{


}
