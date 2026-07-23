#ifndef ZIPPY_DECOMPRESS_H
#define ZIPPY_DECOMPRESS_H

// decompresses input_path into output_path
// returns zero on success negative one on failure

int zippy_decompress(const char *input_path, const char *output_path);

#endif
