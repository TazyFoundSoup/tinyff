CC=gcc
AR=ar
CFLAGS=-Wall -Wextra -std=c11 -O2 -Iinclude -Iinclude/tinyff -Iinclude/ext

SRC=$(shell find src -name "*.c")
OBJ=$(SRC:.c=.o)

LIB=libtinyff.a

all: $(LIB)

$(LIB): $(OBJ)
	$(AR) rcs $(LIB) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(LIB)

.PHONY: all clean
