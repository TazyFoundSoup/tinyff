CC=gcc
AR=ar
CFLAGS=-Wall -Wextra -std=c11 -O2 -Iinclude -Iinclude/tinyff -Iinclude/ext

SRC=$(shell find src -name "*.c")
OBJ=$(SRC:.c=.o)

LIB=libtinyff.a
OUTDIR=dist

all: $(OUTDIR)/$(LIB)

$(OUTDIR)/$(LIB): $(OBJ)
	@mkdir -p $(OUTDIR)
	$(AR) rcs $@ $(addprefix $(OUTDIR)/,$(OBJ))

%.o: %.c
	@mkdir -p $(dir $(OUTDIR)/$@)
	$(CC) $(CFLAGS) -c $< -o $(OUTDIR)/$@

clean:
	rm -rf $(OUTDIR)

.PHONY: all clean
