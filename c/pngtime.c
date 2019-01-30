#include <png.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include "ra.h"

//static clock_t begin, end;
size_t total_bytes;
struct timeval begin, end;

uint64_t
time_usec(const struct timeval *tv)
{
    return tv->tv_usec + 1000000*tv->tv_sec;
}

static void
fatal_error (const char * message, ...)
{
	va_list args;
	va_start (args, message);
	vfprintf (stderr, message, args);
	va_end (args);
	exit (EXIT_FAILURE);
}

int png_read (const char *png_file)
{
	png_structp	png_ptr;
	png_infop info_ptr;
	FILE * fp;
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	int interlace_method;
	int compression_method;
	int filter_method;
	int j;
	png_bytepp rows;
	fp = fopen (png_file, "rb");
	if (! fp) {
		fatal_error ("Cannot open '%s': %s\n", png_file, strerror (errno));
	}
	png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (! png_ptr) {
		fatal_error ("Cannot create PNG read structure");
	}
	info_ptr = png_create_info_struct (png_ptr);
	if (! png_ptr) {
		fatal_error ("Cannot create PNG info structure");
	}
	png_init_io (png_ptr, fp);
	png_read_png (png_ptr, info_ptr, 0, 0);
	png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth,
				  &color_type, &interlace_method, &compression_method,
				  &filter_method);
	//printf("Color type: %d  bit depth: %d\n", color_type, bit_depth);
	rows = png_get_rows (png_ptr, info_ptr);
	//printf ("Width is %d, height is %d\n", width, height);
	int rowbytes;
	rowbytes = png_get_rowbytes (png_ptr, info_ptr);
	//printf ("Row bytes = %d\n", rowbytes);
	for (j = 0; j < height; j++) {
		int i;
		png_bytep row;
		row = rows[j];
		for (i = 0; i < rowbytes; i++) {
			png_byte pixel;
			pixel = row[i];
		}
	}
	fclose(fp);
	return width*height*3;
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
main(int argc, char *argv[])
{
	char png_file[256], ra_file[256];
	if (argc < 2) {
		printf("%s <basename>\n", argv[0]);
		return 1;
	}
	snprintf(png_file, 256, "%s.png", argv[1]);
	snprintf(ra_file, 256, "%s.ra", argv[1]);
	const int navg =10, nimg = 50000;
	uint64_t *t = (uint64_t*)malloc(navg*sizeof(uint64_t));
	size_t total_rowbytes = 0;
	for (int i = 0; i < navg; ++i) {
    	gettimeofday(&begin,NULL);
		for (int j = 0; j < nimg; ++j) {
			total_rowbytes += png_read(png_file);
		}
    	gettimeofday(&end,NULL);
    	t[i] = time_usec(&end) - time_usec(&begin);
	}
	print_stats(png_file, t, navg);
	//printf("%s, PNG, %6.2f us/img, %lu bytes, %6.2f ns/MB\n", argv[1], t, total_rowbytes, 1e9*t/total_rowbytes);
	ra_t r;
	total_rowbytes = 0;
	for (int i = 0; i < navg; ++i) {
    	gettimeofday(&begin,NULL);
		for (int j = 0; j < nimg; ++j) {
			ra_read(&r, ra_file); 
			total_rowbytes += r.size;
			ra_free(&r);
		}
    	gettimeofday(&end,NULL);
    	t[i] = time_usec(&end) - time_usec(&begin);
	}
	print_stats(ra_file, t, navg);
	//printf("%s, RawArray, %6.2f us/img, %lu bytes, %6.2f ns/MB\n", argv[1], t, total_rowbytes, 1e9*t/total_rowbytes);
	return 0;
}
