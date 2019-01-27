
#include <assert.h>
#include <stdio.h>
#include "ra.h"


int
main()
{
    const char *testfile1 = "../data/test.ra";
    const char *testfile2 = "test2.ra";

    ra_t r;
    ra_read_all(&r, testfile1);
    ra_write_all(&r, testfile2);

    ra_print_header(testfile1);
    ra_print_header(testfile2);

    ra_t r2;
    ra_read_all(&r2, testfile2);

	assert(ra_diff(&r, &r2, 0) == 0);
    printf("\nTEST PASSED\n");
	ra_free_all(&r);
	ra_free_all(&r2);

    return 0;
}
