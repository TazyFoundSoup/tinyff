CC ?= gcc
AR ?= ar

OUTDIR = dist
LIB = libtinyff.a

INCLUDES = -Iinclude
CFLAGS = -Wall -Wextra -Werror -std=c11 $(INCLUDES)

DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2
SANFLAGS = -fsanitize=address,undefined -g -O1

SRC = $(shell find src -name "*.c")

ifeq ($(USE_HOSTED),1)
INCLUDES += -Iinclude/bridges
CFLAGS += -DUSE_HOSTED
SRC += $(shell find src/bridges -name "*.c")
endif

OBJ = $(patsubst %.c,$(OUTDIR)/%.o,$(SRC))

all: $(OUTDIR)/$(LIB)

$(OUTDIR)/$(LIB): $(OBJ)
	mkdir -p $(OUTDIR)
	$(AR) rcs $@ $^

$(OUTDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTDIR)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all

release: CFLAGS += $(RELEASE_FLAGS)
release: clean all

asan: CC=clang
asan: CFLAGS += $(SANFLAGS)
asan: clean all

# Currently supporting:
# Images
# - png
# - bmp (WIP)

test-png: clean
	$(MAKE) USE_HOSTED=1
	$(CC) $(CFLAGS) -DUSE_HOSTED tests/format/image/png/png_open.c -o png_test -Iinclude/bridges -Ldist -ltinyff && ./png_test

test: test-png

.PHONY: all clean debug release asan test test-png
