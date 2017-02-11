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
validate_types (const ra_t* src, const ra_t* dst);
{
    if (dst->eltype == RA_TYPE_USER) {
        printf("USER-defined type must be handled by the USER. :-)\n");
        exit(EX_USAGE);
    } else if ((dst->eltype == RA_TYPE_COMPLEX && src->eltype != RA_TYPE_COMPLEX) ||
               (dst->eltype != RA_TYPE_COMPLEX && src->eltype == RA_TYPE_COMPLEX))
        printf("Cannot convert between complex and non-complex types.\n");
        exit(EX_USAGE);
    } else if (dst->elbyte == src->elbyte && dst->eltype == src->eltype) {
        printf("Specified type is already the type of the source. Nothing to be done.\n");
        exit(EX_OK);
    } else if (src->flags & RA_FLAG_COMPRESSED) {
        printf("Conversion of compressed types is not implemented yet.\n"); 
        exit(EX_USAGE);
    } else if (dst->elbyte < src->elbyte) 
        printf("Destination type is smaller than source. Loss of precision may occur.\n");
}


int
main (int argc, char *argv[])
{
    uint64_t j, nelem;
    ra_t src, dst;
    // conversion helpers
    int8_t *src_i8, *dst_i8;  
    int16_t *src_i16, *dst_i16;
    int32_t *src_i32, *dst_i32;
    int64_t *src_i64, *dst_i64;
    uint8_t *src_u8, *dst_u8;
    uint16_t *src_u16, *dst_u16;
    uint32_t *src_u32, *dst_u32;
    uint64_t *src_u64, *dst_u64;
    float *src_f32, *dst_f32;
    double *src_f64, *dst_f64;

    // read the source file
    if (argc > 4) {
        ra_read(&src, argv[3]);
        dst.eltype = atoi(argv[1]);
        dst.elbyte = atoi(argv[2]); 
        validate_types(&src, &dst);
    } else
        printf("Usage: %s <eltype> <elbyte> <source.ra> <dest.ra>\n", argv[0]);

    // copy the properties that don't change to the destination file
    dst.flags = src.flags;
    dst.ndims = src.ndims;

    dst.dims = (uint64_t*)malloc(dst.ndims*sizeof(uint64_t));
    nelem = 1;
    for (j = 0; j < src.ndims; ++j) {
        dst.dims[j] = src.dims[j];
        nelem *= src.dims[j];
    }

    // convert the data type
    dst.size = dst.elbyte * src.size / src.elbyte;
    dst.data = (uint8_t*)malloc(dst.size);

    if (src.eltype == RA_TYPE_INT            && dst.eltype == RA_TYPE_INT) {  
          
        for (j = 0; j < nelem; ++j)
            
    } else if (src.eltype == RA_TYPE_INT     && dst.eltype == RA_TYPE_UINT) {  

    } else if (src.eltype == RA_TYPE_INT     && dst.eltype == RA_TYPE_FLOAT) { 

    } else if (src.eltype == RA_TYPE_INT     && dst.eltype == RA_TYPE_COMPLEX) { 

    } else if (src.eltype == RA_TYPE_UINT    && dst.eltype == RA_TYPE_INT) {  

    } else if (src.eltype == RA_TYPE_UINT    && dst.eltype == RA_TYPE_UINT) {  

    } else if (src.eltype == RA_TYPE_UINT    && dst.eltype == RA_TYPE_FLOAT) { 

    } else if (src.eltype == RA_TYPE_UINT    && dst.eltype == RA_TYPE_COMPLEX) { 

    } else if (src.eltype == RA_TYPE_FLOAT   && dst.eltype == RA_TYPE_INT) {  

    } else if (src.eltype == RA_TYPE_FLOAT   && dst.eltype == RA_TYPE_UINT) {  

    } else if (src.eltype == RA_TYPE_FLOAT   && dst.eltype == RA_TYPE_FLOAT) { 

    } else if (src.eltype == RA_TYPE_FLOAT   && dst.eltype == RA_TYPE_COMPLEX) { 

    } else if (src.eltype == RA_TYPE_COMPLEX && dst.eltype == RA_TYPE_COMPLEX) {

    } else {
        printf("Specified type and size did not conform to any supported combinations.\n");
        exit(EX_USAGE);
    }

    // write the file
    ra_write(&dst, argv[4]);
    ra_free(&dst);
    ra_free(&src);

    return 0;
}
