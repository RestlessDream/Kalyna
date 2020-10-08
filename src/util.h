#ifndef _UTIL_H_
#define _UTIL_H_

#include "types.h"

#include "stdlib.h"

byte high_bits(byte val);
byte low_bits(byte val);
void to_endian(dw * state, size_t size);
size_t del_cbc_padding(dw * block, size_t block_dwsize);
void add_cbc_padding(dw * block, size_t size, size_t block_dwsize);

#endif // UTIL_H_
