
#include <assert.h>
#include <stdio.h>
#include "ra.h"


int
test_rw()
{
    const char *testfile1 = "../data/cifar_airplane.ra";
    const char *testfile2 = "test.ra";

    ra_t r;
    ra_read(&r, testfile1);
    ra_write(&r, testfile2);

    ra_print_header(testfile1);
    ra_print_header(testfile2);

    ra_t r2;
    ra_read(&r2, testfile2);

	assert(ra_diff(&r, &r2, 0) == 0);
    printf("Read/Write TEST PASSED\n");
	ra_free(&r);
	ra_free(&r2);

    return 0;
}

int
test_compress()
{
    const char *testfile1 = "../data/cifar_airplane_z.ra";
    const char *testfile2 = "test.ra";
    const char *testfile3 = "test2.ra";

    ra_t r, r2, rorig;
    ra_read(&r, testfile1);
    ra_read(&rorig, testfile1);
	ra_decompress(&r);
    ra_write(&r, testfile2);
    ra_read(&r2, testfile2);
	ra_compress(&r2);
	ra_write(&r2, testfile3);

    ra_print_header(testfile1);
    ra_print_header(testfile2);
    ra_print_header(testfile3);

	int diff = ra_diff(&rorig, &r2, 0);
	//printf("diff code: %d\n", diff);
	assert (diff == 0);
    printf("Compress TEST PASSED\n");
	ra_free(&r);
	ra_free(&r2);
	ra_free(&rorig);

    return 0;
}


int
main ()
{
	test_rw();
	test_compress();
	return 0;
}
