#ifndef _KALYNA_H_
#define _KALYNA_H_

#include "static.h"

void setup(standart_config * kalyna_standart);
void cipher(dw * in, dw * out, dw * w);
void cipher_round(dw * state);
void add_round_key(dw * state, const dw * w);
void sub_bytes(dw * state);
void shift_rows(dw * state);
void mix_columns(dw * state);
void matrix_multiply(dw * state, uint8_t mat[8][8]);
byte multiplyGF(byte x, byte y);
void xor_round_key(dw * state, const dw * w);

void decipher(dw * in, dw * out, dw * w);
void decipher_round(dw * state);
void inv_add_round_key(dw * state, dw * w);
void inv_sub_bytes(dw * state);
void inv_shift_rows(dw * state);
void inv_mix_columns(dw * state);

void key_expansion(dw * key, dw * w);
void key_expand_kt(dw * key, dw * kt, dw * state);
void key_expand_even(dw * key, dw * w, dw * kt, dw * state);
void key_expand_odd(dw * state, dw * w);

void shift_left(size_t state_size, dw * state_value);
void rotate(size_t state_size, dw * state_value);
void rotate_left(size_t state_size, dw * state_value);

#endif // _KALYNA_H_
