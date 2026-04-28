CC ?= gcc
AR ?= ar

OUTDIR = build
BENCH_OUT = $(OUTDIR)/bench
LIB = libtinyff.a

ALL_CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude -Ibench

DEBUG_FLAGS = -g -O0 -fno-omit-frame-pointer
RELEASE_FLAGS = -O2
SANFLAGS = -fsanitize=address,undefined -g -O1

SRC = $(shell find src -name "*.c")
BENCH_SRC = $(shell find bench/src -name "*.c")

ifeq ($(USE_HOSTED),1)
ALL_CFLAGS += -DUSE_HOSTED -Iinclude/bridges
SRC += $(shell find src/bridges -name "*.c")
endif

OBJ = $(patsubst %.c,$(OUTDIR)/%.o,$(SRC))

all: release

$(OUTDIR)/$(LIB): $(OBJ)
	mkdir -p $(OUTDIR)
	$(AR) rcs $@ $^

$(OUTDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTDIR) png_test

debug: CFLAGS = $(DEBUG_FLAGS)
debug: $(OUTDIR)/$(LIB)

release: CFLAGS = $(RELEASE_FLAGS)
release: $(OUTDIR)/$(LIB)

asan: CC = clang
asan: CFLAGS = $(SANFLAGS)
asan: clean $(OUTDIR)/$(LIB)

test-png: clean
	$(MAKE) USE_HOSTED=1 debug
	$(CC) $(ALL_CFLAGS) $(DEBUG_FLAGS) -DUSE_HOSTED tests/format/image/png/png_open.c -o $(OUTDIR)/png_test -L$(OUTDIR) -ltinyff && ./$(OUTDIR)/png_test

test: test-png

gdb: debug
	gdb -x debug/.gdbinit $(OUTDIR)/$(TEST)

bench: $(OUTDIR)/$(LIB)
	mkdir -p $(BENCH_OUT)
	$(CC) $(ALL_CFLAGS) $(RELEASE_FLAGS) $(BENCH_SRC) -o $(BENCH_OUT)/bench -L$(OUTDIR) -ltinyff
	$(BENCH_OUT)/bench

.PHONY: all clean debug release asan test test-png gdb bench