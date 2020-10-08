#include "unity.h"
#include "../src/kalyna.h"
#include "../src/util.h"
#include "stdlib.h"
#include "stdio.h"

extern standart_config Kalyna_128_128;
extern standart_config * kalyna;
extern size_t rows_count;

void setUp(void) {
    setup(&Kalyna_128_128);
}

void tearDown(void) {
}

void test_add_round_key() {
    TEST_ASSERT_EQUAL_UINT32(2, kalyna->key_dwlength);
    TEST_ASSERT_EQUAL_UINT32(2, kalyna->state_rows);
    TEST_ASSERT_EQUAL_UINT32(2, kalyna->block_dwsize);

    dw state[2] = { 0x53E85C8F02C0CA94, 0xB7578DD19C8B8A35 };
    dw key[2] = { 0x0001020304050607, 0x08090A0B0C0D0E0F };
    to_endian(state, kalyna->state_rows);
    to_endian(key, kalyna->state_rows);

    add_round_key(state, key);

    dw expected[2] = { 0x53E95E9206C5D09B, 0xBF6097DCA8989844 };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_sub_bytes() {
    dw state[2] = { 0x0501020304050607, 0x08090A0B0C0D0E0F };
    dw expected[2] = { 0x75BB9A4D6BCB452A, 0x713ADFB31790511F };
    to_endian(state, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    sub_bytes(state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_shift_rows() {
    dw state[2] = { 0x75BB9A4D6BCB452A, 0x713ADFB31790511F };
    to_endian(state, kalyna->state_rows);

    shift_rows(state);

    dw expected[2] = { 0x75BB9A4D1790511F, 0x713ADFB36BCB452A };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_mix_columns() {
    dw state[2] = { 0x75BB9A4D1790511F, 0x713ADFB36BCB452A };
    to_endian(state, kalyna->state_rows);

    mix_columns(state);

    dw expected[2] = { 0x62C97C6E6ABF4133, 0xED5131D624C7C182 };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_xor_round_key() {
    dw state[2] = { 0x62C97C6E6ABF4133, 0xED5131D624C7C182 };
    dw key[2] = { 0x0001020304050607, 0x08090A0B0C0D0E0F };
    to_endian(state, kalyna->state_rows);
    to_endian(key, kalyna->state_rows);

    xor_round_key(state, key);

    dw expected[2] = { 0x62C87E6D6EBA4734, 0xE5583BDD28CACF8D };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_inv_add_round_key() {
    dw state[2] = { 0x1F1E1D1C1B1A1918, 0x1716151413121110 };
    dw round_key[2] = { 0x45D32764EB4B669E, 0xD8A3B2E73888CC77 };
    to_endian(state, kalyna->state_rows);
    to_endian(round_key, kalyna->key_dwlength);

    inv_add_round_key(state, round_key);
    
    dw expected[2] = { 0xDA4AF5B72FCEB279, 0x3F72622CDA894498 };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_inv_sub_bytes() {
    dw state[2] = { 0xAFF9B83FB5CB2EAD, 0xB66A08CECDB4966C };
    dw expected[2] = { 0x17AF69BA9A0547EB, 0x259BC23A8813BDB0 };
    to_endian(state, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    inv_sub_bytes(state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_inv_shift_rows() {
    dw state[2] = { 0xAFF9B83FCDB4966C, 0xB66A08CEB5CB2EAD };
    dw expected[2] = { 0xAFF9B83FB5CB2EAD, 0xB66A08CECDB4966C };
    to_endian(state, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    inv_shift_rows(state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_inv_mix_columns() {
    dw state[2] = { 0xE7BD3FE8893B3119, 0x6521963EAB2E88EB};
    dw expected[2] = { 0xFE8350F9596E1B29, 0xF02AD90C1FC24A23};
    to_endian(state, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    inv_mix_columns(state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_key_expand_kt() {
    dw key[2] = { 0x0001020304050607, 0x08090A0B0C0D0E0F };
    dw expected[2] = { 0x862F1F653B775BA1, 0xD05CBC2F38E2D87D };
    dw * kt = (dw *) malloc(rows_count * kalyna->state_rows);
    dw * state = (dw *) malloc(rows_count * kalyna->state_rows);

    to_endian(key, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    key_expand_kt(key, kt, state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, kt, kalyna->state_rows);

    free(kt);
    free(state);
}

void test_key_expansion() {
    dw key[2] = { 0x0001020304050607, 0x08090A0B0C0D0E0F };
    dw expected[22] = { 0x16505E6B9B3AB1E6, 0x865B77DCE082A0F4,
                    0xE6865B77DCE082A0, 0xF416505E6B9B3AB1,
                    0x7E70876EAE498476, 0x8AAAA00A7C93EC42,
                    0x768AAAA00A7C93EC, 0x427E70876EAE4984,
                    0x45CED4C51E9140F5, 0x3E7276820F0BD9FE,
                    0xF53E7276820F0BD9, 0xFE45CED4C51E9140,
                    0x8C77EE227900C462, 0x515F66320560C4B1,
                    0x62515F66320560C4, 0xB18C77EE227900C4,
                    0x0A9872E25CD2B0B8, 0xAA879A2086A66DD8,
                    0xB8AA879A2086A66D, 0xD80A9872E25CD2B0,
                    0x5726B1A894DBC418, 0xF60BF3D5E8D74861 };
    to_endian(key, kalyna->key_dwlength);
    to_endian(expected, kalyna->state_rows * (kalyna->rounds + 1));
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);

    TEST_ASSERT_EQUAL_UINT32(22, kalyna->state_rows * (kalyna->rounds + 1));
    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, w, kalyna->state_rows * (kalyna->rounds + 1));

    free(w);
}

void test_shift_left() {
    dw tmv[2] = { 0x0100010001000100, 0x0100010001000100 };
    dw expected[2] = { 0x0200020002000200, 0x0200020002000200 };
    to_endian(tmv, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    shift_left(kalyna->state_rows, tmv);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, tmv, kalyna->state_rows);
}

void test_rotate() {
    dw id[2] = { 0x0001020304050607, 0x08090A0B0C0D0E0F };

    dw expected[2] = { 0x08090A0B0C0D0E0F, 0x0001020304050607 };
    to_endian(id, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    rotate(kalyna->key_dwlength, id);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, id, kalyna->key_dwlength);
}

void test_rotate_left() {
    dw state_value[2] = { 0x16505E6B9B3AB1E6, 0x865B77DCE082A0F4 };

    dw expected[2] = { 0xE6865B77DCE082A0, 0xF416505E6B9B3AB1 };

    to_endian(state_value, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    rotate_left(kalyna->state_rows, state_value);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state_value, kalyna->state_rows);
}

void test_cipher() {
    dw key[2] = { 0x0001020304050607, 0x08090A0B0C0D0E0F };
    to_endian(key, kalyna->key_dwlength);
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);

    dw in[2] = { 0x1011121314151617, 0x18191A1B1C1D1E1F };
    to_endian(in, kalyna->state_rows);

    dw * out = malloc(sizeof(dw) * kalyna->state_rows);

    cipher(in, out, w);

    dw expected[2] = { 0x81BF1C7D779BAC20, 0xE1C9EA39B4D2AD06 };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, out, kalyna->state_rows);
}

void test_decipher() {
    dw key[2] = { 0x0F0E0D0C0B0A0908, 0x0706050403020100 };
    to_endian(key, kalyna->key_dwlength);
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);
    dw in[2] = { 0x1F1E1D1C1B1A1918, 0x1716151413121110 };
    to_endian(in, kalyna->state_rows);

    dw * out = malloc(sizeof(dw) * kalyna->state_rows);

    decipher(in, out, w);

    dw expected[2] = { 0x7291EF2B470CC784, 0x6F09C2303973DAD7 };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, out, kalyna->state_rows);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_add_round_key);
    RUN_TEST(test_sub_bytes);
    RUN_TEST(test_shift_rows);
    RUN_TEST(test_mix_columns);
    RUN_TEST(test_xor_round_key);
    RUN_TEST(test_inv_add_round_key);
    RUN_TEST(test_inv_sub_bytes);
    RUN_TEST(test_inv_shift_rows);
    RUN_TEST(test_inv_mix_columns);
    RUN_TEST(test_key_expand_kt);
    RUN_TEST(test_key_expansion);
    RUN_TEST(test_shift_left);
    RUN_TEST(test_rotate);
    RUN_TEST(test_rotate_left);
    RUN_TEST(test_cipher);
    RUN_TEST(test_decipher);

    return UNITY_END();
}
