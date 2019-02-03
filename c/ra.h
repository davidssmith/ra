#ifndef _RA_H
#define _RA_H

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

#include <stdint.h>
#include "lz4.h"

/*
   File layout

   Additional info can be stored after the data region with no harmful effects.
*/
typedef struct {
	uint64_t magic; 
    uint64_t flags;             /* file properties, such as endianness and future capabilities */
    uint64_t eltype;            /* enum representing the element type in the array */
    uint64_t elbyte;            /* # of bytes in type's canonical representation */
    uint64_t size;              /* size of data in bytes (may be compressed: check 'flags') */
    uint64_t ndims;             /* number of dimensions in array */
    uint64_t *dims;             /* the actual dimensions */
    uint8_t *data;              /* pointer to raw data -- contiguous -- so can mmap if y'ant'ta.
                                   Use chars to handle generic data, since reader can use 'type'
                                   enum to recreate correct pointer cast */
	uint8_t *top;               /* pointer to top of the memory area holding the file in RAM */
} ra_t;


static const uint64_t RA_MAGIC_NUMBER = 0x7961727261776172ULL;

/* flags */
#define NFLAGS              2
#define RA_DEFAULT          0
#define RA_FLAG_BIG_ENDIAN  (1ULL<<0)
#define RA_FLAG_COMPRESSED  (1ULL<<1)
#define RA_UNKNOWN_FLAGS    (-(1LL<<NFLAGS))

/* maximum size that read system call can handle */
#define RA_MAX_BYTES  (1ULL<<31)

/* elemental types */
typedef enum {
    RA_TYPE_USER = 0, /* composite type, with optional elemental size
                          given by elbyte. User must handle decoding.
                          Note ras are recursive: a ra can contain
                          another ra */
    RA_TYPE_INT,
    RA_TYPE_UINT,
    RA_TYPE_FLOAT,
    RA_TYPE_COMPLEX
} ra_type;


enum { RA_DIFF_EQ, RA_DIFF_L1, RA_DIFF_L2 };

static const char RA_TYPE_CODES[] = { "siufc" };

#ifdef __cplusplus
extern "C" {
#endif


// Basic functions
ra_t * ra_create(const char *type, const uint64_t ndims, const uint64_t dims[], const uint64_t flags);
int ra_read(ra_t * a, const char *path);
int ra_write(ra_t *a, const char *path);
int ra_copy(ra_t* dst, ra_t* src);
void ra_free(ra_t * a);
void print_magic(const ra_t *r);
ra_t * ra_decompress(ra_t *r);
ra_t * ra_compress(ra_t *r);

int ra_read_header(ra_t *a, const char *path);
void ra_peek(const ra_t *a);
void ra_print_header(const char *path);
uint64_t ra_flags(const char *path);
uint64_t ra_eltype(const char *path);
uint64_t ra_elbyte(const char *path);
uint64_t ra_size(const char *path);
uint64_t ra_ndims(const char *path);
uint64_t *ra_dims(const char *path);
void ra_print_dims(const char *path);
int ra_reshape(ra_t * r, const uint64_t newdims[], const uint64_t ndimsnew);
int ra_diff(const ra_t * a, const ra_t * b, const int diff_type);


#ifdef __cplusplus
}
#endif
#endif                          /* _RA_H */
