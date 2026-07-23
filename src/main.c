#include "compress.h"
#include "decompress.h"

#include <stdio.h>
#include <string.h>

static void print_usage(const char *program_name)
{
    fprintf(stderr, "usage: %s -c input output   compress a file\n", program_name);
    fprintf(stderr, "       %s -d input output   decompress a file\n", program_name);
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    const char *input_path = argv[2];
    const char *output_path = argv[3];

    if (strcmp(mode, "-c") == 0) {
        if (zippy_compress(input_path, output_path) != 0) {
            fprintf(stderr, "error: failed to compress %s\n", input_path);
            return 1;
        }
        printf("compressed %s into %s\n", input_path, output_path);
        return 0;
    }

    if (strcmp(mode, "-d") == 0) {
        if (zippy_decompress(input_path, output_path) != 0) {
            fprintf(stderr, "error: failed to decompress %s\n", input_path);
            return 1;
        }
        printf("decompressed %s into %s\n", input_path, output_path);
        return 0;
    }

    print_usage(argv[0]);
    return 1;
}
