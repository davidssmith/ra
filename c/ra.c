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

#include <err.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lz4.h"
#include "ra.h"

// TODO: extend validation checks to internal consistency
// TODO: compressed with LEB128?


// field offsets (bytes)
#define MAGIC_OFFSET   0
#define FLAGS_OFFSET   8
#define ELTYPE_OFFSET 16
#define ELBYTE_OFFSET 24
#define SIZE_OFFSET   32
#define NDIMS_OFFSET  40
#define DIMS_OFFSET   48

enum diff_type {
	DIFF_FLAGS = 0,
	DIFF_ELTYPE,
	DIFF_ELBYTE,
	DIFF_SIZE,
	DIFF_NDIMS,
	DIFF_DIMS,
	DIFF_DATA
};

//static char endianchar[] = {'l', 'b'};

/* flag booleans */
inline static int is_compressed(ra_t *r) { return r->flags & RA_FLAG_COMPRESSED; }
inline static int is_big_endian(ra_t *r) { return r->flags & RA_FLAG_BIG_ENDIAN; }


//
// VALIDATION FUNCTIONS
//

static int
check_magic_and_flags (const ra_t * restrict a)
{
    if (a->magic != RA_MAGIC_NUMBER)
        err(EX_DATAERR, "Invalid magic: %lu\n", a->magic);
    if (a->flags & RA_UNKNOWN_FLAGS) {
        fprintf(stderr, "Warning: This RA file must have been written by a newer version of this\n");
        fprintf(stderr, "code. Correctness of input is not guaranteed. Update your version of the\n");
        fprintf(stderr, "RawArray package to stop this warning.\n");
    }
    return 1;
}

void
print_magic (const ra_t *r)
{
	char *s = (char*)&r->magic;
	for (int i = 0; i < 8; ++i)
		printf("%c", s[i]);
	printf("\n");
}

static size_t
valid_read(int fd, void *buf, const size_t count)
{
    size_t nread = read(fd, buf, count);
    if (nread != count)
        err(EX_IOERR, "Read %lu bytes instead of %lu.\n", nread, count);
    return nread;
}

static size_t
valid_write (int fd, const void * restrict buf, const size_t count)
{
    size_t nwrote = write(fd, buf, count);
    if (nwrote != count)
        err(EX_IOERR, "Wrote %lu bytes instead of %lu.\n", nwrote, count);
    return nwrote;
}

static void *
safe_malloc(const size_t size)
{
	void *data = malloc(size);
	if (data == NULL)
		err(EX_OSERR, "unable to allocate memory for data");
	return data;
}

static void
refresh_mem_from_struct(ra_t *r)
{
	if (r->top != NULL) {  // only do if using unified mem
		*((uint64_t*)(r->top)) = r->magic;
		*((uint64_t*)(r->top + FLAGS_OFFSET)) = r->flags;
		*((uint64_t*)(r->top + ELTYPE_OFFSET)) = r->eltype;
		*((uint64_t*)(r->top + ELBYTE_OFFSET)) = r->elbyte;
		*((uint64_t*)(r->top + SIZE_OFFSET)) =  r->size;
		*((uint64_t*)(r->top + NDIMS_OFFSET)) =  r->ndims;
	}
}


static int
valid_open(const char *path, const int perms)
{
    int fd = open(path, perms, 0644);
    if (fd == -1)
        err(EX_CANTCREAT, "unable to open %s", path);
    return fd;
}


//
// SIMPLE QUERIES
//

static size_t
ra_ondisk_size(int fd)
{
	size_t cur = lseek(fd, 0, SEEK_CUR);
	size_t size = lseek(fd, 0, SEEK_END) - cur; 
	lseek(fd, cur, SEEK_SET);
	return size;
	//struct stat st;
	//stat(path, &st);
	//return st.st_size;
}

inline static size_t
ra_header_size(const ra_t * restrict r)
{
	return DIMS_OFFSET + sizeof(uint64_t)*r->ndims;
}

