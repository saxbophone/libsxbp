#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "check_wrapper.h"

#include "../sxbp/sxbp.h"

#include "test_suites.h"


START_TEST(test_blank_figure) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_init_figure) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_init_figure_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_free_figure_unallocated) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_free_figure_allocated) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_copy_figure) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_copy_figure_from_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_copy_figure_to_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_begin_figure) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_begin_figure_data_too_big) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_begin_figure_data_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_begin_figure_figure_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_refine_figure) {
    /*
     * NOTE: This function may be rather hard to unit-test as it suffers from
     * the halting problem
     */
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_refine_figure_figure_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_refine_figure_no_lines) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_refine_figure_unimplemented_method) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_dump_figure) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_dump_figure_figure_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_dump_figure_buffer_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_load_figure) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_load_figure_buffer_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_load_figure_figure_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_render_figure) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_render_figure_figure_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_render_figure_buffer_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_render_figure_render_callback_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_render_figure_to_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_render_figure_to_pbm) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_render_figure_to_pbm_figure_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_render_figure_to_pbm_buffer_null) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

START_TEST(test_render_figure_to_svg) {
    // TODO: replace this body with actual test code
    ck_abort_msg("Test not implemented!");
} END_TEST

Suite* make_figure_suite(void) {
    // Test cases for figure data type
    Suite* test_suite = suite_create("Figure");

    TCase* blank_figure = tcase_create("Create blank Figure");
    tcase_add_test(blank_figure, test_blank_figure);
    suite_add_tcase(test_suite, blank_figure);

    TCase* init_figure = tcase_create("Allocate a Figure");
    tcase_add_test(init_figure, test_init_figure);
    suite_add_tcase(test_suite, init_figure);

    TCase* init_figure_null = tcase_create(
        "Figure allocation returns appropriate error code when given NULL pointer"
    );
    tcase_add_test(init_figure_null, test_init_figure_null);
    suite_add_tcase(test_suite, init_figure_null);

    TCase* free_figure_unallocated = tcase_create("Free an unallocated Figure");
    tcase_add_test(free_figure_unallocated, test_free_figure_unallocated);
    suite_add_tcase(test_suite, free_figure_unallocated);

    TCase* free_figure_allocated = tcase_create("Free an allocated Figure");
    tcase_add_test(free_figure_allocated, test_free_figure_allocated);
    suite_add_tcase(test_suite, free_figure_allocated);

    TCase* copy_figure = tcase_create("Copy a figure");
    tcase_add_test(copy_figure, test_copy_figure);
    suite_add_tcase(test_suite, copy_figure);

    TCase* copy_figure_from_null = tcase_create(
        "Figure copying returns appropriate error code when from is NULL"
    );
    tcase_add_test(copy_figure_from_null, test_copy_figure_from_null);
    suite_add_tcase(test_suite, copy_figure_from_null);

    TCase* copy_figure_to_null = tcase_create(
        "Figure copying returns appropriate error code when to is NULL"
    );
    tcase_add_test(copy_figure_to_null, test_copy_figure_to_null);
    suite_add_tcase(test_suite, copy_figure_to_null);

    TCase* begin_figure = tcase_create("Begin generation of a figure");
    tcase_add_test(begin_figure, test_begin_figure);
    suite_add_tcase(test_suite, begin_figure);

    TCase* begin_figure_data_too_big = tcase_create(
        "Figure generation returns appropriate error code when given data that "
        "is too large"
    );
    tcase_add_test(begin_figure_data_too_big, test_begin_figure_data_too_big);
    suite_add_tcase(test_suite, begin_figure_data_too_big);

    TCase* begin_figure_data_null = tcase_create(
        "Figure generation returns appropriate error code when data is NULL"
    );
    tcase_add_test(begin_figure_data_null, test_begin_figure_data_null);
    suite_add_tcase(test_suite, begin_figure_data_null);

    TCase* begin_figure_figure_null = tcase_create(
        "Figure generation returns appropriate error code when figure is NULL"
    );
    tcase_add_test(begin_figure_figure_null, test_begin_figure_figure_null);
    suite_add_tcase(test_suite, begin_figure_figure_null);

    /*
     * TODO: Add these test cases:
     *
     * - test_refine_figure
     * - test_refine_figure_figure_null
     * - test_refine_figure_no_lines
     * - test_refine_figure_unimplemented_method
     * - test_dump_figure
     * - test_dump_figure_figure_null
     * - test_dump_figure_buffer_null
     * - test_load_figure
     * - test_load_figure_buffer_null
     * - test_load_figure_figure_null
     * - test_render_figure
     * - test_render_figure_figure_null
     * - test_render_figure_buffer_null
     * - test_render_figure_render_callback_null
     * - test_render_figure_to_null
     * - test_render_figure_to_pbm
     * - test_render_figure_to_pbm_figure_null
     * - test_render_figure_to_pbm_buffer_null
     * - test_render_figure_to_sv
     *
     */

    return test_suite;
}
