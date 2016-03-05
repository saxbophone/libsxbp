#include <stdbool.h>
#include <stdio.h>

#include "bspirals.h"


bool
dummy_test_case() {
    // blank dummy test case
    return true;
}

int
main(int argc, char const *argv[]) {
    // run tests
    int result = 0;
    printf("dummy_test_case: ");
    if(!dummy_test_case()) {
        printf("FAIL\n");
        result = 1;
    } else {
        printf("PASS\n");
    }
    return result;
}
