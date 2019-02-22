/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This program is the main entry point for the unit tests of sxbp.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <stdlib.h>
#include <time.h>

#include "check_wrapper.h"

#include "test_suites.h"


int main(void) {
    int number_failed = -1;
    SRunner* suite_runner = srunner_create(make_internal_suite());
    srunner_add_suite(suite_runner, make_bitmap_suite());
    srunner_add_suite(suite_runner, make_buffer_suite());
    srunner_add_suite(suite_runner, make_figure_suite());

    /*
     * to add variety to the 'randomness' of tests which use rand(), seed the
     * mediocre random number generator
     */
    srand((unsigned int)time(NULL));

    srunner_run_all(suite_runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(suite_runner);
    srunner_free(suite_runner);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
