
/* Converts Berkeley Advanced Reconstruction Toolbox CFL to RA */

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

static const size_t NAME_MAX = 256UL;
static const size_t LINE_MAX = 256UL;

int
cfl_read (ra_t *a, char* filename)
{
  char path[NAME_MAX];
  char *line = NULL;
  snprintf(path, NAME_MAX, "%s.hdr", filename);
  FILE *fp = fopen(path, "r");
  if (fp == NULL)
      err(errno, "unable to open %s for reading", path);

  if (getline(&line, &LINE_MAX, fp) != -1) {
      a->ndims = 0;
      for (int c = 0; c != '\n' && c != '\0'; ++c)
        if (line[c] == ' ')
          a->ndims++;
  } else {
    err(errno, "unable to parse first line of %s", path);
  }
  fclose(fp);

  a->ndims--;  // to account for trailing 0 dimension
  a->dims = (uint64_t*)malloc(a->ndims*sizeof(uint64_t));
  a->flags = 0;
  a->eltype = RA_TYPE_COMPLEX;
  a->elbyte = 8;
  a->size = a->elbyte;
  int i;
  for (int k = 0; k < a->ndims; ++k) {
    sscanf(line, "%d", &i);
    a->dims[k] = i;
    a->size *= a->dims[k];
  }
  snprintf(path, NAME_MAX, "%s.cfl", filename);
  int fd = open(path, O_RDONLY);
  if (fd == -1)
      err(errno, "unable to open %s for writing", path);
  a->data = malloc(a->size);
  assert(read(fd, a->data, a->size) == a->size);
  close(fd);

  return EX_OK;
}

int
main (int argc, char *argv[])
{
    ra_t a;
    if (argc < 3) {
      printf("Convert a cfl file to ra format.\n");
      printf("Usage: cfl2ra <cflfile> <rafile>\n");
      exit(EX_USAGE);
    }
    cfl_read(&a, argv[1]);
    ra_write(&a, argv[2]);
    ra_free(&a);
    return EX_OK;
}
