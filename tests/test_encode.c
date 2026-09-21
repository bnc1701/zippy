#include "test_utils.h"
#include "encode.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int tests_run = 0;
int tests_failed = 0;

static void test_native_matches_reference(void)
{
    huffman_code codes[HUFFMAN_SYMBOLS] = {0};
    const uint8_t input[] = {'a', 'b', 'a', 'c', 'b', 'a', 'c'};
    uint8_t native_output[16] = {0};
    uint8_t reference_output[16] = {0};
    encode_state native_state = {0};
    encode_state reference_state = {0};

    codes['a'].bits = 0;
    codes['a'].length = 1;
    codes['b'].bits = 2;
    codes['b'].length = 2;
    codes['c'].bits = 3;
    codes['c'].length = 2;

    CHECK(encode_huffman(input, sizeof(input), codes, native_output, &native_state) == 0,
          "native encoding succeeds");
    CHECK(encode_huffman_reference(input, sizeof(input), codes, reference_output, &reference_state) == 0,
          "reference encoding succeeds");
    CHECK(native_state.output_size == reference_state.output_size, "output sizes match");
    CHECK(native_state.tail == reference_state.tail, "tail bytes match");
    CHECK(native_state.tail_bits == reference_state.tail_bits, "tail sizes match");
    CHECK(memcmp(native_output, reference_output, native_state.output_size) == 0,
          "output bytes match");
}

static void test_empty_input(void)
{
    huffman_code codes[HUFFMAN_SYMBOLS] = {0};
    uint8_t output[1] = {0};
    encode_state state = {0};

    CHECK(encode_huffman(NULL, 0, codes, output, &state) == 0, "empty encoding succeeds");
    CHECK(state.output_size == 0, "empty output has no bytes");
    CHECK(state.tail_bits == 0, "empty output has no tail");
}

int main(void)
{
    test_native_matches_reference();
    test_empty_input();
    printf("encode: %d run, %d failed\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
