/*
 * This source file forms part of sxbp, a library which generates experimental
 * 2D spiral-like shapes based on input binary data.
 *
 * This is a wrapper header used to wrap <check.h> instead of inlcuding it
 * directly.
 *
 * There's a shortcoming within libcheck, which involves using a GNU extension
 * for macros --this whole project compiles with strict warning flags and Clang
 * complains about these extensions, so silencing these warnings when on Clang
 * is required.
 *
 * NOTE: If and when this is ever resolved within libcheck itself, this wrapper
 * header can be removed.
 *
 * Copyright (C) Joshua Saxby <joshua.a.saxby@gmail.com> 2016-2017, 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#include <check.h>
// reënable all warnings
#pragma clang diagnostic pop
