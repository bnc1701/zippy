CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude -O2
LDFLAGS =

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests

TARGET = $(BUILD_DIR)/zippy

# main program sources, main.c is excluded from the shared object list
# used by tests since tests bring their own main function
LIB_SRC = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
LIB_OBJ = $(LIB_SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

MAIN_OBJ = $(BUILD_DIR)/main.o

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(MAIN_OBJ) $(LIB_OBJ)
	$(CC) $(MAIN_OBJ) $(LIB_OBJ) -o $(TARGET) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# each test file has its own main and links against the library objects

TEST_BINARIES = $(BUILD_DIR)/test_bitio $(BUILD_DIR)/test_huffman $(BUILD_DIR)/test_roundtrip

$(BUILD_DIR)/test_bitio: $(TEST_DIR)/test_bitio.c $(LIB_OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@

$(BUILD_DIR)/test_huffman: $(TEST_DIR)/test_huffman.c $(LIB_OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@

$(BUILD_DIR)/test_roundtrip: $(TEST_DIR)/test_roundtrip.c $(LIB_OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< $(LIB_OBJ) -o $@

test: $(TEST_BINARIES)
	@for bin in $(TEST_BINARIES); do \
		./$$bin || exit 1; \
	done

clean:
	rm -rf $(BUILD_DIR)
