CC ?= gcc
AR ?= ar

OUTDIR = build
LIB = libtinyff.a

ALL_CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude

DEBUG_FLAGS = -g -O0 -fno-omit-frame-pointer
RELEASE_FLAGS = -O2
SANFLAGS = -fsanitize=address,undefined -g -O1

COVERAGE_FLAGS = -fprofile-instr-generate -fcoverage-mapping
COVERAGE_LDFLAGS = -fprofile-instr-generate -fcoverage-mapping

SRC = $(shell find src -name "*.c")

ifeq ($(USE_HOSTED),1)
ALL_CFLAGS += -DUSE_HOSTED -Iinclude/bridges
SRC += $(shell find src/bridges -name "*.c")
endif

ifeq ($(USE_BENCH),1)
ALL_CFLAGS += -DUSE_BENCH
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
	rm -rf $(OUTDIR) coverage coverage.info png_test bench_png

debug: CFLAGS = $(DEBUG_FLAGS)
debug: $(OUTDIR)/$(LIB)

release: CFLAGS = $(RELEASE_FLAGS)
release: $(OUTDIR)/$(LIB)

asan: CC = clang
asan: CFLAGS = $(SANFLAGS)
asan: clean $(OUTDIR)/$(LIB)

test-png: clean
	$(MAKE) USE_HOSTED=1 debug
	$(CC) $(ALL_CFLAGS) $(DEBUG_FLAGS) \
		-DUSE_HOSTED \
		tests/format/image/png/png_open.c \
		-o $(OUTDIR)/png_test \
		-L$(OUTDIR) -ltinyff
	./$(OUTDIR)/png_test

test: test-png

gdb: debug
	gdb -x debug/.gdbinit $(OUTDIR)/$(TEST)

bench: clean
	$(MAKE) USE_HOSTED=1 USE_BENCH=1 release
	$(CC) $(ALL_CFLAGS) $(RELEASE_FLAGS) \
		-DUSE_HOSTED -DUSE_BENCH \
		bench/src/format/image/png.c \
		-o $(OUTDIR)/bench_png \
		-L$(OUTDIR) -ltinyff
	./$(OUTDIR)/bench_png

coverage: clean
	$(MAKE) USE_HOSTED=1 CFLAGS="$(COVERAGE_FLAGS)" LDFLAGS="$(COVERAGE_LDFLAGS)" debug

	$(CC) $(ALL_CFLAGS) $(DEBUG_FLAGS) $(COVERAGE_FLAGS) \
		-DUSE_HOSTED \
		tests/format/image/png/png_open.c \
		-o $(OUTDIR)/png_test \
		-L$(OUTDIR) -ltinyff $(COVERAGE_LDFLAGS)

	./$(OUTDIR)/png_test || true

	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage.info --ignore-errors unused
	genhtml coverage.info --output-directory coverage

.PHONY: all clean debug release asan test test-png gdb bench coverage