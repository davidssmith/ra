
/* Converts RA to Berkeley Advanced Reconstruction Toolbox CFL */

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
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <unistd.h>
#include "ra.h"

#define NAME_MAX 256

int
cfl_write (ra_t *a, char* filename)
{
  if (a->eltype != 4) {
    printf("Can only convert RA files containing complex floats.\n");
    exit(EX_DATAERR);
  }
  if (a->elbyte != 8)
    fprintf(stderr, "Warning: converting double to single will lose precision.\n");
  char path[NAME_MAX];
  snprintf(path, NAME_MAX, "%s.cfl", filename);
  int fd = open(path, O_WRONLY|O_TRUNC|O_CREAT,0644);
  if (fd == -1)
      err(errno, "unable to open %s for writing", path);

  uint64_t bytesleft = a->size;
  uint8_t *data_in_cursor = a->data;

  uint64_t bufsize = bytesleft < RA_MAX_BYTES ? bytesleft : RA_MAX_BYTES;
  while (bytesleft > 0) {
      assert(write(fd, data_in_cursor, bufsize) == bufsize);
      data_in_cursor += bufsize / sizeof(uint8_t);
      bytesleft -= bufsize;
  }
  snprintf(path, NAME_MAX, "%s.hdr", filename);
  FILE *fp = fopen(path, "w");
  if (fp == NULL)
      err(errno, "unable to open %s for writing", path);

  for (int k = 0; k < a->ndims; ++k)
    fprintf(fp, "%lu ", a->dims[k]);
  fprintf(fp, "0\n");
  fclose(fp);
  return EX_OK;

}

int
main (int argc, char *argv[])
{
    ra_t a;
    if (argc < 3) {
      printf("Convert an ra file to a cfl format.\n");
      printf("Usage: ra2cfl <rafile> <cflfile>\n");
      exit(EX_USAGE);
    }
    ra_read(&a, argv[1]);
    cfl_write(&a, argv[2]);
    ra_free(&a);
    return EX_OK;
}
