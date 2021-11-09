CC=gcc
CFLAGS=-g -W -Wall
DEPS = string_parser.h
OBJECTS = part1.o part2.o part3.o string_parser.o
PROGRAMS = part1 part2 part3

all: part1 part2 part3

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

part1: part1.o string_parser.o
	$(CC) -o $@ $^ $(CFLAGS)

part2: part2.o string_parser.o
	$(CC) -o $@ $^ $(CFLAGS)

part3: part3.o string_parser.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(PROGRAMS)