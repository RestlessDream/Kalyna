#include "kalyna.h"

#include "util.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

extern standart_config * kalyna;
extern size_t block_bsize;
extern const size_t rows_count;
extern size_t byte_size;
extern size_t dword_size;
extern size_t max_block_dwsize;
extern const byte s_box[4][16][16];
extern byte inv_s_box[4][16][16];
extern size_t s_box_dimensions;
extern byte mds_matrix[8][8];
extern byte mds_inv_matrix[8][8];
 
void cipher(dw * in, dw * out, dw * w) {
    dw * state = (dw *) malloc(rows_count * kalyna->state_rows);

    memcpy(state, in, rows_count * kalyna->state_rows);

    add_round_key(state, w);

    int round = 1;
    for (; round < kalyna->rounds; round++) {
        cipher_round(state);
        xor_round_key(state, w + round * kalyna->state_rows);
    }

    cipher_round(state);
    add_round_key(state, w + kalyna->rounds * kalyna->state_rows);

    memcpy(out, state, rows_count * kalyna->state_rows);
}

void cipher_round(dw * state) {
    sub_bytes(state);
    shift_rows(state);
    mix_columns(state);
}

void add_round_key(dw * state, const dw * w) {
    for (size_t i = 0; i < kalyna->state_rows; i++) {
        state[i] += w[i];
    }
}

void sub_bytes(dw * state) {
    for (size_t clmn = 0; clmn < kalyna->state_rows; clmn++) {
        byte * state_clmn = (byte *)&state[clmn];
        for (size_t row = 0; row < rows_count; row++) {
            state_clmn[row] = s_box[row % s_box_dimensions][high_bits(state_clmn[row])]
                                                      [low_bits(state_clmn[row])];
        }
    }
}

void shift_rows(dw * state) {
    for (size_t i = 0; i < rows_count; i++) {
        size_t shift_s = (i * kalyna->block_dwsize) / max_block_dwsize; 

        for (size_t j = 0; j < shift_s; j++) {
            byte swp = ((byte *)&state[kalyna->state_rows - 1])[i];

            for (size_t k = kalyna->state_rows - 1; k > 0; k--) {
                ((byte *)&state[k])[i] = ((byte *)&state[k - 1])[i];
            }
            ((byte *)&state[0])[i] = swp;
        }
    }
}

void mix_columns(dw * state) {
    matrix_multiply(state, mds_matrix);
}

void matrix_multiply(dw * state, uint8_t mat[8][8]) {
    byte product;
    dw result;

    for (int col = 0; col < kalyna->state_rows; col++) {
        result = 0;

        for (int row = rows_count - 1; row >= 0; row--) {
            product = 0;
            for (int i = rows_count - 1; i >= 0; i--) {
                product ^= multiplyGF(((byte *)&state[col])[i], mat[row][i]);
            }

            result |= (dw)product << (row * rows_count);
        }

        state[col] = result;
    }
}

byte multiplyGF(byte x, byte y) {
    byte r = 0;
    byte hbit = 0;

    for (size_t i = 0; i < byte_size; i++) {
        if ((y & 0x1) == 1) {
            r ^= x;
        }

        hbit = x & 0x80;
        x <<= 1;

        if (hbit == 0x80) {
            x ^= 0x011d;
        }

        y >>= 1;
    }

    return r;
}

void xor_round_key(dw * state, const dw * w) {
    for (size_t i = 0; i < rows_count; i++) {
        state[i] ^= w[i];
    }
}

void decipher(dw * in, dw * out, dw * w) {
    dw * state = (dw *) malloc(rows_count * kalyna->state_rows);

    memcpy(state, in, rows_count * kalyna->state_rows);

    inv_add_round_key(state, w + kalyna->rounds * kalyna->state_rows);

    int round = kalyna->rounds - 1;
    for (; round > 0; round--) {
        decipher_round(state);
        xor_round_key(state, w + round * kalyna->state_rows);
    }

    decipher_round(state);
    inv_add_round_key(state, w);

    memcpy(out, state, rows_count * kalyna->state_rows);
}

void decipher_round(dw * state) {
    inv_mix_columns(state);
    inv_shift_rows(state);
    inv_sub_bytes(state);
}

void inv_add_round_key(dw * state, dw * w) {
    for (size_t i = 0; i < kalyna->state_rows; i++) {
        state[i] -= w[i];
    }
}

void inv_sub_bytes(dw * state) {
    for (size_t clmn = 0; clmn < kalyna->state_rows; clmn++) {
        byte * state_clmn = (byte *)&state[clmn];
        for (size_t row = 0; row < rows_count; row++) {
            state_clmn[row] = inv_s_box[row % s_box_dimensions][high_bits(state_clmn[row])]
                                                      [low_bits(state_clmn[row])];
        }
    }
}

void inv_shift_rows(dw * state) {
    for (size_t i = 0; i < rows_count; i++) {
        size_t shift_s = (i * kalyna->block_dwsize) / max_block_dwsize; 

        for (size_t j = 0; j < shift_s; j++) {
            byte swp = ((byte *)&state[0])[i];

            for (size_t k = 0; k < kalyna->state_rows - 1; k++) {
                ((byte *)&state[k])[i] = ((byte *)&state[k + 1])[i];
            }
            ((byte *)&state[kalyna->state_rows - 1])[i] = swp;
        }
    }
}

void inv_mix_columns(dw * state) {
    matrix_multiply(state, mds_inv_matrix);
}