inline static size_t
ra_file_size(const ra_t * restrict r)
{
	return DIMS_OFFSET + sizeof(uint64_t)*r->ndims + r->size;
}

inline static uint64_t
ra_data_size(const ra_t *restrict r)
{  /* return size of data region based on dimensions and elbyte */
	uint64_t size = r->elbyte;
	for (uint64_t d = 0; d < r->ndims; ++d)
		size *= r->dims[d];
	return size;
}


// 
// WRAPPED IO FUNCTIONS
//

static uint8_t *
chunked_read(int fd)
{
	size_t size = ra_ondisk_size(fd);
	uint8_t *data = safe_malloc(size);
    size_t bytesleft = size;
    uint8_t *cursor = data;
	size_t bufsize = bytesleft < RA_MAX_BYTES ? bytesleft : RA_MAX_BYTES;
    while (bytesleft > 0)
    {
        valid_read(fd, cursor, bufsize);
        cursor += bufsize;
        bytesleft -= bufsize;
    }
	return data;
}

static int
chunked_write(int fd, uint8_t* data, const size_t size)
{
    size_t bytesleft = size;
    uint8_t *cursor = data;
    size_t bufsize = bytesleft < RA_MAX_BYTES ? bytesleft : RA_MAX_BYTES;
    while (bytesleft > 0)
    {
        valid_write(fd, cursor, bufsize);
        cursor += bufsize;
        bytesleft -= bufsize;
    }
	return 0;
}


int
ra_read_header(ra_t *a, const char *path)
{
    int fd = valid_open(path, O_RDONLY);
    valid_read(fd, a, DIMS_OFFSET);
    check_magic_and_flags(a);
    a->dims = (uint64_t *) malloc(a->ndims * sizeof(uint64_t));
	a->top = NULL;
	a->data = NULL;
    valid_read(fd, a->dims, a->ndims * sizeof(uint64_t));
	return fd;
}

void
ra_peek(const ra_t *a)
{
    //ra_t a;
	//int fd = ra_read_header(&a, path);
	//close(fd);
	char typecode[7];
	snprintf(typecode, 7, "%c%lu%c", RA_TYPE_CODES[a->eltype], a->elbyte * 8,
			a->flags & RA_FLAG_COMPRESSED ? 'z' : ' ');
	printf("%6s ", typecode);
    //printf("%ce, ", endianchar[a->flags & RA_FLAG_BIG_ENDIAN]);
    //printf("t%lu, ", a->eltype);
    //printf("%2lu, ", a->elbyte);
    printf("%12lu  ", a->size);
    //printf("%4lu, ", a->ndims);
    printf("(");
    for (int j = 0; j < a->ndims - 1; ++j)
        printf("%lu, ", a->dims[j]);
    printf("%lu)\n", a->dims[a->ndims - 1]);
}

void
ra_print_header(const char * path)
{
    ra_t a;
	int fd = ra_read_header(&a, path);
	printf("%-30s ", path);
	ra_peek(&a);
	close(fd);
	ra_free(&a);
}

uint64_t
ra_get_field(const char *path, const int n)
{
    uint64_t val;
    int fd = valid_open(path, O_RDONLY);
    lseek(fd, n * sizeof(uint64_t), SEEK_CUR);
    valid_read(fd, &val, sizeof(uint64_t));
    close(fd);
    return val;
}

#define MAKE_ACCESSOR(var,num) \
	uint64_t ra_##var(const char *path) { return ra_get_field(path, num); }

MAKE_ACCESSOR(flags,  1);
MAKE_ACCESSOR(eltype, 2);
MAKE_ACCESSOR(elbyte, 3);
MAKE_ACCESSOR(size,   4);
MAKE_ACCESSOR(ndims,  5);


uint64_t *
ra_dims(const char *path)
{
    uint64_t *dims;
    uint64_t ndims;
    int fd = valid_open(path, O_RDONLY);
    lseek(fd, 5 * sizeof(uint64_t), SEEK_CUR);
    valid_read(fd, &ndims, sizeof(uint64_t));
    dims = (uint64_t *) malloc(ndims * sizeof(uint64_t));
    valid_read(fd, dims, ndims * sizeof(uint64_t));
    return dims;
}

