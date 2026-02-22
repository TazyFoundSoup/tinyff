CC=gcc
AR=ar
CFLAGS=-Wall -Wextra -std=c11 -O2 -Iinclude -Iinclude/tinyff -Iinclude/ext

SRC=$(shell find src -name "*.c")
OBJ=$(SRC:.c=.o)

LIB=libtinyff.a
OUTDIR=dist

all: $(LIB)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(LIB): $(OBJ)
	$(AR) rcs $(OUTDIR)/$(LIB) $(OUTDIR)/$(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $(OUTDIR)/$@

clean:
	rm -f $(OUTDIR)/$(OBJ) $(LIB)

.PHONY: all clean
