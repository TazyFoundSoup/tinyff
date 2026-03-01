CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -O2 \
	-I../include \
	-I../include/tinyff \
	-I../include/ext

OUTDIR=dist
TARGET=$(OUTDIR)/bench
FORMATS ?= image

COMMON_SRC := bench.c

FORMAT_SRC := $(foreach fmt,$(FORMATS),\
	$(shell find ../src/format/$(fmt) -name "*.c" 2>/dev/null))

SRC := $(COMMON_SRC) $(FORMAT_SRC)

OBJ := $(patsubst %.c,$(OUTDIR)/%.o,$(SRC))

LIB=../dist/libtinyff.a

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ $(LIB) -o $@

$(OUTDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTDIR)

.PHONY: all clean
