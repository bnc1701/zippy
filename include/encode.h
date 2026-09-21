#ifndef ZIPPY_ENCODE_H
#define ZIPPY_ENCODE_H

#include <stddef.h>
#include <stdint.h>

#include "huffman.h"

// encoded output state
typedef struct {
    size_t output_size;
    uint8_t tail;
    uint8_t tail_bits;
} encode_state;

// encode input bytes into packed output bytes
int encode_huffman(
    const uint8_t *input,
    size_t input_size,
    const huffman_code codes[HUFFMAN_SYMBOLS],
    uint8_t *output,
    encode_state *state
);

// encode input with the c reference path
int encode_huffman_reference(
    const uint8_t *input,
    size_t input_size,
    const huffman_code codes[HUFFMAN_SYMBOLS],
    uint8_t *output,
    encode_state *state
);

#endif
