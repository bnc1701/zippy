#ifndef ZIPPY_BITIO_H
#define ZIPPY_BITIO_H

#include <stdio.h>
#include <stdint.h>

// writes single bits to a file buffered in a byte

typedef struct {
    FILE *fp;
    uint8_t buffer;
    int count;
} bit_writer;

typedef struct {
    FILE *fp;
    uint8_t buffer;
    int count;
} bit_reader;

void bit_writer_init(bit_writer *bw, FILE *fp);
int bit_writer_write_bit(bit_writer *bw, int bit);
int bit_writer_flush(bit_writer *bw);

void bit_reader_init(bit_reader *br, FILE *fp);
int bit_reader_read_bit(bit_reader *br);

#endif