void
ra_parse_type(const char *typestr, uint64_t *eltype, uint64_t *elbyte)
{
	switch(typestr[0]) {
	case 's':
		*eltype = RA_TYPE_USER;
		break;
	case 'i':
		*eltype = RA_TYPE_INT;
		break;
	case 'u':
		*eltype = RA_TYPE_UINT;
		break;
	case 'f':
		*eltype = RA_TYPE_FLOAT;
		break;
	case 'c':
		*eltype = RA_TYPE_COMPLEX;
		break;
	default:
		err(EX_USAGE, "Unknown type code %c", typestr[0]);
	}
	*elbyte = atoi(typestr+1);
}


ra_t *
ra_create(const char *type, const uint64_t ndims,
		const uint64_t dims[], const uint64_t flags)
{
	ra_t *r = malloc(sizeof(ra_t));
	r->magic = RA_MAGIC_NUMBER;
	r->flags = flags;
	ra_parse_type(type, &(r->eltype), &r->elbyte);
	r->ndims = ndims;
	r->size = r->elbyte;
	for (uint64_t i = 0; i < ndims; ++i)
		r->size *= dims[i];
	r->top = (uint8_t*) malloc(ra_file_size(r));
	refresh_mem_from_struct(r);
	r->dims = (uint64_t*)(r->top + DIMS_OFFSET);
	for (int i = 0; i < ndims; ++i)
		r->dims[i] = dims[i];
	r->data = (uint8_t*)(r->top +  ra_header_size(r));
	return r;
}

int
ra_read(ra_t *a, const char *path)
{
    int fd = valid_open(path, O_RDONLY);
	a->top = chunked_read(fd);
	close(fd);
	memcpy(a, a->top, DIMS_OFFSET); // fixed part of struct
	a->dims = (uint64_t*)(a->top + DIMS_OFFSET);
	a->data = a->top + DIMS_OFFSET + sizeof(uint64_t)*a->ndims;
    return 0;
}

int
ra_write(ra_t *a, const char *path)
{
    int fd;
    fd = valid_open(path, O_WRONLY | O_TRUNC | O_CREAT); //0644
	if (a->top == NULL) // don't have a single malloc-ed space for the raw array
	{
		valid_write(fd, a, DIMS_OFFSET);  // write in parts
		valid_write(fd, a->dims, a->ndims * sizeof(uint64_t));
		chunked_write(fd, a->data, a->size);
	}
	else 
	{
		refresh_mem_from_struct(a);  // make sure malloc memory contains updated struct vars
		chunked_write(fd, a->top, ra_file_size(a));  // can write all at once
	}
    close(fd);
    return 0;
}


int
ra_copy (ra_t *dst, ra_t *src)
{
	if (src->top == NULL) {
		memcpy(dst, src, DIMS_OFFSET);
		memcpy(dst->dims, src->dims, src->ndims*sizeof(uint64_t));
		memcpy(dst->data, src->data, src->size);
	} else {
		refresh_mem_from_struct(src);
		memcpy(dst->top, src->top, ra_file_size(src));
	}
	return 0;
}

ra_t *
ra_compress(ra_t *r)
{
	if (is_compressed(r))  // already compressed
		return r;
	size_t maxoutsize = LZ4_compressBound(r->size);
	//printf("Uncompressed size: %lu\n", r->size);
	//printf("INferred maxoutsize: %lu\n", maxoutsize);
	char *compressed_data = safe_malloc(maxoutsize);
	size_t outsize = LZ4_compress_default((char*)r->data, compressed_data, r->size, maxoutsize);
	//printf("Actual outsize: %lu\n", outsize);
	if (outsize <= 0)
		err(EX_DATAERR, "LZ4 compression failed, size=%lu", r->size);
	if (r->top == NULL) {
		free(r->data);
		r->data = (uint8_t*)compressed_data;
	} else {
		memcpy(r->data, compressed_data, outsize);
		free(compressed_data);
	}
	r->flags |= RA_FLAG_COMPRESSED;
	r->size = outsize;
	return r;
}


