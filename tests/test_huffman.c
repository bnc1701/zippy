#include "test_utils.h"
#include "huffman.h"

#include <stdio.h>

int tests_run = 0;
int tests_failed = 0;

// every symbol that appears should get a code and no code should be empty

static void test_all_used_symbols_get_codes(void)
{
    uint64_t freq[HUFFMAN_SYMBOLS] = {0};
    freq['a'] = 5;
    freq['b'] = 2;
    freq['c'] = 1;
    freq['d'] = 1;

    huffman_node *root = huffman_build_tree(freq);
    huffman_code codes[HUFFMAN_SYMBOLS];
    huffman_build_codes(root, codes);

    CHECK(codes['a'].length > 0, "symbol a has a code");
    CHECK(codes['b'].length > 0, "symbol b has a code");
    CHECK(codes['c'].length > 0, "symbol c has a code");
    CHECK(codes['d'].length > 0, "symbol d has a code");
    CHECK(codes['z'].length == 0, "unused symbol has no code");

    // the most frequent symbol should never be longer than a rarer one
    CHECK(codes['a'].length <= codes['c'].length, "frequent symbol is not longer than rare symbol");

    huffman_free_tree(root);
}

// a file with a single distinct symbol still needs a usable one bit code

static void test_single_symbol(void)
{
    uint64_t freq[HUFFMAN_SYMBOLS] = {0};
    freq['x'] = 10;

    huffman_node *root = huffman_build_tree(freq);
    huffman_code codes[HUFFMAN_SYMBOLS];
    huffman_build_codes(root, codes);

    CHECK(codes['x'].length >= 1, "single symbol still gets a code");

    huffman_free_tree(root);
}

// no code should ever be the exact prefix of another or decoding would be ambiguous

static void test_prefix_free(void)
{
    uint64_t freq[HUFFMAN_SYMBOLS] = {0};
    freq['a'] = 45;
    freq['b'] = 13;
    freq['c'] = 12;
    freq['d'] = 16;
    freq['e'] = 9;
    freq['f'] = 5;

    huffman_node *root = huffman_build_tree(freq);
    huffman_code codes[HUFFMAN_SYMBOLS];
    huffman_build_codes(root, codes);

    const char *symbols = "abcdef";
    int violation = 0;

    for (int i = 0; symbols[i] != '\0'; i++) {
        for (int j = 0; symbols[j] != '\0'; j++) {
            if (i == j) {
                continue;
            }
            huffman_code short_code = codes[(int)symbols[i]];
            huffman_code long_code = codes[(int)symbols[j]];
            if (short_code.length == 0 || long_code.length == 0) {
                continue;
            }
            if (short_code.length >= long_code.length) {
                continue;
            }
            uint32_t shifted = long_code.bits >> (long_code.length - short_code.length);
            if (shifted == short_code.bits) {
                violation = 1;
            }
        }
    }

    CHECK(violation == 0, "no code is a prefix of another code");

    huffman_free_tree(root);
}

int main(void)
{
    test_all_used_symbols_get_codes();
    test_single_symbol();
    test_prefix_free();

    printf("huffman: %d run, %d failed\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
