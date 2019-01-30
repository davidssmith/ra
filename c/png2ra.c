#include <png.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ra.h"

static void
fatal_error (const char * message, ...)
{
	va_list args;
	va_start (args, message);
	vfprintf (stderr, message, args);
	va_end (args);
	exit (EXIT_FAILURE);
}

ra_t *
png_read (const char *png_file)
{
	png_structp		png_ptr;
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
	int rowbytes;
	rowbytes = png_get_rowbytes (png_ptr, info_ptr);

	uint64_t *dims = (uint64_t*)malloc(3*sizeof(uint64_t));
	dims[0] = width;
	dims[1] = height;
	dims[2] = 3;

    ra_t *r = ra_create("u1", 3, dims);

	for (j = 0; j < height; j++) {
		int i;
		png_bytep row;
		row = rows[j];
		for (i = 0; i < rowbytes; i++) {
			png_byte pixel;
			for (int c = 0; c < 3; ++c)
				r->data[c + 3*i + 3*j*width] = row[i];
		}
	}
	fclose(fp);
	return r;
}


int main(int argc, char *argv[])
{
	char png_file[256], ra_file[256];
	if (argc < 2) {
		printf("%s <pngfile>\n", argv[0]);
		return 1;
	}
	snprintf(png_file, 256, "%s.png", argv[1]);
	snprintf(ra_file, 256, "%s.ra", argv[1]);
	ra_t *r = png_read(png_file);
	printf("dims: %lu %lu %lu\n", r->dims[0], r->dims[1], r->dims[2]);
	printf("read %lu total bytes\n", r->size);
	print_magic(r);
	ra_write(r, ra_file);

	return 0;
}
