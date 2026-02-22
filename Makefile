CC=gcc
AR=ar
CFLAGS=-Wall -Wextra -std=c11 -O2 -Iinclude -Iinclude/tinyff -Iinclude/ext

SRC=$(shell find src -name "*.c")
OBJ=$(patsubst src/%.c,$(OUTDIR)/src/%.o,$(SRC))

LIB=libtinyff.a
OUTDIR=dist

all: $(OUTDIR)/$(LIB)

$(OUTDIR)/$(LIB): $(OBJ)
	@mkdir -p $(OUTDIR)
	$(AR) rcs $@ $^

$(OUTDIR)/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTDIR)

.PHONY: all clean
