#ifndef ZIPPY_COMPRESS_H
#define ZIPPY_COMPRESS_H

// compresses input_path into output_path
// returns zero on success negative one on failure

int zippy_compress(const char *input_path, const char *output_path);

#endif
