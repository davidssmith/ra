/*
  This file is part of the RA package (http://github.com/davidssmith/ra).

  The MIT License (MIT)

  Copyright (c) 2015-2017 David Smith

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

#include <math.h>
#include <sysexits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "ra.h"


void
diff_print_usage(char *argv[])
{

    fprintf(stderr, "Compare two RA files.\n");
    fprintf(stderr, "Usage: ra %s [-1] [-2] [-h] <file1.ra> <file2.ra>\n",
        argv[0]);
    fprintf(stderr, "\t-1\tCompute L1 distance between data sections.\n");
    fprintf(stderr, "\t-2\tCompute L2 distance between data sections.\n");
    fprintf(stderr, "\t-h\tPrint usage.\n");
    fprintf(stderr,
        "Default behavior is to identify first differing datum.\n");
}

int
diff(int argc, char *argv[])
{
    ra_t r1, r2;
    int isdiff = 0;
    int diff_type = 0;
    int c;
    while ((c = getopt(argc, argv, "12h")) != -1)
    {
        switch (c) {
        case '2':
            diff_type = 2;
            break;
        case '1':
            diff_type = 1;
            break;
        }
    }

    if (argc == optind)
    {
        diff_print_usage(argv);
        return EX_USAGE;
    }
    for (int index = optind; index < argc; index++)
    {
        if (index == optind)
            ra_read(&r1, argv[index]);
        else if (index == optind + 1)
            ra_read(&r2, argv[index]);
    }
    ra_diff(&r1, &r2, diff_type);
    ra_free(&r1);
    ra_free(&r2);
    return isdiff;
}

int
dims (int argc, char *argv[])
{
    if (argc > 1) {
		ra_t r;
		ra_read_header(&r, argv[1]);
		for (uint64_t i = 0; i < r.ndims; ++i)
			printf("%lu ", r.dims[i]);
		printf("\n");
		ra_free(&r);
    } else {
        printf("Print dimensions of ra file.\n");
        printf("Usage: ra dims <file.ra>\n");
    }

    return 0;
}

int
head(int argc, char *argv[])
{
    if (argc > 2) 
	{
		if (strncmp(argv[1], "flags", 5) == 0)
		{
			printf("%lu\n", ra_flags(argv[2]));
		}
		else if (strncmp(argv[1], "eltype", 6) == 0)
		{
			printf("%lu\n", ra_eltype(argv[2]));
		}
		else if (strncmp(argv[1], "elbyte", 6) == 0)
		{
			printf("%lu\n", ra_elbyte(argv[2]));
		}
		else if (strncmp(argv[1], "size", 4) == 0)
		{
			printf("%lu\n", ra_size(argv[2]));
		}
		else if (strncmp(argv[1], "ndims", 5) == 0)
		{
			printf("%lu\n", ra_ndims(argv[2]));
		}
		else
		{
        	ra_print_header(argv[1]);
		}
	}
	else if (argc > 1)
        ra_print_header(argv[1]);
    else
    {
        printf("View header of ra file.\n");
        printf("Usage: ra head [flags|eltype|elbyte|size|ndims] <file.ra>\n");
    }

    return 0;
}



int
reshape(int argc, char *argv[])
{
    ra_t r;
    uint64_t *newdims;
    uint64_t ndimsnew;
    if (argc > 2)
    {
        ra_read(&r, argv[1]);
        ndimsnew = argc - 2;
        newdims = (uint64_t *) malloc(ndimsnew * sizeof(uint64_t));
        for (uint64_t k = 0; k < ndimsnew; ++k)
            newdims[k] = atol(argv[k + 2]);
        if (ra_reshape(&r, newdims, ndimsnew) == 0)
        {
            // TODO: just write the header if total elements still the same
            ra_write(&r, argv[1]);
        }
        ra_free(&r);
        free(newdims);
    }
    else
    {
        fprintf(stderr, "Reshape ra file.\n");
        fprintf(stderr, "Usage: ra %s file.ra n1 n2 ...\n", argv[0]);
        return EX_USAGE;
    }
    return EX_OK;
}


int
compress (int argc, char *argv[])
{
	ra_t r;
	if (argc < 2) {
		printf("ra compress <file.ra>\n");
		return EX_USAGE;
	}
	ra_read(&r, argv[1]);
	ra_compress(&r);
	ra_write(&r, argv[1]);
	ra_free(&r);
	return EX_OK;
}


int
decompress (int argc, char *argv[])
{
	ra_t r;
	if (argc < 2) {
		printf("ra decompress <file.ra>\n");
		return EX_USAGE;
	}
	ra_read(&r, argv[1]);
	ra_decompress(&r);
	ra_write(&r, argv[1]);
	ra_free(&r);
	return EX_OK;
}

void
print_usage()
{
		printf("Usage: ra [diff|head|reshape|compress|decompress] <options>\n");
}

int
main (int argc, char *argv[])
{
	if (argc < 2) {
		print_usage();
		return EX_USAGE;
	}
	if (strncmp(argv[1], "diff", 4) == 0)
		diff(argc-1, argv+1);
	else if (strncmp(argv[1], "head", 4) == 0)
		head(argc-1, argv+1);
	else if (strncmp(argv[1], "reshape", 7) == 0)
		reshape(argc-1, argv+1);
	else if (strncmp(argv[1], "dims", 4) == 0)
		dims(argc-1, argv+1);
	else if (strncmp(argv[1], "head", 4) == 0)
		head(argc-1, argv+1);

	else if (strncmp(argv[1], "compress", 8) == 0)
		compress(argc-1, argv+1);
	else if (strncmp(argv[1], "decompress", 10) == 0)
		decompress(argc-1, argv+1);
	else  {
		print_usage();
		return EX_USAGE;
	}

	return EX_OK;
}