void key_expansion(dw * key, dw * w) {
    dw * kt = (dw *) malloc(rows_count * kalyna->state_rows);
    dw * state = (dw *) malloc(rows_count * kalyna->state_rows);

    key_expand_kt(key, kt, state);
    key_expand_even(key, w, kt, state);
    key_expand_odd(state, w);

    free(kt);
    free(state);
}

void key_expand_kt(dw * key, dw * kt, dw * state) {
    dw * k0 = malloc(rows_count * kalyna->state_rows);
    dw * k1 = malloc(rows_count * kalyna->state_rows);

    memset(state, 0, rows_count * kalyna->state_rows);
    ((byte *)state)[0] = rows_count + kalyna->state_rows + 1; 

    if (rows_count == kalyna->state_rows) {
        k0 = memcpy(k0, key, rows_count * kalyna->state_rows);
        k1 = memcpy(k1, key, rows_count * kalyna->state_rows);
    } else {
        k0 = memcpy(k0, key, rows_count * kalyna->state_rows);
        k1 = memcpy(k1, key + kalyna->state_rows, rows_count * kalyna->state_rows);
    }

    add_round_key(state, k0);
    cipher_round(state);
    xor_round_key(state, k1);
    cipher_round(state);
    add_round_key(state, k0);
    cipher_round(state);
    memcpy(kt, state, rows_count * kalyna->state_rows);

    free(k0);
    free(k1);
}

void key_expand_even(dw * key, dw * w, dw * kt, dw * state) {
    dw * initial_data = malloc(kalyna->key_dwlength * rows_count);
    dw * kt_round = malloc(kalyna->state_rows * rows_count);
    dw * tmv = malloc(kalyna->state_rows * rows_count);
    size_t round = 0;

    memcpy(initial_data, key, kalyna->key_dwlength * rows_count);

    for (int i = 0; i < kalyna->state_rows; i++) {
        tmv[i] = 0x0001000100010001;
    }

    while (1) {
        if (round == 9) {
            printf("HELLO DYAN");
        }
        memcpy(state, kt, kalyna->state_rows * rows_count);
        add_round_key(state, tmv);
        memcpy(kt_round, state, kalyna->state_rows * rows_count);

        memcpy(state, initial_data, kalyna->state_rows * rows_count);

        add_round_key(state, kt_round);
        cipher_round(state);
        xor_round_key(state, kt_round);
        cipher_round(state);
        add_round_key(state, kt_round);

        memcpy(w + round * kalyna->state_rows, state, kalyna->state_rows * rows_count);

        if (kalyna->rounds == round) {
            break;
        }

        if (kalyna->key_dwlength != kalyna->block_dwsize) {
            round += 2;

            shift_left(kalyna->state_rows, tmv);

            memcpy(state, kt, kalyna->state_rows * rows_count);
            add_round_key(state, tmv);
            memcpy(kt_round, state, kalyna->state_rows * rows_count);

            memcpy(state, initial_data + kalyna->state_rows, kalyna->state_rows * rows_count);

            add_round_key(state, kt_round);
            cipher_round(state);
            xor_round_key(state, kt_round);
            cipher_round(state);
            add_round_key(state, kt_round);

            memcpy(w + round * kalyna->state_rows, state, kalyna->state_rows * rows_count);

            if (kalyna->rounds == round) {
                break;
            }
        }

        round += 2;
        shift_left(kalyna->state_rows, tmv);
        rotate(kalyna->key_dwlength, initial_data);
    }

    free(initial_data);
    free(kt_round);
    free(tmv);
}

void key_expand_odd(dw * state, dw * w) {
    for (int i = 1; i < kalyna->rounds; i += 2) {
        memcpy(w + i * kalyna->state_rows, w + (i - 1) * kalyna->state_rows, kalyna->state_rows * rows_count);
        rotate_left(kalyna->state_rows, w + i * kalyna->state_rows);
    }
}

void shift_left(size_t state_size, dw * state_value) {
    for (int i = 0; i < state_size; i++) {
        state_value[i] <<= 1;
    }
}

void rotate(size_t state_size, dw * state_value) {
    dw temp = state_value[0];

    for (int i = 1; i < state_size; i++) {
        state_value[i - 1] = state_value[i];
    }

    state_value[state_size - 1] = temp;
}

void rotate_left(size_t state_size, dw * state_value) {
    size_t rotate_bytes = 2 * state_size + 3;
    size_t bytes_num = state_size * (dword_size / byte_size);

    byte * bytes = (byte *)state_value;
    byte * buffer = malloc(rotate_bytes);

    memcpy(buffer, bytes, rotate_bytes);
    memmove(bytes, bytes + rotate_bytes, bytes_num - rotate_bytes);
    memcpy(bytes + bytes_num - rotate_bytes, buffer, rotate_bytes);

    free(buffer);
}

//void add_cbc_padding(byte * block, size_t size) {
//    if (size >= block_bsize) {
//        return;
//    }
//
//    byte padding_num = block_bsize - size;
//
//    for (size_t i = size; i < block_bsize; i++) {
//        block[i] = padding_num;
//    }
//}
//
//size_t del_cbc_padding(byte * block) {
//    byte padding_num = block[block_bsize - 1];
//
//    if (padding_num >= block_bsize) {
//        return 0;
//    }
//
//    for (int i = block_bsize - 2; i >= block_bsize - padding_num; i--) {
//        if (block[i] != padding_num) {
//            return 0;
//        }
//    }
//
//    return padding_num;
//}

void setup(standart_config * kalyna_standart) {
    kalyna = kalyna_standart;
}
