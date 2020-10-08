#include "unity.h"
#include "../src/kalyna.h"
#include "../src/util.h"
#include "stdlib.h"
#include "stdio.h"

extern standart_config Kalyna_256_256;
extern standart_config * kalyna;
extern size_t rows_count;

void setUp(void) {
    setup(&Kalyna_256_256);
}

void tearDown(void) {
}

void test_add_round_key() {
    TEST_ASSERT_EQUAL_UINT32(4, kalyna->key_dwlength);
    TEST_ASSERT_EQUAL_UINT32(4, kalyna->state_rows);
    TEST_ASSERT_EQUAL_UINT32(4, kalyna->block_dwsize);

    dw state[4] = { 0xD905723FF356C644, 0xF1610397522EE49F,
                    0xA5A1D64F36C3C5C9, 0x671996CA04DD3C87 };
    dw key[4] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                  0x1011121314151617, 0x18191A1B1C1D1E1F };
    to_endian(state, kalyna->state_rows);
    to_endian(key, kalyna->key_dwlength);

    add_round_key(state, key);

    dw expected[4] = { 0xD9067442F75BCC4B, 0xF96A0DA25E3BF2AE, 0xB5B2E8624AD8DBE0, 0x7F32B0E520FA5AA6 };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_sub_bytes() {
    dw state[4] = { 0x0901020304050607, 0x08090A0B0C0D0E0F,
                    0x1011121314151617, 0x18191A1B1C1D1E1F };
    dw expected[4] = { 0xDFBB9A4D6BCB452A, 0x713ADFB31790511F,
                       0x6D152B3DC91CBB83, 0x795C71D56F5716BD };
    to_endian(state, kalyna->state_rows);
    to_endian(expected, kalyna->state_rows);

    sub_bytes(state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_shift_rows() {
    dw state[4] = { 0xDFBB9A4D6BCB452A, 0x713ADFB31790511F,
                    0x6D152B3DC91CBB83, 0x795C71D56F5716BD };
    to_endian(state, kalyna->state_rows);

    shift_rows(state);

    dw expected[4] = { 0xDFBB71D5C91C511F, 0x713A9A4D6F57BB83, 0x6D15DFB36BCB16BD, 0x795C2B3D1790452A };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_mix_columns() {
    dw state[4] = { 0xDFBB71D5C91C511F, 0x713A9A4D6F57BB83,
                    0x6D15DFB36BCB16BD, 0x795C2B3D1790452A };
    to_endian(state, kalyna->state_rows);

    mix_columns(state);

    dw expected[4] = { 0x81B873FEF0692FBB, 0x4EE3702C2DC1ECF9,
                       0x1C3B2ECCB31C46CB, 0x0EF13CE6FEE16710  };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_xor_round_key() {
    dw state[4] = { 0x81B873FEF0692FBB, 0x4EE3702C2DC1ECF9,
                       0x1C3B2ECCB31C46CB, 0x0EF13CE6FEE16710  };
    dw key[4] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                  0x1011121314151617, 0x18191A1B1C1D1E1F };
    to_endian(state, kalyna->state_rows);
    to_endian(key, kalyna->key_dwlength);

    xor_round_key(state, key);

    dw expected[4] = { 0x81B971FDF46C29BC, 0x46EA7A2721CCE2F6,
                       0x0C2A3CDFA70950DC, 0x16E826FDE2FC790F };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kalyna->state_rows);
}

void test_key_expand_kt() {
    dw key[4] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                  0x1011121314151617, 0x18191A1B1C1D1E1F };
    dw expected[4] = { 0x19198C1CA96B064B, 0x8E0ED34A5D7F1A85,
                       0xEF0932D459CFE4B2, 0x9BA66D7355B8AEF8 };
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
    dw expected[60] = { 
        0xF7DA2647DFD55B35, 0x2F085208E30FCBA1, 0x69B3C9DCC80DD780, 0x1F072CC16C942E36,
        0x08E30FCBA169B3C9, 0xDCC80DD7801F072C, 0xC16C942E36F7DA26, 0x47DFD55B352F0852,
        0xDF1F1158FD74EEA5, 0xC15531C9239038DB, 0x26839A9100031FCB, 0xB77CFD35F14F73C2,
        0xC9239038DB26839A, 0x9100031FCBB77CFD, 0x35F14F73C2DF1F11, 0x58FD74EEA5C15531,
        0xCA2AE58A5C656C0A, 0xCE0BF0A628FBA9FC, 0x3176E710FC11F75D, 0xEB9601F95C22FD12,
        0xA628FBA9FC3176E7, 0x10FC11F75DEB9601, 0xF95C22FD12CA2AE5, 0x8A5C656C0ACE0BF0,
        0x5157FCECC510404E, 0x44F5E76CF2F2CB80, 0xC29BEE53E310BE7D, 0xF816A49AEB175160,
        0x6CF2F2CB80C29BEE, 0x53E310BE7DF816A4, 0x9AEB1751605157FC, 0xECC510404E44F5E7,
        0x5E1C15889B51862A, 0xA11DD2875781B8D1, 0x0BD0A270AA4669B0, 0x6B7A49B2473467EB,
        0x875781B8D10BD0A2, 0x70AA4669B06B7A49, 0xB2473467EB5E1C15, 0x889B51862AA11DD2,
        0x3752265E1FA9E6AA, 0x082AA99931C8B67B, 0x0864DFE7946F4FEF, 0x7C5BAD7C9212DB02,
        0x9931C8B67B0864DF, 0xE7946F4FEF7C5BAD, 0x7C9212DB02375226, 0x5E1FA9E6AA082AA9,
        0x38A743D67F8838BE, 0x74D17E6C1734E7C5, 0x2BC87D984406B606, 0x79A71C17F9BA8F3B,
        0x6C1734E7C52BC87D, 0x984406B60679A71C, 0x17F9BA8F3B38A743, 0xD67F8838BE74D17E,
        0xFC69B1A735C1CA49, 0x1850EBBB88D6FCAC, 0x5D7616165C109C31, 0xF13144C5B825EA02 };

    to_endian(key, kalyna->key_dwlength);
    to_endian(expected, kalyna->state_rows * (kalyna->rounds + 1));
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);

    TEST_ASSERT_EQUAL_UINT32(60, kalyna->state_rows * (kalyna->rounds + 1));
    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, w, kalyna->state_rows * (kalyna->rounds + 1));

    free(w);
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

