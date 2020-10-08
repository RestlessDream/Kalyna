#include "unity.h"
#include "../src/util.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_high_bits() {
    TEST_ASSERT_EQUAL_UINT8(0, high_bits(0x0F));
    TEST_ASSERT_EQUAL_UINT8(0xF, high_bits(0xF0));
    TEST_ASSERT_EQUAL_UINT8(0xF, high_bits(0xFF));
    TEST_ASSERT_EQUAL_UINT8(0x2, high_bits(0x22));
}

void test_low_bits() {
    TEST_ASSERT_EQUAL_UINT8(0x0F, low_bits(0x0F));
    TEST_ASSERT_EQUAL_UINT8(0x00, low_bits(0xF0));
    TEST_ASSERT_EQUAL_UINT8(0x0F, low_bits(0xFF));
    TEST_ASSERT_EQUAL_UINT8(0x02, low_bits(0x22));
}
int main() {
    UNITY_BEGIN();
    RUN_TEST(test_high_bits);
    RUN_TEST(test_low_bits);
    return UNITY_END();
}
