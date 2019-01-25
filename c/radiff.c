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
//#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include "ra.h"


void
print_usage(char *argv[])
{

    fprintf(stderr, "Compare two RA files.\n");
    fprintf(stderr, "Usage: %s [-1] [-2] [-h] <file1.ra> <file2.ra>\n",
        argv[0]);
    fprintf(stderr, "\t-1\tCompute L1 distance between data sections.\n");
    fprintf(stderr, "\t-2\tCompute L2 distance between data sections.\n");
    fprintf(stderr, "\t-h\tPrint usage.\n");
    fprintf(stderr,
        "Default behavior is to identify first differing datum.\n");
}

int
main(int argc, char *argv[])
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
        print_usage(argv);
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
