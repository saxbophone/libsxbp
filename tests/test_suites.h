/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This compilation unit provides forward-declaration of functions used to build
 * the test suites comprising the units we want to test.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "check_wrapper.h"

/*
 * Forward-declarations of the prototypes of all functions that generate Check
 * test suites
 */
Suite* make_bitmap_suite(void);
Suite* make_buffer_suite(void);
Suite* make_figure_suite(void);
// NOTE: this suite isn't testing the public spec
Suite* make_internal_suite(void);
