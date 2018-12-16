#include <check.h>

#include "../sxbp/sxbp.h"


START_TEST(test_blank_bitmap)
{
    sxbp_bitmap_t bitmap = sxbp_blank_bitmap();

    ck_assert(bitmap.width == 0);
    ck_assert(bitmap.height == 0);
    ck_assert(bitmap.pixels == NULL);
}
END_TEST
