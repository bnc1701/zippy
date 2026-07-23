# zippy

a file compressor written in C using huffman coding

## how it works

zippy reads the input file and counts how often each byte value appears.
it then builds a huffman tree from those frequencies, which assigns
shorter bit codes to common bytes and longer bit codes to rare bytes.
the file is rewritten as a stream of these codes, which is smaller
whenever the byte distribution is uneven, such as in text files.

the compressed file stores the original size and the frequency table
needed to rebuild the exact same tree, so decompression is lossless.

## project structure

```
zippy/
├── src/            source files
│   ├── main.c          command line interface
│   ├── compress.c      compression pipeline
│   ├── decompress.c    decompression pipeline
│   ├── huffman.c       tree construction and code generation
│   ├── bitio.c         bit level file reading and writing
│   └── serialize.c     little endian integer read and write helpers
├── include/        public headers for each module
├── tests/          unit and end to end tests
├── Makefile
├── LICENSE
└── README.md
```

## building

requires gcc and make.

```bash
make
```

this produces the binary at `build/zippy`.

## usage

compress a file:
```bash
./build/zippy -c input.txt input.zpy
```

decompress a file:
```bash
./build/zippy -d input.zpy restored.txt
```

## running tests

```bash
make test
```

this builds and runs three test binaries:
- `test_bitio` checks the bit level reader and writer
- `test_huffman` checks tree construction, code generation and the prefix free property
- `test_roundtrip` compresses and decompresses several inputs, including an empty file, a single byte, a file with only one repeated symbol, plain text, all 256 byte values and pseudo random data, and confirms the restored file always matches the original exactly

all tests have also been run under gcc's address and undefined behavior
sanitizers with no leaks or errors reported.

## file format

```
4 bytes    magic "ZPY1"
8 bytes    original file size
2 bytes    number of distinct byte values in the file
per symbol  1 byte value plus 8 bytes frequency count
remainder  the huffman encoded bit stream
```

## limitations

the current implementation reads the whole input file into memory, so it
is best suited for files that comfortably fit in ram rather than very
large files.

## license

MIT, see [LICENSE](LICENSE)
