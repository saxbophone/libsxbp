/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides unit tests for some functionality of the
 * sxbp_internal compilation unit.
 * NOTE: These unit tests are not testing the public spec, but exist because it
 * is very important to verify that some of this private functionality works
 * correctly.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <signal.h>

#include "check_wrapper.h"

#include "../sxbp/sxbp_internal.h"

#include "test_suites.h"


START_TEST(test_unreachable_code_abort_macro) {
    // calling this should trigger an abort (with NDEBUG set)
    SXBP_ABORT_UNREACHABLE_CODE();
} END_TEST

Suite* make_internal_suite(void) {
    // Test cases for sxbp_internal unit
    Suite* test_suite = suite_create("internal");

    TCase* abort_unreachable_code = tcase_create("SXBP_ABORT_UNREACHABLE_CODE()");
    tcase_add_test_raise_signal(
        abort_unreachable_code,
        test_unreachable_code_abort_macro,
        SIGABRT // expect that this test case will raise the abort signal
    );
    suite_add_tcase(test_suite, abort_unreachable_code);

    return test_suite;
}
