#include "serialize.h"

int write_u16(FILE *fp, uint16_t value)
{
    uint8_t bytes[2];
    bytes[0] = (uint8_t)(value & 0xff);
    bytes[1] = (uint8_t)((value >> 8) & 0xff);

    return fwrite(bytes, 1, 2, fp) == 2 ? 0 : -1;
}

int write_u64(FILE *fp, uint64_t value)
{
    uint8_t bytes[8];
    for (int i = 0; i < 8; i++) {
        bytes[i] = (uint8_t)((value >> (8 * i)) & 0xff);
    }

    return fwrite(bytes, 1, 8, fp) == 8 ? 0 : -1;
}

int read_u16(FILE *fp, uint16_t *value)
{
    uint8_t bytes[2];
    if (fread(bytes, 1, 2, fp) != 2) {
        return -1;
    }

    *value = (uint16_t)(bytes[0] | (bytes[1] << 8));
    return 0;
}

int read_u64(FILE *fp, uint64_t *value)
{
    uint8_t bytes[8];
    if (fread(bytes, 1, 8, fp) != 8) {
        return -1;
    }

    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result |= ((uint64_t)bytes[i]) << (8 * i);
    }

    *value = result;
    return 0;
}
