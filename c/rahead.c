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

#include <stdio.h>
#include <string.h>
#include "ra.h"

int
main(int argc, char *argv[])
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
        printf("Usage: rahead [flags|eltype|elbyte|size|ndims] <file.ra>\n");
    }

    return 0;
}
