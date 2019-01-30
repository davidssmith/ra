
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
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hdf5.h"

uint64_t
time_usec(const struct timeval *tv)
{
	    return tv->tv_usec + 1000000*tv->tv_sec;
}

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

static struct timeval begin, end;
size_t total_bytes;

uint64_t
h5smalltestseparate (size_t n, size_t nfiles)
{
	char filename[32];
	float *data = (float *) calloc(n,sizeof(float));
	total_bytes = n * nfiles *sizeof(float);
	gettimeofday(&begin, NULL);
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(filename, "/data/ra/%ld.h5", i);
		h5write(filename, "x", data, n, 1);
	}
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(filename, "/data/ra/%ld.h5", i);
		h5read(filename, "x", data);
	}
	gettimeofday(&end, NULL);
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(filename, "/data/ra/%ld.h5", i);
		unlink(filename);
	}
	uint64_t t = time_usec(&end) - time_usec(&begin);
	//float mb = total_bytes * 1e-6;
	//printf("HDF5     %ld %ldx1 files:       %6.1f ms, %6.1f MBps\n", nfiles, n, 1000*t, mb/t);
	return t;
}

uint64_t
h5smalltestcombined (size_t n, size_t nfiles)
{
	char groupname[32];
	float *data = (float *) calloc(n,sizeof(float));
	total_bytes = n * nfiles *sizeof(float);
   	hid_t       file_id;
   	herr_t      status;

	gettimeofday(&begin, NULL);

     /* Create a new file using default properties. */
   	file_id = H5Fcreate("/data/ra/small.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(groupname, "tmp%ld", i);
		h5writegroup(file_id, groupname, data, n, 1);
	}
	for (size_t i = 0; i < nfiles; ++i) {
		sprintf(groupname, "tmp%ld", i);
		h5readgroup(file_id, groupname, data);
	}
    status = H5Fclose(file_id);
	gettimeofday(&end, NULL);
	unlink("/data/ra/small.h5");
	uint64_t t = time_usec(&end) - time_usec(&begin);
	//float mb = total_bytes * 1e-6;
	//printf("HDF5 %ld %ldx1, %6.1f, ms, %6.1f, MBps\n", nfiles, n, 1000*t, mb/t);
	return t;
}

uint64_t
h5bigtest (size_t n, size_t nfiles)
{
	float *data = (float *) calloc(n*nfiles, sizeof(float));
	total_bytes = n*nfiles*sizeof(float);
	gettimeofday(&begin, NULL);
	h5write("/data/ra/big.h5", "x", data, n,nfiles);
	h5read("/data/ra/big.h5", "x", data);
	gettimeofday(&end, NULL);
	unlink("/data/ra/big.h5");
	uint64_t t = time_usec(&end) - time_usec(&begin);
	//printf("HDF5 1 %ldx%ld, %6.1f, ms, %6.1f, MBps\n", n,nfiles, 1000*t, mb/t);
	return t;
}
 
void
print_stats (const char *name, uint64_t t[], const int navg)
{
	float tavg = 0.f, tmin=1e20, tmax =0;
	for (int i = 0; i < navg; ++i){
		tavg += (float)t[i]*1e-3;
		if (t[i] < tmin) tmin = t[i];
		if (t[i] > tmax) tmax = t[i];
	}
	tavg /= navg;
	tmin *= 1e-3;
	tmax *= 1e-3;
	printf("%s, %7.2f, ms avg of %d, %7.2f, min, %7.2f, max\n",
			name, tavg, navg, tmin, tmax);
}

int
main (int argc, char *argv[])
{
	size_t n = 10;
	size_t nfiles = 100000;
	//float mb = n*nfiles*sizeof(float) * 1e-6;
	char name[32];

    if(argc < 2) {
		fprintf(stderr, "%s <navg>\n", argv[0]);
		return 1;
	}
    int navg = atoi(argv[1]);
	uint64_t *t = malloc(sizeof(uint64_t)*navg);

    for (int i = 0; i < navg; ++i) {
         t[i] = h5smalltestcombined(n, nfiles);
    }
	sprintf(name, "HDF5 %ld %ldx1", nfiles, n);
	print_stats(name, t, navg);

    for (int i = 0; i < navg; ++i) {
         t[i] = h5smalltestcombined(n*10, nfiles/10);
    }
	sprintf(name, "HDF5 %ld %ldx1", nfiles/10, n*10);
	print_stats(name, t, navg);

    for (int i = 0; i < navg; ++i) {
         t[i] = h5bigtest(n, nfiles);
    }
	sprintf(name, "HDF5 1 %ldx%ld", n, nfiles);
	print_stats(name, t, navg);

	free(t);
    return 0;
}

