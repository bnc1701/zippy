#include "huffman.h"
#include <stdlib.h>

// simple binary min heap ordered by node frequency

typedef struct {
    huffman_node **items;
    int size;
    int capacity;
} min_heap;

static min_heap *heap_create(int capacity)
{
    min_heap *h = malloc(sizeof(min_heap));
    h->items = malloc(sizeof(huffman_node *) * (size_t)capacity);
    h->size = 0;
    h->capacity = capacity;
    return h;
}

static void heap_destroy(min_heap *h)
{
    free(h->items);
    free(h);
}

static void heap_swap(huffman_node **a, huffman_node **b)
{
    huffman_node *tmp = *a;
    *a = *b;
    *b = tmp;
}

// moves an out of place node up towards the root
static void heap_sift_up(min_heap *h, int i)
{
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->items[parent]->freq <= h->items[i]->freq) {
            break;
        }
        heap_swap(&h->items[parent], &h->items[i]);
        i = parent;
    }
}

// moves an out of place node down towards the leaves
static void heap_sift_down(min_heap *h, int i)
{
    for (;;) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < h->size && h->items[left]->freq < h->items[smallest]->freq) {
            smallest = left;
        }
        if (right < h->size && h->items[right]->freq < h->items[smallest]->freq) {
            smallest = right;
        }
        if (smallest == i) {
            break;
        }

        heap_swap(&h->items[i], &h->items[smallest]);
        i = smallest;
    }
}

static void heap_push(min_heap *h, huffman_node *node)
{
    h->items[h->size] = node;
    heap_sift_up(h, h->size);
    h->size++;
}

static huffman_node *heap_pop(min_heap *h)
{
    huffman_node *top = h->items[0];
    h->size--;
    h->items[0] = h->items[h->size];
    heap_sift_down(h, 0);
    return top;
}

static huffman_node *node_create(int symbol, uint64_t freq, huffman_node *left, huffman_node *right)
{
    huffman_node *node = malloc(sizeof(huffman_node));
    node->symbol = symbol;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}

// builds the huffman tree from symbol frequencies using a min heap
huffman_node *huffman_build_tree(const uint64_t freq[HUFFMAN_SYMBOLS])
{
    min_heap *h = heap_create(HUFFMAN_SYMBOLS);

    for (int i = 0; i < HUFFMAN_SYMBOLS; i++) {
        if (freq[i] > 0) {
            heap_push(h, node_create(i, freq[i], NULL, NULL));
        }
    }

    // edge case a file with a single distinct symbol still needs a real tree
    if (h->size == 1) {
        huffman_node *only = heap_pop(h);
        heap_push(h, node_create(-1, 0, only, NULL));
    }

    while (h->size > 1) {
        huffman_node *a = heap_pop(h);
        huffman_node *b = heap_pop(h);
        huffman_node *parent = node_create(-1, a->freq + b->freq, a, b);
        heap_push(h, parent);
    }

    huffman_node *root = h->size > 0 ? heap_pop(h) : NULL;
    heap_destroy(h);

    return root;
}

void huffman_free_tree(huffman_node *root)
{
    if (root == NULL) {
        return;
    }
    huffman_free_tree(root->left);
    huffman_free_tree(root->right);
    free(root);
}

// walks the tree recording the path taken to reach each leaf symbol
static void walk_tree(const huffman_node *node, uint32_t bits, int length, huffman_code codes[HUFFMAN_SYMBOLS])
{
    if (node == NULL) {
        return;
    }

    if (node->left == NULL && node->right == NULL) {
        // a lone symbol at the root still needs at least one bit
        codes[node->symbol].bits = bits;
        codes[node->symbol].length = length > 0 ? length : 1;
        return;
    }

    walk_tree(node->left, bits << 1, length + 1, codes);
    walk_tree(node->right, (bits << 1) | 1, length + 1, codes);
}

void huffman_build_codes(const huffman_node *root, huffman_code codes[HUFFMAN_SYMBOLS])
{
    for (int i = 0; i < HUFFMAN_SYMBOLS; i++) {
        codes[i].bits = 0;
        codes[i].length = 0;
    }

    walk_tree(root, 0, 0, codes);
}
