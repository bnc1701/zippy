#include "test_utils.h"
#include "compress.h"
#include "decompress.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tests_run = 0;
int tests_failed = 0;

static void write_file(const char *path, const unsigned char *data, size_t len)
{
    FILE *fp = fopen(path, "wb");
    if (len > 0) {
        size_t written = fwrite(data, 1, len, fp);
        (void)written;
    }
    fclose(fp);
}

// reads a whole file into a malloc buffer sets out_len returns null on failure

static unsigned char *read_file(const char *path, size_t *out_len)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    unsigned char *buffer = malloc((size_t)size > 0 ? (size_t)size : 1);
    if (size > 0) {
        size_t read_bytes = fread(buffer, 1, (size_t)size, fp);
        if (read_bytes != (size_t)size) {
            fclose(fp);
            free(buffer);
            return NULL;
        }
    }
    fclose(fp);

    *out_len = (size_t)size;
    return buffer;
}

// compresses then decompresses a buffer and checks the result matches the original

static void run_roundtrip_case(const char *name, const unsigned char *data, size_t len)
{
    const char *original_path = "test_original_tmp.bin";
    const char *compressed_path = "test_compressed_tmp.bin";
    const char *restored_path = "test_restored_tmp.bin";

    write_file(original_path, data, len);

    int compress_result = zippy_compress(original_path, compressed_path);
    CHECK(compress_result == 0, "compress succeeds");

    int decompress_result = zippy_decompress(compressed_path, restored_path);
    CHECK(decompress_result == 0, "decompress succeeds");

    size_t restored_len = 0;
    unsigned char *restored = read_file(restored_path, &restored_len);
    CHECK(restored != NULL, "restored file can be read");

    if (restored != NULL) {
        int same_len = restored_len == len;
        CHECK(same_len, name);

        if (same_len) {
            int same_content = len == 0 || memcmp(data, restored, len) == 0;
            CHECK(same_content, name);
        }

        free(restored);
    }

    remove(original_path);
    remove(compressed_path);
    remove(restored_path);
}

static void test_empty_file(void)
{
    run_roundtrip_case("empty file round trips", NULL, 0);
}

static void test_single_byte(void)
{
    unsigned char data[] = {'a'};
    run_roundtrip_case("single byte round trips", data, sizeof(data));
}

static void test_single_repeated_symbol(void)
{
    unsigned char data[500];
    memset(data, 'q', sizeof(data));
    run_roundtrip_case("single repeated symbol round trips", data, sizeof(data));
}

static void test_text(void)
{
    const char *text =
        "the quick brown fox jumps over the lazy dog "
        "the quick brown fox jumps over the lazy dog "
        "pack my box with five dozen liquor jugs";
    run_roundtrip_case("plain text round trips", (const unsigned char *)text, strlen(text));
}

static void test_binary_data(void)
{
    unsigned char data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = (unsigned char)i;
    }
    run_roundtrip_case("all byte values round trip", data, sizeof(data));
}

static void test_pseudo_random_data(void)
{
    unsigned char data[4096];
    unsigned int seed = 12345;
    for (size_t i = 0; i < sizeof(data); i++) {
        seed = seed * 1103515245u + 12345u;
        data[i] = (unsigned char)(seed >> 16);
    }
    run_roundtrip_case("pseudo random data round trips", data, sizeof(data));
}

int main(void)
{
    test_empty_file();
    test_single_byte();
    test_single_repeated_symbol();
    test_text();
    test_binary_data();
    test_pseudo_random_data();

    printf("roundtrip: %d run, %d failed\n", tests_run, tests_failed);
    return tests_failed == 0 ? 0 : 1;
}
