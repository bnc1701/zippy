#include "test_utils.h"
#include "bitio.h"

#include <stdio.h>
#include <stdlib.h>

int tests_run = 0;
int tests_failed = 0;

// writes a known bit pattern then reads it back and compares

static void test_write_then_read(void)
{
    const char *path = "test_bitio_tmp.bin";
    int pattern[] = {1, 0, 1, 1, 0, 0, 1, 0, 1, 1};
    int pattern_len = (int)(sizeof(pattern) / sizeof(pattern[0]));

    FILE *out = fopen(path, "wb");
    CHECK(out != NULL, "open file for writing bits");

    bit_writer bw;
    bit_writer_init(&bw, out);
    for (int i = 0; i < pattern_len; i++) {
        bit_writer_write_bit(&bw, pattern[i]);
    }
    bit_writer_flush(&bw);
    fclose(out);

    FILE *in = fopen(path, "rb");
    CHECK(in != NULL, "open file for reading bits");

    bit_reader br;
    bit_reader_init(&br, in);

    int mismatch = 0;
    for (int i = 0; i < pattern_len; i++) {
        int bit = bit_reader_read_bit(&br);
        if (bit != pattern[i]) {
            mismatch = 1;
        }
    }
    CHECK(mismatch == 0, "bits read back match bits written");

    fclose(in);
    remove(path);
}

// a full byte should round trip cleanly with no padding artifacts

static void test_full_byte(void)
{
    const char *path = "test_bitio_byte.bin";

    FILE *out = fopen(path, "wb");
    bit_writer bw;
    bit_writer_init(&bw, out);
    int bits[] = {1, 1, 0, 0, 1, 0, 1, 0};
    for (int i = 0; i < 8; i++) {
        bit_writer_write_bit(&bw, bits[i]);
    }
    bit_writer_flush(&bw);
    fclose(out);

    FILE *in = fopen(path, "rb");
    int value = fgetc(in);
    CHECK(value == 0xCA, "packed byte matches expected value");
    fclose(in);
    remove(path);
}

int main(void)
{
    test_write_then_read();
    test_full_byte();

    printf("bitio: %d run, %d failed\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
