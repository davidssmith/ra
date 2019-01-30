#include <png.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

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
	//printf ("Width is %d, height is %d\n", width, height);
	int rowbytes;
	rowbytes = png_get_rowbytes (png_ptr, info_ptr);
	//printf ("Row bytes = %d\n", rowbytes);
#if 0
	for (j = 0; j < height; j++) {
		int i;
		png_bytep row;
		row = rows[j];
		for (i = 0; i < rowbytes; i++) {
			png_byte pixel;
			pixel = row[i];
			if (pixel < 64) {
				printf ("#");
			}
			else if (pixel < 128) {
				printf ("*");
			}
			else if (pixel < 196) {
				printf (".");
			}
			else {
				printf (" ");
			}

		}
		printf ("\n");
	}
#endif
	fclose(fp);
	return rowbytes;
}


int main(int argc, char *argv[])
{
	char png_file[256];
	if (argc < 2) {
		printf("%s <pngfile>\n", argv[0]);
		return 1;
	}
	snprintf(png_file, 256, "%s", argv[1]);
	const int nreps =10000;
	size_t total_rowbytes = 0;
	for (int i = 0; i < nreps; ++i)
		total_rowbytes += png_read(png_file);

	printf("read %lu total bytes\n", total_rowbytes);
	return 0;
}
