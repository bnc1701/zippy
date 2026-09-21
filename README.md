# zippy

a small file compressor written in c with huffman coding

## features

- compress files with huffman coding
- decompress zippy files
- preserve the original file contents

## structure

```text
zippy/
├── src/
│   ├── main.c
│   ├── compress.c
│   ├── decompress.c
│   ├── huffman.c
│   ├── bitio.c
│   └── serialize.c
├── include/
│   ├── compress.h
│   ├── decompress.h
│   ├── huffman.h
│   ├── bitio.h
│   ├── serialize.h
│   └── format.h
├── tests/
├── makefile
├── readme.md
├── .gitignore
└── license
```

## build

```bash
make
```

## usage

```bash
./build/zippy -c input.txt output.zpy
./build/zippy -d output.zpy restored.txt
```

## tests

```bash
make test
```

## license

mit license
