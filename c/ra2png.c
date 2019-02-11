#include <assert.h>
#include <getopt.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <strings.h>

#include "ra.h"
    
/* A coloured pixel. */

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}
pixel_t;

/* A picture. */
    
typedef struct
{
    pixel_t *pixels;
    size_t width;
    size_t height;
}
bitmap_t;
    
/* Given "bitmap", this returns the pixel of bitmap at the point 
   ("x", "y"). */

static pixel_t * 
pixel_at (bitmap_t * bitmap, int x, int y)
{
    return bitmap->pixels + bitmap->width * y + x;
}
    
/* Write "bitmap" to a PNG file specified by "path"; returns 0 on
   success, non-zero on error. */

static int 
save_png_to_file (bitmap_t *bitmap, const char *path)
{
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int pixel_size = 3;
    int depth = 8;
    
    fp = fopen (path, "wb");
    if (! fp)
        goto fopen_failed;

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
        goto png_create_write_struct_failed;
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL)
        goto png_create_info_struct_failed;
    
    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr)))
        goto png_failure;
    
    /* Set image attributes. */

    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    /* Initialize rows of PNG. */

    row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            pixel_t * pixel = pixel_at (bitmap, x, y);
            *row++ = pixel->red;
            *row++ = pixel->green;
            *row++ = pixel->blue;
        }
    }
    
    /* Write the image data to "fp". */
    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;
    
    for (y = 0; y < bitmap->height; y++)
        png_free(png_ptr, row_pointers[y]);
    png_free(png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    fclose (fp);
 fopen_failed:
    return status;
}

/* Given "value" and "max", the maximum value which we expect "value"
   to take, this returns an integer between 0 and 255 proportional to
   "value" divided by "max". */

static int 
as_pix(float value, float max)
{
    if (value < 0)
        return 0;
    return (int) (256.0 *(value/max));
}

void
print_usage()
{
    fprintf(stderr, "Usage: ra2png [-g] [-h] <file.ra>\n");
    fprintf(stderr, "\t-g\t\t grayscale (default is RGB)\n");
    fprintf(stderr, "\t-h\t\t help\n");
}


int 
main (int argc, char *argv[])
{
    bitmap_t image;
    int x, y, c;
    int status = 0;
	int grayscale = 0;
	char rafilename[256], pngfilename[256];

	if (argc < 2) {
		print_usage();
		return 1;
	}

    while ((c = getopt(argc, argv, "gh")) != -1)
    {
        switch (c) {
        case 'g':
            grayscale = 1;
            break;
        case 'h':
        default:
            print_usage();
            return 1;
        }
    }
    argc -= optind;
    argv += optind;

	ra_t r;
	ra_read(&r, *argv);
	assert(r.ndims >= 2); 

	char *ext_ptr = rindex(*argv, '.');
	*ext_ptr = '\0';
	snprintf(rafilename, sizeof rafilename, "%s.ra", *argv);
	snprintf(pngfilename, sizeof pngfilename, "%s.png", *argv);

	printf("%s -> %s\n", rafilename, pngfilename);

    /* Create an image. */
    image.width = (int)r.dims[0];
    image.height =(int)r.dims[1]; 
    image.pixels = calloc(image.width * image.height, sizeof (pixel_t));
    if (!image.pixels)
		return -1;

    for (y = 0; y < image.height; y++)
        for (x = 0; x < image.width; x++) 
		{
            pixel_t *pixel = pixel_at(&image, x, y);
			//y*width + x
			int pix = as_pix(*((float*)r.data+x + y*image.width), 1.0f);
			printf("pix=%d\n", pix);
			pixel->red = pix;
			pixel->green = pix;
			pixel->blue = pix;
        }

    /* Write the image to a file 'image.png'. */
    if (save_png_to_file(&image, pngfilename)) {
		fprintf(stderr, "Error writing file.\n");
		status = -1;
    }

    free(image.pixels);

    return status;
}
