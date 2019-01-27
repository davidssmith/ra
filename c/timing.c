/*
  This file is part of the RA package (http://github.com/davidssmith/ra).

  The MIT License (MIT)

  Copyright (c) 2015-2019 David Smith

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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ra.h"

static clock_t begin, end;
size_t total_bytes;

float
rasmalltest (size_t n, size_t nfiles)
{
	char filename[32];
	uint64_t dims[] = {0};
	dims[0] = n;
	ra_t *r = ra_create("f4", 1, dims);
	total_bytes = r->size * nfiles;
	begin = clock();
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(filename, "tmp/%ld.ra", i);
		ra_write(r, filename);
	}
	ra_free(r);
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(filename, "tmp/%ld.ra", i);
		//puts(filename);
		ra_read(r, filename);
		ra_free(r);
	}
	end = clock();
	//printf("r.data[0] = %f\n", testval);
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(filename, "tmp/%ld.ra", i);
		unlink(filename);
	}
	float t = (float)(end - begin) / (float)CLOCKS_PER_SEC;
	//float mb = total_bytes * 1e-6;
	free(r);
	return t;
}

float
rabigtest (size_t n, size_t nfiles)
{
	uint64_t dims[] = {0, 0};
	dims[0] = n;
	dims[1] = nfiles;
	ra_t *r = ra_create("f4", 2, dims);
	total_bytes = r->size;
	begin = clock();
	ra_write(r, "tmp/big.ra");
	ra_free(r);
	ra_read(r, "tmp/big.ra");
	end = clock();
	unlink("tmp/big.ra");
	float t = (float)(end - begin) / (float)CLOCKS_PER_SEC;
	ra_free(r);
	free(r);
	return t;
}

void
print_stats (const char *name, float t[], const int navg)
{
	float tavg = 0.f, tmin=1e20, tmax =0;
	for (int i = 0; i < navg; ++i){
		tavg += t[i];
		if (t[i] < tmin) tmin = t[i];
		if (t[i] > tmax) tmax = t[i];
	}
	tavg /= navg;
	printf("%s, %7.2f, MBps avg of %d, %7.2f, min, %7.2f, max\n", 
			name, tavg, navg, tmin, tmax);
}

int
main (int argc, char *argv[])
{
	size_t nfiles = 10000;
	size_t n = 100;
	float mb = 1e-6*n*nfiles*sizeof(float);
	char name[32];

	if(argc < 2) {
		fprintf(stderr, "%s <navg>\n", argv[0]);
		return 1;
	}
	int navg = atoi(argv[1]);
	float *t = (float*)malloc(navg*sizeof(float));

	for (int i = 0; i < navg; ++i) 
		t[i] = mb/rasmalltest(n, nfiles); 
	sprintf(name, "RawArray %ld %ldx1", nfiles, n);
	print_stats(name, t, navg);
	for (int i = 0; i < navg; ++i)
		t[i] = mb/rasmalltest(n*10, nfiles/10);
	sprintf(name, "RawArray %ld %ldx1", nfiles/10, n*10);
	print_stats(name, t, navg);
	for (int i = 0; i < navg; ++i)
		t[i] = mb/rabigtest(n, nfiles);
	sprintf(name, "RawArray 1 %ldx%ld", n,nfiles);
	print_stats(name, t, navg);

    return 0;
}
