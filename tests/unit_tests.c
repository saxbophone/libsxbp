#include <stdlib.h>

#include <check.h>

#include "test_suites.h"


int main(void) {
    int number_failed = -1;
    SRunner* sr = srunner_create(make_bitmap_suite());
    srunner_add_suite(sr, make_buffer_suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
