/*
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
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#include <check.h>
// reënable all warnings
#pragma clang diagnostic pop
