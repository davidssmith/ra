
#include <stdio.h>
#include "ra.h"


int
main ()
{
    const char* testfile1 = "../data/test.ra";
    const char* testfile2 = "test2.ra";

    ra_t r;
    ra_read(&r, testfile1);
    ra_write(&r, testfile2);

    ra_query(testfile1);
    ra_query(testfile2);

    ra_t r2;
    ra_read(&r2, testfile2);

    for (size_t k = 0; k < r.size; ++k) {
        printf("%2x%2x ", r.data[k], r2.data[k]);
        assert(r.data[k] == r2.data[k]);
        if ((k + 1) % 20 == 0) printf("\n");
    }
    printf("\nTEST PASSED\n");

    return 0;
}
