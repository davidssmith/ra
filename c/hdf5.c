
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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hdf5.h"

int 
h5write (const char* filename, const char *dataset, float *data, size_t m, size_t n) 
{

   hid_t       file_id, dataset_id, dataspace_id;  /* identifiers */
   herr_t      status;
   hsize_t     dims[2];

   /* Create a new file using default properties. */
   file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

   /* Create the data space for the dataset. */
   dims[0] = m; 
   dims[1] = n; 
   dataspace_id = H5Screate_simple(2, dims, NULL);

   /* Create the dataset. */
   dataset_id = H5Dcreate2(file_id, dataset, H5T_NATIVE_FLOAT, dataspace_id, 
					                             H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

   /* Write the dataset. */
   status = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

   /* Close the dataset. */
   status = H5Dclose(dataset_id);

   /* Close the file. */
   status = H5Fclose(file_id);
}

int 
h5writegroup (hid_t file_id, const char *dataset, float *data, size_t m, size_t n) 
{

   hid_t       dataset_id, dataspace_id;  /* identifiers */
   herr_t      status;
   hsize_t     dims[2];

   /* Create the data space for the dataset. */
   dims[0] = m; 
   dims[1] = n; 
   dataspace_id = H5Screate_simple(2, dims, NULL);

   /* Create the dataset. */
   dataset_id = H5Dcreate2(file_id, dataset, H5T_NATIVE_FLOAT, dataspace_id, 
					                             H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

   /* Write the dataset. */
   status = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

   /* Close the dataset. */
   status = H5Dclose(dataset_id);

}

void
h5read (const char *filename, const char *dataset, float *data)
{

   hid_t       file_id, dataset_id, dataspace_id;  /* identifiers */
   herr_t      status;

   /* Open an existing file. */
   file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);

   /* Open an existing dataset. */
   dataset_id = H5Dopen2(file_id, dataset, H5P_DEFAULT);


   status = H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

   /* Close the dataset. */
   status = H5Dclose(dataset_id);

   /* Close the file. */
   status = H5Fclose(file_id);

}


void
h5readgroup (hid_t file_id, const char *dataset, float *data)
{

   hid_t       dataset_id, dataspace_id;  /* identifiers */
   herr_t      status;

   /* Open an existing dataset. */
   dataset_id = H5Dopen2(file_id, dataset, H5P_DEFAULT);


   status = H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

   /* Close the dataset. */
   status = H5Dclose(dataset_id);

}

static clock_t begin, end;
size_t total_bytes;

void
h5smalltestseparate (size_t n, size_t nfiles)
{
	char filename[32];
	float *data = (float *) calloc(n,sizeof(float));
	total_bytes = n * nfiles *sizeof(float);
	begin = clock();
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(filename, "tmp/%ld.h5", i);
		h5write(filename, "x", data, n, 1);
	}
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(filename, "tmp/%ld.h5", i);
		h5read(filename, "x", data);
	}
	end = clock();
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(filename, "tmp/%ld.h5", i);
		unlink(filename);
	}
	float t = (double)(end - begin) / (double)CLOCKS_PER_SEC;
	float mb = total_bytes * 1e-6;
	printf("HDF5     %ld %ldx1 files:       %6.1f ms, %6.1f MBps\n", nfiles, n, 1000*t, mb/t);
}

void
h5smalltestcombined (size_t n, size_t nfiles)
{
	char groupname[32];
	float *data = (float *) calloc(n,sizeof(float));
	total_bytes = n * nfiles *sizeof(float);
	begin = clock();
   	hid_t       file_id;
   	herr_t      status;

     /* Create a new file using default properties. */
   	file_id = H5Fcreate("tmp/small.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(groupname, "tmp%ld", i);
		h5writegroup(file_id, groupname, data, n, 1);
	}
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(groupname, "tmp%ld", i);
		h5readgroup(file_id, groupname, data);
	}
    status = H5Fclose(file_id);
	end = clock();
	unlink("tmp/small.h5");
	float t = (double)(end - begin) / (double)CLOCKS_PER_SEC;
	float mb = total_bytes * 1e-6;
	printf("HDF5     %ld %ldx1 datasets:    %6.1f ms, %6.1f MBps\n", nfiles, n, 1000*t, mb/t);
}

void
h5bigtest (size_t n, size_t nfiles)
{
	float *data = (float *) calloc(n*nfiles, sizeof(float));
	total_bytes = n*nfiles*sizeof(float);
	begin = clock();
	h5write("tmp/big.h5", "x", data, n,nfiles);
	h5read("tmp/big.h5", "x", data);
	end = clock();
	unlink("tmp/big.h5");
	float t = (double)(end - begin) / (double)CLOCKS_PER_SEC;
	float mb = total_bytes * 1e-6;
	printf("HDF5     1 %ldx%ld dataset:     %6.1f ms, %6.1f MBps\n", n,nfiles, 1000*t, mb/t);
}


int
main (int argc, char *argv[])
{
	size_t n = 100;
	size_t nfiles = 10000;

	//h5smalltestseparate(n, nfiles);
	h5smalltestcombined(n, nfiles);
	h5smalltestcombined(n*10, nfiles/10);
	h5bigtest(n, nfiles);

    return 0;
}

