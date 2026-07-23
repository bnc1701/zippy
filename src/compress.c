#include "compress.h"
#include "huffman.h"
#include "bitio.h"
#include "serialize.h"
#include "format.h"

#include <stdio.h>
#include <stdlib.h>

// counts how many distinct symbols appear in the frequency table
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

// writes the magic bytes original size and the frequency table used to rebuild the tree
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
        if (freq[i] > 0) {
            uint8_t symbol = (uint8_t)i;
            if (fwrite(&symbol, 1, 1, out) != 1) {
                return -1;
            }
            if (write_u64(out, freq[i]) != 0) {
                return -1;
            }
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

    uint8_t *buffer = NULL;
    if (file_size > 0) {
        buffer = malloc((size_t)file_size);
        if (buffer == NULL) {
            fclose(in);
            return -1;
        }
        if (fread(buffer, 1, (size_t)file_size, in) != (size_t)file_size) {
            free(buffer);
            fclose(in);
            return -1;
        }
    }
    fclose(in);

    uint64_t freq[HUFFMAN_SYMBOLS] = {0};
    for (long i = 0; i < file_size; i++) {
        freq[buffer[i]]++;
    }

    FILE *out = fopen(output_path, "wb");
    if (out == NULL) {
        free(buffer);
        return -1;
    }

    if (write_header(out, (uint64_t)file_size, freq) != 0) {
        free(buffer);
        fclose(out);
        return -1;
    }

    // an empty input has nothing left to encode the header alone is enough
    if (file_size == 0) {
        free(buffer);
        fclose(out);
        return 0;
    }

    huffman_node *root = huffman_build_tree(freq);
    huffman_code codes[HUFFMAN_SYMBOLS];
    huffman_build_codes(root, codes);

    bit_writer bw;
    bit_writer_init(&bw, out);

    int ok = 1;
    for (long i = 0; i < file_size && ok; i++) {
        huffman_code code = codes[buffer[i]];
        for (int b = code.length - 1; b >= 0; b--) {
            int bit = (code.bits >> b) & 1;
            if (bit_writer_write_bit(&bw, bit) != 0) {
                ok = 0;
                break;
            }
        }
    }

    if (ok) {
        ok = bit_writer_flush(&bw) == 0;
    }

    huffman_free_tree(root);
    free(buffer);
    fclose(out);

    return ok ? 0 : -1;
}
