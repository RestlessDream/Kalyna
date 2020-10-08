#include "unity.h"
#include "../src/kalyna.h"
#include "../src/util.h"
#include "stdlib.h"
#include "stdio.h"

extern standart_config Kalyna_128_256;
extern standart_config * kalyna;
extern size_t rows_count;

void setUp(void) {
    setup(&Kalyna_128_256);
}

void tearDown(void) {
}

void test_add_round_key() {
    TEST_ASSERT_EQUAL_UINT32(4, kalyna->key_dwlength);
    TEST_ASSERT_EQUAL_UINT32(2, kalyna->state_rows);
    TEST_ASSERT_EQUAL_UINT32(2, kalyna->block_dwsize);

    dw state[2] = { 0x0700000000000000, 0x0000000000000000 };
    dw key[4] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                  0x1011121314151617, 0x18191A1B1C1D1E1F };
    to_endian(state, kalyna->state_rows);
    to_endian(key, kalyna->key_dwlength);

    add_round_key(state, key);

    dw expected[2] = { 0x0701020304050607, 0x08090A0B0C0D0E0F };
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
    dw state[2] = { 0x2F2E2D2C2B2A2928, 0x2726252423222120 };
    dw round_key[2] = { 0x44FEDFC4E5CDB7AA, 0x3F52FCDDD9538EF2 };
    to_endian(state, kalyna->state_rows);
    to_endian(round_key, kalyna->key_dwlength);

    inv_add_round_key(state, round_key);
    
    dw expected[2] = { 0xEB2F4D67455C717D, 0xE8D3284649CE922D };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_inv_sub_bytes() {
    dw state[2] = { 0x07364CA452317200, 0xE417C0B371C4E367 };
    dw expected[2] = { 0xD91A3C98E1202FB2, 0x450E220B08943378 };
    to_endian(state, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    inv_sub_bytes(state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_inv_shift_rows() {
    dw state[2] = { 0x07364CA471C4E367, 0xE417C0B352317200 };
    dw expected[2] = { 0x07364CA452317200, 0xE417C0B371C4E367 };
    to_endian(state, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    inv_shift_rows(state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_inv_mix_columns() {
    dw state[2] = { 0xEB2F4D67455C717D, 0xE8D3284649CE922D };
    dw expected[2] = { 0x07364CA471C4E367, 0xE417C0B352317200 };
    to_endian(state, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    inv_mix_columns(state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_key_expand_kt() {
    dw key[4] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                  0x1011121314151617, 0x18191A1B1C1D1E1F };
    dw expected[2] = { 0x1F4477802D366859, 0x9A40153652482CBF };
    dw * kt = (dw *) malloc(rows_count * kalyna->state_rows);
    dw * state = (dw *) malloc(rows_count * kalyna->state_rows);

    to_endian(key, kalyna->key_dwlength);
    to_endian(expected, kalyna->state_rows);

    key_expand_kt(key, kt, state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, kt, kalyna->state_rows);

    free(kt);
    free(state);
}

void test_key_expansion() {
    dw key[4] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                  0x1011121314151617, 0x18191A1B1C1D1E1F };
    dw expected[30] = { 0x57C816EB3F7E12DE, 0xED2C6B56E6B5BE1A,
                        0xDEED2C6B56E6B5BE, 0x1A57C816EB3F7E12,
                        0xD8069A7D889ACD80, 0xCD3184456CCCAE6F,
                        0x80CD3184456CCCAE, 0x6FD8069A7D889ACD,
                        0xC361CC973513411A, 0x82324D2B6742F3FE,
                        0x1A82324D2B6742F3, 0xFEC361CC97351341,
                        0x8310698C65CF80A4, 0x09EF6FAABEB80F56,
                        0xA409EF6FAABEB80F, 0x568310698C65CF80,
                        0xC6D5C4C381461A7B, 0x034D691842901510,
                        0x7B034D6918429015, 0x10C6D5C4C381461A,
                        0x84D0F82146C8BDF9, 0xB2B3707B4D49387E,
                        0xF9B2B3707B4D4938, 0x7E84D0F82146C8BD,
                        0x43FADB28A0D1D42B, 0xBFF92FF9794546B3,
                        0x2BBFF92FF9794546, 0xB343FADB28A0D1D4,
                        0xC31EE8A87E2CED24, 0x5A21A435FDB25B92 };
    to_endian(key, kalyna->key_dwlength);
    to_endian(expected, kalyna->state_rows * (kalyna->rounds + 1));
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);

    TEST_ASSERT_EQUAL_UINT32(30, kalyna->state_rows * (kalyna->rounds + 1));
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
    dw id[4] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                 0x1011121314151617, 0x18191A1B1C1D1E1F };

    dw expected[4] = { 0x08090A0B0C0D0E0F, 0x1011121314151617,
                       0x18191A1B1C1D1E1F, 0x0001020304050607 };
    to_endian(id, kalyna->key_dwlength);
    to_endian(expected, kalyna->key_dwlength);

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
    dw key[4] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                  0x1011121314151617, 0x18191A1B1C1D1E1F };
    to_endian(key, kalyna->key_dwlength);
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);

    dw in[2] = { 0x2021222324252627, 0x28292A2B2C2D2E2F };
    to_endian(in, kalyna->state_rows);

    dw * out = malloc(sizeof(dw) * kalyna->state_rows);

    cipher(in, out, w);

    dw expected[2] = { 0x58EC3E091000158A, 0x1148F7166F334F14 };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, out, kalyna->state_rows);
}

void test_decipher() {
    dw key[4] = { 0x1F1E1D1C1B1A1918, 0x1716151413121110,
                  0x0F0E0D0C0B0A0908, 0x0706050403020100 };
    to_endian(key, kalyna->key_dwlength);
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);
    dw in[2] = { 0x2F2E2D2C2B2A2928, 0x2726252423222120 };
    to_endian(in, kalyna->state_rows);

    dw * out = malloc(sizeof(dw) * kalyna->state_rows);

    decipher(in, out, w);

    dw expected[2] = { 0xF36DB456CEFDDFE1, 0xB45B5F7030CAD996 };
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
