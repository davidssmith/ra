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
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>


#include "ra.h"


void
validate_magic (const uint64_t magic)
{
   if (magic != RA_MAGIC_NUMBER) {
        fprintf(stderr, "Invalid RA file.\n");
        exit(EX_DATAERR);
   }
}

void
valid_read(int fd, void *buf, const size_t count)
{
    size_t nread = read(fd, buf, count);
    if (nread != count) {
        fprintf(stderr, "Read %lu B instead of %lu B.\n", nread, count);
        exit(EX_IOERR);
    }
}


void
valid_write(int fd, const void *buf, const size_t count)
{
    size_t nwrote = write(fd, buf, count);
    if (nwrote != count) {
        fprintf(stderr, "Wrote %lu B instead of %lu B.\n", nwrote, count);
        exit(EX_IOERR);
    }
}


// TODO: extend validation checks to internal consistency

void
ra_query (const char *path)
{
    ra_t a;
    int j, fd;
    uint64_t magic;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        err(errno, "unable to open output file for writing");
    valid_read(fd, &magic, sizeof(uint64_t));
    validate_magic(magic);
    valid_read(fd, &(a.flags), sizeof(uint64_t));
    valid_read(fd, &(a.eltype), sizeof(uint64_t));
    valid_read(fd, &(a.elbyte), sizeof(uint64_t));
    valid_read(fd, &(a.size), sizeof(uint64_t));
    valid_read(fd, &(a.ndims), sizeof(uint64_t));
    printf("---\nname: %s\n", path);
    printf("endian: %s\n", a.flags  & RA_FLAG_BIG_ENDIAN ? "big" : "little");
    printf("type: %c%u\n", RA_TYPE_CODES[a.eltype], a.elbyte*8);
    printf("eltype: %u\n", a.eltype);
    printf("elbyte: %u\n", a.elbyte);
    printf("size: %u\n", a.size);
    printf("dimension: %u\n", a.ndims);
    a.dims = (uint64_t*)malloc(a.ndims*sizeof(uint64_t));
    valid_read(fd, a.dims, a.ndims*sizeof(uint64_t));
    printf("shape:\n");
    for (j = 0; j < a.ndims; ++j)
        printf("  - %lu\n", a.dims[j]);
    printf("...\n");
    close(fd);
    free(a.dims);
}


void
ra_dims (const char *path)
{
    ra_t a;
    int fd;
    uint64_t magic;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        err(errno, "unable to open output file for writing");
    valid_read(fd, &magic, sizeof(uint64_t));
    validate_magic(magic);
    lseek(fd, 4*sizeof(uint64_t), SEEK_CUR);
    valid_read(fd, &(a.ndims), sizeof(uint64_t));
    a.dims = (uint64_t*)malloc(a.ndims*sizeof(uint64_t));
    valid_read(fd, a.dims, a.ndims*sizeof(uint64_t));
    for (uint64_t i = 0; i < a.ndims; ++i)
        printf("%lu ", a.dims[i]);
    printf("\n");
}


int
ra_read (ra_t *a, const char *path)
{
    int fd;
    uint64_t bytestoread, bytesleft, magic;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        err(errno, "unable to open output file for writing");
    valid_read(fd, &magic, sizeof(uint64_t));
    validate_magic(magic);
    valid_read(fd, &(a->flags), sizeof(uint64_t));
    if ((a->flags & RA_UNKNOWN_FLAGS) != 0) {
        fprintf(stderr, "Warning: This RA file must have been written by a newer version of this\n");
        fprintf(stderr, "code. Correctness of input is not guaranteed. Update your version of the\n");
        fprintf(stderr, "RawArray package to stop this warning.\n");
    }
    valid_read(fd, &(a->eltype), sizeof(uint64_t));
    valid_read(fd, &(a->elbyte), sizeof(uint64_t));
    valid_read(fd, &(a->size), sizeof(uint64_t));
    valid_read(fd, &(a->ndims), sizeof(uint64_t));
    a->dims = (uint64_t*)malloc(a->ndims*sizeof(uint64_t));
    valid_read(fd, a->dims, a->ndims*sizeof(uint64_t));
    bytesleft = a->size;
    // if (a->flags & RA_FLAG_COMPRESSED)
    //     bytesleft += 16;
    a->data = (uint8_t*)malloc(bytesleft);
    if (a->data == NULL)
        err(errno, "unable to allocate memory for data");
    uint8_t *data_cursor = a->data;
    while (bytesleft > 0) {
        bytestoread = bytesleft < RA_MAX_BYTES ? bytesleft : RA_MAX_BYTES;
        valid_read(fd, data_cursor, bytestoread);
        data_cursor += bytestoread;
        bytesleft -= bytestoread;
    }
    close(fd);
    return 0;
}



