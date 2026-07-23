#ifndef ZIPPY_HUFFMAN_H
#define ZIPPY_HUFFMAN_H

#include <stdint.h>
#include <stddef.h>

#define HUFFMAN_SYMBOLS 256

// tree node used both while building the tree and while walking it

typedef struct huffman_node {
    int symbol;
    uint64_t freq;
    struct huffman_node *left;
    struct huffman_node *right;
} huffman_node;

// one entry of the generated code table

typedef struct {
    uint32_t bits;
    int length;
} huffman_code;

huffman_node *huffman_build_tree(const uint64_t freq[HUFFMAN_SYMBOLS]);
void huffman_free_tree(huffman_node *root);

void huffman_build_codes(const huffman_node *root, huffman_code codes[HUFFMAN_SYMBOLS]);

#endif
