#include "bitio.h"

void bit_writer_init(bit_writer *bw, FILE *fp)
{
    bw->fp = fp;
    bw->buffer = 0;
    bw->count = 0;
}

// pushes a single bit into the buffer and flushes to file when full
int bit_writer_write_bit(bit_writer *bw, int bit)
{
    bw->buffer = (uint8_t)((bw->buffer << 1) | (bit & 1));
    bw->count++;

    if (bw->count == 8) {
        if (fputc(bw->buffer, bw->fp) == EOF) {
            return -1;
        }
        bw->buffer = 0;
        bw->count = 0;
    }

    return 0;
}

// pads remaining bits with zero and writes the last partial byte
int bit_writer_flush(bit_writer *bw)
{
    if (bw->count > 0) {
        bw->buffer = (uint8_t)(bw->buffer << (8 - bw->count));
        if (fputc(bw->buffer, bw->fp) == EOF) {
            return -1;
        }
        bw->buffer = 0;
        bw->count = 0;
    }

    return 0;
}

void bit_reader_init(bit_reader *br, FILE *fp)
{
    br->fp = fp;
    br->buffer = 0;
    br->count = 0;
}

// returns the next bit or negative one on end of file
int bit_reader_read_bit(bit_reader *br)
{
    if (br->count == 0) {
        int c = fgetc(br->fp);
        if (c == EOF) {
            return -1;
        }
        br->buffer = (uint8_t)c;
        br->count = 8;
    }

    br->count--;
    return (br->buffer >> br->count) & 1;
}
