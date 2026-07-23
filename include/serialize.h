#ifndef ZIPPY_SERIALIZE_H
#define ZIPPY_SERIALIZE_H

#include <stdio.h>
#include <stdint.h>

// little endian helpers so the file format is portable across machines

int write_u16(FILE *fp, uint16_t value);
int write_u64(FILE *fp, uint64_t value);
int read_u16(FILE *fp, uint16_t *value);
int read_u64(FILE *fp, uint64_t *value);

#endif
