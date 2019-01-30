#include <stdlib.h>

#include "check_wrapper.h"

#include "test_suites.h"


int main(void) {
    int number_failed = -1;
    SRunner* suite_runner = srunner_create(make_bitmap_suite());
    srunner_add_suite(suite_runner, make_buffer_suite());
    srunner_add_suite(suite_runner, make_figure_suite());

    srunner_run_all(suite_runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(suite_runner);
    srunner_free(suite_runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
