#include <stdlib.h>

#include <check.h>

#include "test_bitmap.c"

static Suite* sxbp_suite(void) {
    Suite* s = suite_create("SXBP");

    // Test cases for bitmap data type
    TCase* test_bitmap = tcase_create("Bitmap");
    tcase_add_test(test_bitmap, test_blank_bitmap);
    suite_add_tcase(s, test_bitmap);

    return s;
}

int main(void) {
    int number_failed = -1;
    Suite* s = sxbp_suite();
    SRunner* sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
