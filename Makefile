CC ?= gcc
AR ?= ar

OUTDIR = dist
LIB = libtinyff.a

CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2
SANFLAGS = -fsanitize=address,undefined -g -O1

SRC = $(shell find src -name "*.c")

ifeq ($(USE_HOSTED),1)
SRC += $(wildcard bridges/*.c)
endif

OBJ = $(patsubst %.c,$(OUTDIR)/%.o,$(SRC))

all: $(OUTDIR)/$(LIB)

$(OUTDIR)/$(LIB): $(OBJ)
	mkdir -p $(OUTDIR)
	$(AR) rcs $@ $^

$(OUTDIR)/src/%.o: src/%.c
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

.PHONY: all clean debug release asan
