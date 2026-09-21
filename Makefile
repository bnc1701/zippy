cc = gcc
cflags = -Wall -Wextra -Werror -std=c11 -Iinclude -O2
src_dir = src
build_dir = build
test_dir = tests
bin = $(build_dir)/zippy
lib_src = $(filter-out $(src_dir)/main.c,$(wildcard $(src_dir)/*.c))
lib_obj = $(lib_src:$(src_dir)/%.c=$(build_dir)/%.o)
main_obj = $(build_dir)/main.o
asm_obj =

ifeq ($(shell uname -m),x86_64)
asm_obj = $(build_dir)/encode_x86_64.o
endif

test_bins = $(build_dir)/test_bitio $(build_dir)/test_encode $(build_dir)/test_huffman $(build_dir)/test_roundtrip

.PHONY: all clean test

all: $(bin)

$(bin): $(main_obj) $(lib_obj) $(asm_obj)
	$(cc) $(main_obj) $(lib_obj) $(asm_obj) -o $(bin)

$(build_dir)/%.o: $(src_dir)/%.c
	@mkdir -p $(build_dir)
	$(cc) $(cflags) -c $< -o $@

$(build_dir)/%.o: $(src_dir)/%.S
	@mkdir -p $(build_dir)
	$(cc) -c $< -o $@

$(build_dir)/test_bitio: $(test_dir)/test_bitio.c $(lib_obj) $(asm_obj)
	@mkdir -p $(build_dir)
	$(cc) $(cflags) $< $(lib_obj) $(asm_obj) -o $@

$(build_dir)/test_encode: $(test_dir)/test_encode.c $(lib_obj) $(asm_obj)
	@mkdir -p $(build_dir)
	$(cc) $(cflags) $< $(lib_obj) $(asm_obj) -o $@

$(build_dir)/test_huffman: $(test_dir)/test_huffman.c $(lib_obj) $(asm_obj)
	@mkdir -p $(build_dir)
	$(cc) $(cflags) $< $(lib_obj) $(asm_obj) -o $@

$(build_dir)/test_roundtrip: $(test_dir)/test_roundtrip.c $(lib_obj) $(asm_obj)
	@mkdir -p $(build_dir)
	$(cc) $(cflags) $< $(lib_obj) $(asm_obj) -o $@

test: $(test_bins)
	@for bin in $(test_bins); do \
		./$$bin || exit 1; \
	done

clean:
	rm -rf $(build_dir)