int
ra_write (ra_t *a, const char *path)
{
    int fd;
    uint64_t bytesleft, bufsize;
    uint8_t *data_in_cursor;
    fd = open(path, O_WRONLY|O_TRUNC|O_CREAT,0644);
    if (fd == -1)
        err(errno, "unable to open output file for writing");
    /* write the easy stuff */
    valid_write(fd, &RA_MAGIC_NUMBER, sizeof(uint64_t));
    valid_write(fd, &(a->flags), sizeof(uint64_t));
    valid_write(fd, &(a->eltype), sizeof(uint64_t));
    valid_write(fd, &(a->elbyte), sizeof(uint64_t));
    valid_write(fd, &(a->size), sizeof(uint64_t));
    valid_write(fd, &(a->ndims), sizeof(uint64_t));
    valid_write(fd, a->dims, a->ndims*sizeof(uint64_t));

    bytesleft = a->size;
    // if (a->flags & RA_FLAG_COMPRESSED) bytesleft += 16;
    data_in_cursor = a->data;

    bufsize = bytesleft < RA_MAX_BYTES ? bytesleft : RA_MAX_BYTES;
    while (bytesleft > 0) {
        valid_write(fd, data_in_cursor, bufsize);
        data_in_cursor += bufsize / sizeof(uint8_t);
        bytesleft -= bufsize;
    }

    close(fd);
    return 0;
}


void
ra_free (ra_t *a)
{
    free(a->dims);
    free(a->data);
}


int
ra_reshape (ra_t *r, const uint64_t newdims[], const uint64_t ndimsnew)
{
    uint64_t newsize = 1;
    for (uint64_t k = 0; k < ndimsnew; ++k)
            newsize *= newdims[k];
    assert(r->size == newsize*r->elbyte);
    // if new dims preserve total number of elements, then change the dims
    r->ndims = ndimsnew;
    size_t newdimsize = ndimsnew*sizeof(uint64_t);
    free(r->dims);
    r->dims = (uint64_t*)malloc(newdimsize);
    memcpy(r->dims, newdims, newdimsize);
    return 0;
}


/*
union {
    double f;
    int64_t i;
    uint64_t u;
} t8;

union {
    float f;
    int32_t i;
    uint32_t u;
} t4;

union {
    float16 f;
    int16_t i;
    uint16_t u;
} t2;

union {
    int8_t i;
    uint8_t u;
    char c;
} t1;

*/


uint64_t
calc_min_elbyte_int (const int64_t max, const int64_t min)
{
    //printf("min: %d, max: %d\n", min, max);
    int minbits_reqd = log(max)/log(2);
    //printf("minbits_reqd: %d\n", minbits_reqd);
    uint64_t m = 8;
    while (m < minbits_reqd)
        m *= 2;
    return m / 8;
}

uint64_t
calc_min_elbyte_float (const double max, const double min)
{
    //printf("min: %g, max: %g\n", min, max);
    double dynamic_range = fabs(min/max);
    //printf("dynamic_range: %g\n", dynamic_range);
    // if (minbits < 16)
    //     return 2;
    // else if (minbits < 32)
    //     return 4;
    // else
    return 8;
}


// TODO: compressed with LEB128?

int
ra_diff (const ra_t *a, const ra_t *b, const int diff_type)
{
    if (a->flags  != b->flags)  return 1;
    if (a->eltype != b->eltype) return 2;
    if (a->elbyte != b->elbyte) return 3;
    if (a->size   != b->size)   return 4;
    if (a->ndims  != b->ndims)  return 5;
    for (size_t i = 0; i < a->ndims; ++i)
        if (a->dims[i] != b->dims[i]) return 6;
    if (diff_type == 0) {
        for (size_t i = 0; i < a->size; ++i) {
            if (a->data[i] != b->data[i]) {
                printf("differ at position %ld: lhs=%u rhs=%u\n", i, a->data[i], b->data[i]);
                return 7;
            }
        }
    } else if (diff_type == 1) {
        double norm = 0.0;
        for (size_t i = 0; i < a->size; ++i) {
            norm += fabs(a->data[i] - b->data[i]);
        }
        norm = sqrtf(norm);
        printf("L1 distance: %g\n", norm);
        if (norm > 0.) return 7;
    } else if (diff_type == 2) {
        double t, norm = 0.0;
        for (size_t i = 0; i < a->size; ++i) {
            t = (a->data[i] - b->data[i]);
            norm += t*t;
        }
        norm = sqrtf(norm);
        printf("L2 distance: %g\n", norm);
        if (norm > 0.) return 7;
    } else
        error("Unknown diff_type %d\n", diff_type);
    return 0;
}
