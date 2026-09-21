#include "compress.h"
#include "encode.h"
#include "huffman.h"
#include "serialize.h"
#include "format.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int count_distinct(const uint64_t freq[HUFFMAN_SYMBOLS])
{
    int distinct = 0;
    for (int i = 0; i < HUFFMAN_SYMBOLS; i++) {
        if (freq[i] > 0) {
            distinct++;
        }
    }
    return distinct;
}

static int write_header(FILE *out, uint64_t original_size, const uint64_t freq[HUFFMAN_SYMBOLS])
{
    if (fwrite(ZIPPY_MAGIC, 1, ZIPPY_MAGIC_LEN, out) != ZIPPY_MAGIC_LEN) {
        return -1;
    }
    if (write_u64(out, original_size) != 0) {
        return -1;
    }

    int distinct = count_distinct(freq);
    if (write_u16(out, (uint16_t)distinct) != 0) {
        return -1;
    }

    for (int i = 0; i < HUFFMAN_SYMBOLS; i++) {
        if (freq[i] == 0) {
            continue;
        }
        uint8_t symbol = (uint8_t)i;
        if (fwrite(&symbol, 1, 1, out) != 1 || write_u64(out, freq[i]) != 0) {
            return -1;
        }
    }
    return 0;
}

int zippy_compress(const char *input_path, const char *output_path)
{
    FILE *in = fopen(input_path, "rb");
    if (in == NULL) {
        return -1;
    }
    if (fseek(in, 0, SEEK_END) != 0) {
        fclose(in);
        return -1;
    }

    long file_size = ftell(in);
    if (file_size < 0) {
        fclose(in);
        return -1;
    }
    rewind(in);

    size_t input_size = (size_t)file_size;
    uint8_t *input = malloc(input_size > 0 ? input_size : 1);
    if (input == NULL) {
        fclose(in);
        return -1;
    }
    if (input_size > 0 && fread(input, 1, input_size, in) != input_size) {
        free(input);
        fclose(in);
        return -1;
    }
    fclose(in);

    uint64_t freq[HUFFMAN_SYMBOLS] = {0};
    for (size_t i = 0; i < input_size; i++) {
        freq[input[i]]++;
    }

    FILE *out = fopen(output_path, "wb");
    if (out == NULL) {
        free(input);
        return -1;
    }
    if (write_header(out, input_size, freq) != 0) {
        free(input);
        fclose(out);
        return -1;
    }
    if (input_size == 0) {
        free(input);
        return fclose(out) == 0 ? 0 : -1;
    }

    huffman_node *root = huffman_build_tree(freq);
    if (root == NULL) {
        free(input);
        fclose(out);
        return -1;
    }

    huffman_code codes[HUFFMAN_SYMBOLS];
    huffman_build_codes(root, codes);

    if (input_size > (SIZE_MAX - 1) / 8) {
        huffman_free_tree(root);
        free(input);
        fclose(out);
        return -1;
    }
    uint8_t *encoded = malloc(input_size * 8 + 1);
    if (encoded == NULL) {
        huffman_free_tree(root);
        free(input);
        fclose(out);
        return -1;
    }

    encode_state state;
    int ok = encode_huffman(input, input_size, codes, encoded, &state) == 0;
    if (ok && state.output_size > 0 && fwrite(encoded, 1, state.output_size, out) != state.output_size) {
        ok = 0;
    }
    if (ok && state.tail_bits > 0 && fputc(state.tail, out) == EOF) {
        ok = 0;
    }

    free(encoded);
    huffman_free_tree(root);
    free(input);
    if (fclose(out) != 0) {
        ok = 0;
    }
    return ok ? 0 : -1;
}
