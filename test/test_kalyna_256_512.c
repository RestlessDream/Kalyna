#include "unity.h"
#include "../src/kalyna.h"
#include "../src/util.h"
#include "stdlib.h"
#include "stdio.h"

extern standart_config Kalyna_256_512;
extern standart_config * kalyna;
extern size_t rows_count;

void setUp(void) {
    setup(&Kalyna_256_512);
}

void tearDown(void) {
}

void test_add_round_key() {
    TEST_ASSERT_EQUAL_UINT32(8, kalyna->key_dwlength);
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

void test_key_expansion() {
    dw key[8] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                       0x1011121314151617, 0x18191A1B1C1D1E1F,
                       0x2021222324252627, 0x28292A2B2C2D2E2F,
                       0x3031323334353637, 0x38393A3B3C3D3E3F };
    dw expected[76] = { 0xF7BD9738CE49DDA8, 0x0B9ABD79801EE821,
                        0x8860FE42475C9F56, 0x5CD8F433B4C989C4,
                        0x79801EE8218860FE, 0x42475C9F565CD8F4,
                        0x33B4C989C4F7BD97, 0x38CE49DDA80B9ABD,
                        0x400B80A3E5EE1064, 0x02037E6445A8C043,
                        0xC025BC83FB4294B0, 0xA03392A1F20BCAD2,
                        0x6445A8C043C025BC, 0x83FB4294B0A03392,
                        0xA1F20BCAD2400B80, 0xA3E5EE106402037E,
                        0xD011D865E1E63E78, 0x353CDCE9F6E131E6,
                        0x6F7AAF50A3011487, 0x98CD52A4004CD904,
                        0xE9F6E131E66F7AAF, 0x50A301148798CD52,
                        0xA4004CD904D011D8, 0x65E1E63E78353CDC,
                        0xB29987F5AEDC7C50, 0x49872A92B4D70DD0,
                        0x6ECB5266D3774630, 0xF330F21BBBB21C45,
                        0x92B4D70DD06ECB52, 0x66D3774630F330F2,
                        0x1BBBB21C45B29987, 0xF5AEDC7C5049872A,
                        0x535BC565CE0838DF, 0x30FC6AB4D7D11CC7,
                        0x1DF00EB1A3C796AC, 0x1469E8C6CE80238D,
                        0xB4D7D11CC71DF00E, 0xB1A3C796AC1469E8,
                        0xC6CE80238D535BC5, 0x65CE0838DF30FC6A,
                        0x575458552EB47D9C, 0x6246F01AF15E077F,
                        0xCD97D8C55BC8365F, 0x3798FE37B8947898,
                        0x1AF15E077FCD97D8, 0xC55BC8365F3798FE,
                        0x37B8947898575458, 0x552EB47D9C6246F0,
                        0xAE10A655AFA56FDD, 0x369FDEAD237BDBC9,
                        0xB9CFB69D14A77762, 0x4774969AB2022675,
                        0xAD237BDBC9B9CFB6, 0x9D14A77762477496,
                        0x9AB2022675AE10A6, 0x55AFA56FDD369FDE,
                        0x3BA763DCCAF841C5, 0x9B138A7957447C95,
                        0xCB026165A9DCA04A, 0x5D7092BB878B24F5,
                        0x7957447C95CB0261, 0x65A9DCA04A5D7092,
                        0xBB878B24F53BA763, 0xDCCAF841C59B138A,
                        0x19BE451A213F473E, 0x9D5D8713962D5CDE,
                        0xE7367B3C3E7F37C9, 0x6EABA307F3790E79,
                        0x13962D5CDEE7367B, 0x3C3E7F37C96EABA3,
                        0x07F3790E7919BE45, 0x1A213F473E9D5D87,
                        0x5BCA123F021A45E1, 0x156112092D5E3472,
                        0xE9EDEBA57989919B, 0xDEAC86FFEE59CA23 };

    to_endian(key, kalyna->key_dwlength);
    to_endian(expected, kalyna->state_rows * (kalyna->rounds + 1));
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);

    TEST_ASSERT_EQUAL_UINT32(76, kalyna->state_rows * (kalyna->rounds + 1));
    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, w, kalyna->state_rows * (kalyna->rounds + 1));

    free(w);
}

void test_cipher() {
    dw key[8] = { 0x0001020304050607, 0x08090A0B0C0D0E0F,
                  0x1011121314151617, 0x18191A1B1C1D1E1F,
                  0x2021222324252627, 0x28292A2B2C2D2E2F,
                  0x3031323334353637, 0x38393A3B3C3D3E3F };
    to_endian(key, kalyna->key_dwlength);
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);

    dw in[4] = { 0x4041424344454647, 0x48494A4B4C4D4E4F,
                 0x5051525354555657, 0x58595A5B5C5D5E5F };
    to_endian(in, kalyna->state_rows);

    dw * out = malloc(sizeof(dw) * kalyna->state_rows);

    cipher(in, out, w);

    dw expected[4] = { 0x606990E9E6B7B67A, 0x4BD6D893D72268B7,
                       0x8E02C83C3CD7E102, 0xFD2E74A8FDFE5DD9 };
    to_endian(expected, kalyna->state_rows);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, out, kalyna->state_rows);
}

void test_decipher() {
    dw key[8] = { 0x3F3E3D3C3B3A3938, 0x3736353433323130,
                  0x2F2E2D2C2B2A2928, 0x2726252423222120,
                  0x1F1E1D1C1B1A1918, 0x1716151413121110,
                  0x0F0E0D0C0B0A0908, 0x0706050403020100 };
    to_endian(key, kalyna->key_dwlength);
    dw * w = malloc(sizeof(dw) * kalyna->state_rows * (kalyna->rounds + 1));

    key_expansion(key, w);
    dw in[4] = { 0x5F5E5D5C5B5A5958, 0x5756555453525150,
                 0x4F4E4D4C4B4A4948, 0x4746454443424140 };
    to_endian(in, kalyna->state_rows);

    dw * out = malloc(sizeof(dw) * kalyna->state_rows);

    decipher(in, out, w);

    dw expected[4] = { 0x18317A2767DAD482, 0xBCCD07B9A1788D07, 0x5E7098189E5F8497, 0x2D0B916D79BA6AE0 };
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
    RUN_TEST(test_key_expansion);
    RUN_TEST(test_cipher);
    RUN_TEST(test_decipher);

    return UNITY_END();
}