void test_cipher() {
    dw key[4] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                  0x1011121314151617, 0x18191A1B1C1D1E1F };
    to_endian(key, kalyna->key_dwlength);
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);

    dw in[4] = { 0x2021222324252627, 0x28292A2B2C2D2E2F,
                 0x3031323334353637, 0x38393A3B3C3D3E3F };
    to_endian(in, kalyna->state_rows);

    dw * out = malloc(sizeof(dw) * kalyna->state_rows);

    cipher(in, out, w);

    dw expected[4] = { 0xF66E3D570EC92135, 0xAEDAE323DCBD2A8C,
                       0xA03963EC206A0D5A, 0x88385C24617FD92C };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, out, kalyna->state_rows);
}

void test_decipher() {
    dw key[4] = { 0x1F1E1D1C1B1A1918, 0x1716151413121110,
                  0x0F0E0D0C0B0A0908, 0x0706050403020100 };
    to_endian(key, kalyna->key_dwlength);
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);
    dw in[4] = { 0x3F3E3D3C3B3A3938, 0x3736353433323130,
                 0x2F2E2D2C2B2A2928, 0x2726252423222120 };
    to_endian(in, kalyna->state_rows);

    dw * out = malloc(sizeof(dw) * kalyna->state_rows);

    decipher(in, out, w);

    dw expected[4] = { 0x7FC5237896674E86, 0x03C1E9B03F8B4BA3,
                       0xAB5B7C592C3FC3D3, 0x61EDD12586B20FE3 };
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
    RUN_TEST(test_key_expand_kt);
    RUN_TEST(test_key_expansion);
    RUN_TEST(test_rotate);
    RUN_TEST(test_cipher);
    RUN_TEST(test_decipher);

    return UNITY_END();
}
