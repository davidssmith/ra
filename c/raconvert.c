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

#include <sysexits.h>
#include <types.h>
#include "ra.h"

void
validate_types (const uint64_t dst_type, const uint64_t src_type)
{
    if (dst_type == RA_TYPE_USER) {
        printf("User-defined type must be handled by the _USER_.\n");
        exit(EX_USAGE);
    } else if ((dst_type == RA_TYPE_COMPLEX && src_type != RA_TYPE_COMPLEX) ||
               (dst_type != RA_TYPE_COMPLEX && src_type == RA_TYPE_COMPLEX))
        printf("Cannot convert between complex and non-complex types.\n");
        exit(EX_USAGE);
    } else if (dst_elsize < src_elsize) 
        printf("Destination type is smaller than source. Loss of precision may occur.\n");
}

int
main (int argc, char *argv[])
{
    uint64_t j;
    ra_t src, dst;

    // read the source file
    if (argc > 4) {
        ra_read(&src, argv[3]);
        dst.eltype = atoi(argv[1]);
        dst.elbyte = atoi(argv[2]); 
        validate_types(src.eltype, dst.eltype);
    } else
        printf("Usage: %s <eltype> <elbyte> <source.ra> <dest.ra>\n", argv[0]);

    // copy the properties that don't change to the destination file
    dst.flags = src.flags;
    dst.ndims = src.ndims;

    dst.dims = (uint64_t*)malloc(dst.ndims*sizeof(uint64_t));
    for (j = 0; j < src.ndims; ++j)
        dst.dims[j] = src.dims[j];

    // convert the data type
    dst.size = dst.elbyte * src.size / src.elbyte;
    dst.data = (uint8_t*)malloc(dst.size);



    if (dst.eltype == RA_TYPE_INT && src.eltype == RA_TYPE_INT)
    {
            // stuff
            break;
        case RA_TYPE_UINT:
            // stuff
            break;
        case RA_TYPE_FLOAT:
            // stuff
            break;
        case RA_TYPE_COMPLEX:
            // stuff
            break;
    }

    // write the file
    ra_write(&dst, argv[4]);
    ra_free(&dst);
    ra_free(&src);

    return 0;
}
