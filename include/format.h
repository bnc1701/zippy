#ifndef ZIPPY_FORMAT_H
#define ZIPPY_FORMAT_H

// file layout
// 4 bytes    magic bytes zpy1
// 8 bytes    original file size
// 2 bytes    number of distinct symbols
// per symbol 1 byte symbol plus 8 bytes frequency
// remainder  huffman encoded bit stream

#define ZIPPY_MAGIC "ZPY1"
#define ZIPPY_MAGIC_LEN 4

#endif
