#ifndef _TYPES_H_
#define _TYPES_H_

#include "stdlib.h"
#include "stdint.h"

typedef uint8_t byte;
typedef uint32_t word;
typedef uint64_t dw;

typedef struct {
    const size_t block_dwsize;
    const size_t key_dwlength;
    const size_t rounds;
    const size_t state_rows;
} standart_config;

#endif // TYPES_H_
