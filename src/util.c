#include "util.h"

byte high_bits(byte val) {
    return (val & 0xF0) >> 4;
}

byte low_bits(byte val) {
    return val & 0x0F;
}

void to_endian(dw * state, size_t size) {
    byte tmp[8];

    for (size_t i = 0; i < size; i++) {
        byte * state_row = (byte *)&state[i];

        for (size_t j = 0; j < 8; j++) {
            tmp[7 - j] = state_row[j];
        }

        state[i] = ((dw *) tmp)[0];
    }
}

void add_cbc_padding(dw * block, size_t size, size_t block_dwsize) {
    if (size >= block_dwsize) {
        return;
    }

    dw padding_num = block_dwsize - size;

    for (size_t i = size; i < block_dwsize; i++) {
        block[i] = padding_num;
    }
}

size_t del_cbc_padding(dw * block, size_t block_dwsize) {
    dw padding_num = block[block_dwsize - 1];

    if (padding_num >= block_dwsize) {
        return 0;
    }

    for (int i = block_dwsize - 2; i >= (int)block_dwsize - padding_num; i--) {
        if (block[i] != padding_num) {
            return 0;
        }
    }

    return padding_num;
}

