CC=gcc
CFLAGS=-g -W -Wall
DEPS = string_parser.h
OBJECTS = part1.o string_parser.o
PROGRAMS = part1

all: part1

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

part1: part1.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(PROGRAMS)