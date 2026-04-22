CC ?= gcc
AR ?= ar

OUTDIR = dist
LIB = libtinyff.a

INCLUDES = -Iinclude

BASE_CFLAGS = -Wall -Wextra -Werror -std=c11 $(INCLUDES)
DEBUG_FLAGS = -g -O0 -fno-omit-frame-pointer
RELEASE_FLAGS = -O2
SANFLAGS = -fsanitize=address,undefined -g -O1

SRC = $(shell find src -name "*.c")

ifeq ($(USE_HOSTED),1)
INCLUDES += -Iinclude/bridges
BASE_CFLAGS += -DUSE_HOSTED
SRC += $(shell find src/bridges -name "*.c")
endif

OBJ = $(patsubst %.c,$(OUTDIR)/%.o,$(SRC))

all: release

$(OUTDIR)/$(LIB): $(OBJ)
	mkdir -p $(OUTDIR)
	$(AR) rcs $@ $^

$(OUTDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTDIR) png_test

debug: CFLAGS = $(BASE_CFLAGS) $(DEBUG_FLAGS)
debug: $(OUTDIR)/$(LIB)

release: CFLAGS = $(BASE_CFLAGS) $(RELEASE_FLAGS)
release: $(OUTDIR)/$(LIB)

asan: CC = clang
asan: CFLAGS = $(BASE_CFLAGS) $(SANFLAGS)
asan: clean $(OUTDIR)/$(LIB)

test-png: clean
	$(MAKE) USE_HOSTED=1 debug
	$(CC) $(BASE_CFLAGS) $(DEBUG_FLAGS) -DUSE_HOSTED tests/format/image/png/png_open.c -o png_test -Iinclude/bridges -Ldist -ltinyff && ./png_test

test: test-png

# $(TEST) is passed in via `make gdb TEST=<test_name>`
# NOTE: <test_name> should be the name of the test target's output binary
#       inside the Makefile but cannot be the more generic 'test' target
# 
# 
# 
# WARNING: TEST does not have a default value

gdb: debug
	gdb -x debug/.gdbinit $(OUTDIR)/$(TEST)

.PHONY: all clean debug release asan test test-png gdb