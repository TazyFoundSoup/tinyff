CC ?= gcc
AR ?= ar

OUTDIR = build
BENCH_OUT = $(OUTDIR)/bench
LIB = libtinyff.a

ALL_CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude --coverage

DEBUG_FLAGS = -g -O0 -fno-omit-frame-pointer
RELEASE_FLAGS = -O2
SANFLAGS = -fsanitize=address,undefined -g -O1

SRC = $(shell find src -name "*.c")
BENCH_SRC = $(shell find bench/src -name "*.c")

ifeq ($(USE_HOSTED),1)
ALL_CFLAGS += -DUSE_HOSTED -Iinclude/bridges
SRC += $(shell find src/bridges -name "*.c")
endif

ifeq ($(USE_BENCH),1)
ALL_CFLAGS += -DUSE_BENCH -Ibench
SRC += $(shell find bench/src -name "*.c")
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
	rm -rf $(OUTDIR) png_test coverage coverage.info .coverage

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

coverage: clean
	$(MAKE) USE_HOSTED=1 debug
	$(CC) $(ALL_CFLAGS) $(DEBUG_FLAGS) -DUSE_HOSTED tests/format/image/png/png_open.c -o $(OUTDIR)/png_test -L$(OUTDIR) -ltinyff
	./$(OUTDIR)/png_test || true
	lcov --capture --directory . --output-file coverage.info
	lcov --remove coverage.info '/usr/*' --output-file coverage.info --ignore-errors unused
	genhtml coverage.info --output-directory coverage
	lcov --list coverage.info
	@echo ""
	@echo "HTML report generated at coverage/index.html"
	@echo "Opening with browser..."
	xdg-open coverage/index.html
.PHONY: all clean debug release asan test test-png gdb bench coverage