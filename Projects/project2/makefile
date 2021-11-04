CC=gcc
CFLAGS=-g -W -Wall
OBJECTS = part1.o
PROGRAMS = part1

all: part1

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

part1: part1.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(PROGRAMS)