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

#include "ra.h"

void
ra_query (const char *path)
{
    ra_t a;
    int j, fd;
    uint64_t magic;
    printf("---\nname: %s\n", path);
    fd = open(path, O_RDONLY);
    if (fd == -1)
        err(errno, "unable to open output file for writing");
    read(fd, &magic, sizeof(uint64_t));
    read(fd, &(a.flags), sizeof(uint64_t));
    read(fd, &(a.eltype), sizeof(uint64_t));
    read(fd, &(a.elbyte), sizeof(uint64_t));
    read(fd, &(a.size), sizeof(uint64_t));
    read(fd, &(a.ndims), sizeof(uint64_t));
    printf("endian: %s\n", a.flags  & RA_FLAG_BIG_ENDIAN ? "big" : "little");
    printf("type: %s%lld\n", RA_TYPE_NAMES[a.eltype], a.elbyte*8);
    printf("size: %lld\n", a.size);
    printf("dimension: %lld\n", a.ndims);
    a.dims = (uint64_t*)malloc(a.ndims*sizeof(uint64_t));
    read(fd, a.dims, a.ndims*sizeof(uint64_t));
    printf("shape:\n");
    for (j = 0; j < a.ndims; ++j)
        printf("  - %lld\n", a.dims[j]);
    printf("...\n");
    close(fd);
}

int
ra_read (ra_t *a, const char *path)
{
    int fd;
    uint64_t bytestoread, bytesleft, magic;
    fd = open(path, O_RDONLY);
    if (fd == -1)
        err(errno, "unable to open output file for writing");
    read(fd, &magic, sizeof(uint64_t));
    read(fd, &(a->flags), sizeof(uint64_t));
    read(fd, &(a->eltype), sizeof(uint64_t));
    read(fd, &(a->elbyte), sizeof(uint64_t));
    read(fd, &(a->size), sizeof(uint64_t));
    read(fd, &(a->ndims), sizeof(uint64_t));
    a->dims = (uint64_t*)malloc(a->ndims*sizeof(uint64_t));
    read(fd, a->dims, a->ndims*sizeof(uint64_t));
    a->data = (uint8_t*)malloc(a->size);
    if (a->data == NULL)
        err(errno, "unable to allocate memory for data");
    uint8_t *data_cursor = a->data;

    bytesleft = a->size;
    while (bytesleft > 0) {
        bytestoread = bytesleft < RA_MAX_BYTES ? bytesleft : RA_MAX_BYTES;
        read(fd, data_cursor, bytestoread);
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
    write(fd, &RA_MAGIC_NUMBER, sizeof(uint64_t));
    write(fd, &(a->flags), sizeof(uint64_t));
    write(fd, &(a->eltype), sizeof(uint64_t));
    write(fd, &(a->elbyte), sizeof(uint64_t));
    write(fd, &(a->size), sizeof(uint64_t));
    write(fd, &(a->ndims), sizeof(uint64_t));
    write(fd, a->dims, a->ndims*sizeof(uint64_t));

    bytesleft = a->size;
    data_in_cursor = a->data;

    bufsize = bytesleft < RA_MAX_BYTES ? bytesleft : RA_MAX_BYTES;
    while (bytesleft > 0) {
        write(fd, data_in_cursor, bufsize);
        data_in_cursor += bufsize / sizeof(uint8_t);
        bytesleft -= bufsize;
    }

    close(fd);
    return 0;
}


#ifdef __cplusplus
}
#endif


void
ra_free (ra_t *a)
{
    free(a->dims);
    free(a->data);
}

