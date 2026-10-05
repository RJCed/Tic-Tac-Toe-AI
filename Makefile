# Makefile for the terminal Tic-Tac-Toe game.
#
#   make          build the game
#   make run      build (if needed) and run the game
#   make debug    build with debug info and sanitizers (output: tic-tac-toe-debug)
#   make clean    remove build output

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c17
TARGET  = tic-tac-toe

SRCS    = main.c game.c ai.c ui.c
OBJS    = $(SRCS:.c=.o)

.PHONY: all run debug clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# Rebuild an object file when its .c file or any header changes.
%.o: %.c game.h ai.h ui.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

debug:
	$(CC) $(CFLAGS) -g -O0 -fsanitize=address,undefined $(SRCS) -o $(TARGET)-debug

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET)-debug