ra_t *
ra_decompress(ra_t *r)
{
	if (!is_compressed(r)) // only do if compressed
		return r;
	size_t orig_size = ra_data_size(r);
	//printf("compressed_size: %lu\n", r->size);
	//printf("orig_size: %lu\n", orig_size);
	char *decompressed_data = safe_malloc(orig_size);
	size_t decompressed_size = LZ4_decompress_safe((char*)r->data, decompressed_data, r->size, orig_size);
	//printf("decompressed_size: %lu\n", decompressed_size);
	if (decompressed_size <= 0 || decompressed_size != orig_size)
		err(EX_DATAERR, "LZ4 decompression failed on data size %lu", r->size);
	if (r->top == NULL) {
		free(r->data);
		r->data = (uint8_t*)decompressed_data;
	} else {
		// for most cases, fastest is to redo dims and de-unify
		r->dims = safe_malloc(r->ndims*sizeof(uint64_t));
		memcpy(r->dims, r->top + DIMS_OFFSET, r->ndims*sizeof(uint64_t));
		r->data = (uint8_t*)decompressed_data;
		free(r->top);
		r->top = NULL;
	}
	r->flags ^= RA_FLAG_COMPRESSED;  // turn off compression flag
	r->size = orig_size;
	refresh_mem_from_struct(r);
	return r;
}

void
ra_free(ra_t * a)
{
	if (a->top == NULL) {
		free(a->dims);
		free(a->data);
	} else
    	free(a->top);

}

int
ra_reshape(ra_t * r, const uint64_t newdims[], const uint64_t ndimsnew)
{
    uint64_t newsize = 1;
    for (uint64_t k = 0; k < ndimsnew; ++k)
        newsize *= newdims[k];
    if (r->size != newsize * r->elbyte)
		err(EX_DATAERR, "Total number of elements must be conserved.");
    // if new dims preserve total number of elements, then change the dims
    r->ndims = ndimsnew;
    size_t newdimsize = ndimsnew * sizeof(uint64_t);
    free(r->dims);
    r->dims = (uint64_t *) malloc(newdimsize);
    memcpy(r->dims, newdims, newdimsize);
    return 0;
}

int
ra_diff(const ra_t * a, const ra_t * b, const int diff_type)
{
    if (a->flags != b->flags)
        return DIFF_FLAGS;
    if (a->eltype != b->eltype)
        return DIFF_ELTYPE;
    if (a->elbyte != b->elbyte)
        return DIFF_ELBYTE;
    if (a->size != b->size)
        return DIFF_SIZE;
    if (a->ndims != b->ndims)
        return DIFF_NDIMS;
    for (size_t i = 0; i < a->ndims; ++i)
        if (a->dims[i] != b->dims[i])
            return DIFF_DIMS;
    if (diff_type == 0)
    {
        for (size_t i = 0; i < a->size; ++i)
        {
            if (a->data[i] != b->data[i])
            {
                printf("differ at position %ld: lhs=%u rhs=%u\n", i,
                    a->data[i], b->data[i]);
                return DIFF_DATA;
            }
        }
    }
    else if (diff_type == 1)
    {
        double norm = 0.0;
        for (size_t i = 0; i < a->size; ++i)
            norm += fabs(a->data[i] - b->data[i]);
        norm = sqrtf(norm);
        printf("L1 distance: %g\n", norm);
        if (norm > 0.)
            return DIFF_DATA;
    }
    else if (diff_type == 2)
    {
        double t, norm = 0.0;
        for (size_t i = 0; i < a->size; ++i)
        {
            t = (a->data[i] - b->data[i]);
            norm += t * t;
        }
        norm = sqrtf(norm);
        printf("L2 distance: %g\n", norm);
        if (norm > 0.)
            return DIFF_DATA;
    }
    else
        err(EX_USAGE, "Unknown diff_type %d\n", diff_type);
    return 0;
}
