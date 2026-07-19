CC ?= gcc
AR ?= ar

OUTDIR = build
LIB = libtinyff.a
LIB_SHARED = libtinyff.so

PREFIX ?= /usr/local
INCLUDE_DIR ?= $(PREFIX)/include/tinyff
LIB_DIR ?= $(PREFIX)/lib

ALL_CFLAGS = -Wall -Wextra -Werror -std=c99 -Iinclude
SHARED_FLAGS = -fPIC -shared

DEBUG_FLAGS = -g -O0 -fno-omit-frame-pointer
RELEASE_FLAGS = -O3 -march=native -flto
SANFLAGS = -fsanitize=address,undefined -g -O1

COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
COVERAGE_LDFLAGS = -fprofile-arcs -ftest-coverage

SRC = $(shell find src -name "*.c")
TEST = png_test

ifeq ($(USE_ALL),1)
USE_HOSTED := 1
USE_BENCH := 1
endif

ifeq ($(USE_HOSTED),1)
ALL_CFLAGS += -DUSE_HOSTED -Iinclude/bridges
SRC += $(shell find src/bridges -name "*.c")
endif

ifeq ($(USE_BENCH),1)
ALL_CFLAGS += -DUSE_BENCH
endif

ifneq ($(SUDO_USER),)
	__SUDO = true
else
	__SUDO = false
endif

OBJ = $(patsubst %.c,$(OUTDIR)/%.o,$(SRC))

all: release

$(OUTDIR)/$(LIB): $(OBJ)
	mkdir -p $(OUTDIR)
	$(AR) rcs $@ $^

$(OUTDIR)/$(LIB_SHARED): $(OBJ)
	mkdir -p $(OUTDIR)
	$(CC) $(SHARED_FLAGS) -o $@ $^

$(OUTDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(ALL_CFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTDIR) coverage coverage.info png_test bench_png

debug: CFLAGS = $(DEBUG_FLAGS)
debug: $(OUTDIR)/$(LIB)

release: CFLAGS = $(RELEASE_FLAGS)
release: $(OUTDIR)/$(LIB)

shared:
	$(MAKE) clean
	$(MAKE) USE_ALL=1 CFLAGS="$(RELEASE_FLAGS) $(SHARED_FLAGS)" $(OUTDIR)/$(LIB_SHARED)

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
	gdb -x scripts/.gdbinit $(OUTDIR)/$(TEST)

bench: clean
	$(MAKE) USE_HOSTED=1 USE_BENCH=1 release
	$(CC) $(ALL_CFLAGS) $(RELEASE_FLAGS) \
		-DUSE_HOSTED -DUSE_BENCH \
		bench/src/format/image/png.c \
		-o $(OUTDIR)/bench_png \
		-L$(OUTDIR) -ltinyff
	./$(OUTDIR)/bench_png

# TODO: Make coverage for every format test (maybe change sum syntax or do sum magic. future watson knows more than current watson does now)
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

format:
	clang-format -i $(SRC) $(shell find include tests -name "*.h" -o -name "*.c")

install:
	$(MAKE) clean
	$(MAKE) USE_ALL=1 shared
	$(MAKE) USE_ALL=1 release
ifeq ($(__SUDO),false)
		@echo "'make install' requires root priviledges via sudo! aborting."
		@exit 1
endif
	install -d $(DESTDIR)$(INCLUDE_DIR)
	install -d $(DESTDIR)$(LIB_DIR)

	cp -R include/.  $(DESTDIR)$(INCLUDE_DIR)/
	chmod -R a+r $(DESTDIR)$(INCLUDE_DIR)

	install -m 0644 $(OUTDIR)/$(LIB) $(DESTDIR)$(LIB_DIR)/
	install -m 0755 $(OUTDIR)/$(LIB_SHARED) $(DESTDIR)$(LIB_DIR)/
	ldconfig

.PHONY: all clean debug release shared asan test test-png gdb bench coverage format install
