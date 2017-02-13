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
#include <stdint.h>
#include "ra.h"


void
validate_types (const ra_t* src, const ra_t* dst)
{
    if (dst->eltype == RA_TYPE_USER) {
        printf("USER-defined type must be handled by the USER. :-)\n");
        exit(EX_USAGE);
    } else if (src->eltype == RA_TYPE_COMPLEX && dst->eltype != RA_TYPE_COMPLEX) {
        printf("Warning: converting complex to non-complex types may discard information.\n");
    } else if (dst->elbyte == src->elbyte && dst->eltype == src->eltype) {
        printf("Specified type is already the type of the source. Nothing to be done.\n");
        exit(EX_OK);
    } else if (src->flags & RA_FLAG_COMPRESSED) {
        printf("Conversion of compressed types is not implemented yet.\n"); 
        exit(EX_USAGE);
    } else if (dst->elbyte < src->elbyte) 
        printf("Warning: reducing type size may cause loss of precision.\n");
}


#define CASE(TYPE1,BYTE1,TYPE2,BYTE2) \
    (src.eltype == RA_TYPE_##TYPE1 && src.elbyte == BYTE1 && \
     dst.eltype == RA_TYPE_##TYPE2 && dst.elbyte == BYTE2)

#define CONVERT(TYPE1,TYPE2) { \
    TYPE1 *tmp_src; tmp_src = (TYPE1 *)src.data; \
    TYPE2 *tmp_dst; tmp_dst = (TYPE2 *)dst.data; \
    for (size_t i = 0; i < nelem; ++i) tmp_dst[i] = tmp_src[i]; }


int
main (int argc, char *argv[])
{
    uint64_t j, nelem;
    ra_t src, dst;

    // read the source file
    if (argc > 4) {
        ra_read(&src, argv[3]);
        dst.eltype = atoi(argv[1]);
        dst.elbyte = atoi(argv[2]); 
        validate_types(&src, &dst);
    } else {
        printf("Usage: %s <eltype> <elbyte> <source.ra> <dest.ra>\n", argv[0]);
        exit(EX_USAGE);
    }

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

    if CASE(INT,1,INT,2)             // INT -> INT
        CONVERT(int8_t,int16_t) 
    else if CASE(INT,1,INT,4)
        CONVERT(int8_t,int32_t) 
    else if CASE(INT,1,INT,8)
        CONVERT(int8_t,int64_t) 

    else if CASE(INT,2,INT,1) 
        CONVERT(int16_t,int8_t) 
    else if CASE(INT,2,INT,4)
        CONVERT(int16_t,int32_t) 
    else if CASE(INT,2,INT,8)
        CONVERT(int16_t,int64_t) 

    else if CASE(INT,4,INT,1) 
        CONVERT(int32_t,int8_t) 
    else if CASE(INT,4,INT,2)
        CONVERT(int32_t,int16_t) 
    else if CASE(INT,4,INT,8)
        CONVERT(int32_t,int64_t) 

    else if CASE(UINT,8,INT,1) 
        CONVERT(uint64_t,int8_t) 
    else if CASE(UINT,8,INT,2)
        CONVERT(uint64_t,int16_t) 
    else if CASE(UINT,8,INT,4)
        CONVERT(uint64_t,int32_t) 

    else if CASE(UINT,1,INT,2)        // UINT -> INT
        CONVERT(uint8_t,int16_t) 
    else if CASE(UINT,1,INT,4)
        CONVERT(uint8_t,int32_t) 
    else if CASE(UINT,1,INT,8)
        CONVERT(uint8_t,int64_t) 

    else if CASE(UINT,2,INT,1) 
        CONVERT(uint16_t,int8_t) 
    else if CASE(UINT,2,INT,4)
        CONVERT(uint16_t,int32_t) 
    else if CASE(UINT,2,INT,8)
        CONVERT(uint16_t,int64_t) 

    else if CASE(UINT,4,INT,1) 
        CONVERT(uint32_t,int8_t) 
    else if CASE(UINT,4,INT,2)
        CONVERT(uint32_t,int16_t) 
    else if CASE(UINT,4,INT,8)
        CONVERT(uint32_t,int64_t) 

    else if CASE(UINT,8,INT,1) 
        CONVERT(uint64_t,int8_t) 
    else if CASE(UINT,8,INT,2)
        CONVERT(uint64_t,int16_t) 
    else if CASE(UINT,8,INT,4)
        CONVERT(uint64_t,int32_t) 

    else if CASE(UINT,1,UINT,2)        // UINT -> UINT
        CONVERT(uint8_t,uint16_t) 
    else if CASE(UINT,1,UINT,4)
        CONVERT(uint8_t,uint32_t) 
    else if CASE(UINT,1,UINT,8)
        CONVERT(uint8_t,uint64_t) 

    else if CASE(UINT,2,UINT,1) 
        CONVERT(uint16_t,uint8_t) 
    else if CASE(UINT,2,UINT,4)
        CONVERT(uint16_t,uint32_t) 
    else if CASE(UINT,2,UINT,8)
        CONVERT(uint16_t,uint64_t) 

    else if CASE(UINT,4,UINT,1) 
        CONVERT(uint32_t,uint8_t) 
    else if CASE(UINT,4,UINT,2)
        CONVERT(uint32_t,uint16_t) 
    else if CASE(UINT,4,UINT,8)
        CONVERT(uint32_t,uint64_t) 

    else if CASE(UINT,8,UINT,1) 
        CONVERT(uint64_t,uint8_t) 
    else if CASE(UINT,8,UINT,2)
        CONVERT(uint64_t,uint16_t) 
    else if CASE(UINT,8,UINT,4)
        CONVERT(uint64_t,uint32_t) 

    else if CASE(FLOAT,4,FLOAT,8)     // FLOATS AND COMPLEX
        CONVERT(float,double) 
    else if CASE(FLOAT,8,FLOAT,4)     
        CONVERT(double,float) 
    else if CASE(COMPLEX,16,COMPLEX,8) {
        nelem *= 2;
        CONVERT(double,float) 
    } else if CASE(COMPLEX,8,COMPLEX,16) {
        nelem *= 2;
        CONVERT(float,double) 
    } else if CASE(FLOAT,4,COMPLEX,8) {
        float *tmp_src = (float *)src.data; 
        float *tmp_dst = (float *)dst.data;
        for (size_t i = 0; i < nelem; ++i) {
            tmp_dst[2*i] = tmp_src[i]; 
            tmp_dst[2*i+1] = 0.f;
        }
    } else if CASE(FLOAT,8,COMPLEX,16) {
        double *tmp_src = (double *)src.data; 
        double *tmp_dst = (double *)dst.data;
        for (size_t i = 0; i < nelem; ++i) {   
            tmp_dst[2*i] = tmp_src[i]; 
            tmp_dst[2*i+1] = 0.;
        }
    } else if CASE(COMPLEX,8,FLOAT,4) {       // complex -> float using real part
        float *tmp_src = (float *)src.data; 
        float *tmp_dst = (float *)dst.data;
        for (size_t i = 0; i < nelem; ++i) {
            tmp_dst[i] = tmp_src[2*i]; 
        }
    } else if CASE(COMPLEX,16,FLOAT,8) {
        double *tmp_src = (double *)src.data; 
        double *tmp_dst = (double *)dst.data;
        for (size_t i = 0; i < nelem; ++i) {   
            tmp_dst[i] = tmp_src[2*i]; 
        }
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
