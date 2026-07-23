#include "decompress.h"
#include "huffman.h"
#include "bitio.h"
#include "serialize.h"
#include "format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// reads and validates the header filling original_size and the frequency table
static int read_header(FILE *in, uint64_t *original_size, uint64_t freq[HUFFMAN_SYMBOLS])
{
    char magic[ZIPPY_MAGIC_LEN];
    if (fread(magic, 1, ZIPPY_MAGIC_LEN, in) != ZIPPY_MAGIC_LEN) {
        return -1;
    }
    if (memcmp(magic, ZIPPY_MAGIC, ZIPPY_MAGIC_LEN) != 0) {
        return -1;
    }

    if (read_u64(in, original_size) != 0) {
        return -1;
    }

    uint16_t distinct;
    if (read_u16(in, &distinct) != 0) {
        return -1;
    }

    for (int i = 0; i < HUFFMAN_SYMBOLS; i++) {
        freq[i] = 0;
    }

    for (uint16_t i = 0; i < distinct; i++) {
        uint8_t symbol;
        if (fread(&symbol, 1, 1, in) != 1) {
            return -1;
        }
        uint64_t count;
        if (read_u64(in, &count) != 0) {
            return -1;
        }
        freq[symbol] = count;
    }

    return 0;
}

int zippy_decompress(const char *input_path, const char *output_path)
{
    FILE *in = fopen(input_path, "rb");
    if (in == NULL) {
        return -1;
    }

    uint64_t original_size = 0;
    uint64_t freq[HUFFMAN_SYMBOLS];

    if (read_header(in, &original_size, freq) != 0) {
        fclose(in);
        return -1;
    }

    FILE *out = fopen(output_path, "wb");
    if (out == NULL) {
        fclose(in);
        return -1;
    }

    if (original_size == 0) {
        fclose(in);
        fclose(out);
        return 0;
    }

    huffman_node *root = huffman_build_tree(freq);
    if (root == NULL) {
        fclose(in);
        fclose(out);
        return -1;
    }

    bit_reader br;
    bit_reader_init(&br, in);

    int ok = 1;
    for (uint64_t decoded = 0; decoded < original_size; decoded++) {
        const huffman_node *node = root;

        // a single distinct symbol produces a one node tree with no children
        if (node->left == NULL && node->right == NULL) {
            if (fputc(node->symbol, out) == EOF) {
                ok = 0;
                break;
            }
            continue;
        }

        while (node->left != NULL || node->right != NULL) {
            int bit = bit_reader_read_bit(&br);
            if (bit < 0) {
                ok = 0;
                break;
            }
            node = bit ? node->right : node->left;
        }

        if (!ok) {
            break;
        }

        if (fputc(node->symbol, out) == EOF) {
            ok = 0;
            break;
        }
    }

    huffman_free_tree(root);
    fclose(in);
    fclose(out);

    return ok ? 0 : -1;
}
