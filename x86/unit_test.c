#include "minunit.h"

MU_TEST(foo) {
    mu_check(5 == 7);
}
MU_TEST(bar) {
    mu_check(1 == 1);
}

MU_TEST_SUITE(test_suite) {
    MU_RUN_TEST(foo);
    MU_RUN_TEST(bar);
}

int main() {
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
    return MU_EXIT_CODE;
}
