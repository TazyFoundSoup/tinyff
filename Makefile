CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
CFILES = src/common.c
OBJS = $(CFILES:.c=.o)
TARGET = libtinyff.a

$(TARGET): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
