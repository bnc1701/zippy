#include "encode.h"

// encode input with the portable c path
static int encode_huffman_c(
    const uint8_t *input,
    size_t input_size,
    const huffman_code codes[HUFFMAN_SYMBOLS],
    uint8_t *output,
    encode_state *state
)
{
    size_t output_size = 0;
    uint8_t buffer = 0;
    uint8_t bit_count = 0;

    for (size_t i = 0; i < input_size; i++) {
        huffman_code code = codes[input[i]];
        for (int bit = code.length - 1; bit >= 0; bit--) {
            buffer = (uint8_t)((buffer << 1) | ((code.bits >> bit) & 1u));
            bit_count++;
            if (bit_count == 8) {
                output[output_size++] = buffer;
                buffer = 0;
                bit_count = 0;
            }
        }
    }

    state->output_size = output_size;
    state->tail = buffer;
    state->tail_bits = bit_count;
    return 0;
}

#if defined(__x86_64__)
extern size_t encode_huffman_x86_64(
    const uint8_t *input,
    size_t input_size,
    const huffman_code codes[HUFFMAN_SYMBOLS],
    uint8_t *output,
    encode_state *state
);
#endif

// select the native path when the target supports it
int encode_huffman(
    const uint8_t *input,
    size_t input_size,
    const huffman_code codes[HUFFMAN_SYMBOLS],
    uint8_t *output,
    encode_state *state
)
{
#if defined(__x86_64__)
    state->output_size = encode_huffman_x86_64(input, input_size, codes, output, state);
    return 0;
#else
    return encode_huffman_c(input, input_size, codes, output, state);
#endif
}
