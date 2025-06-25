CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -I/usr/include/ncurses
LDFLAGS=-lncurses -lpanelw

# Sources and binaries directory
SRCDIR=src
BINDIR=bin

# Source files to compile
SRC=$(wildcard $(SRCDIR)/*.c)

# Object file for each source file
OBJ=$(SRC:.c=.o)

# Binary name
BIN=systui.o

all: $(BINDIR)/$(BIN)

$(BINDIR)/$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BINDIR)/$(BIN)

.PHONY: all clean
