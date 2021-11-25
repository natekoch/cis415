CC=gcc
CFLAGS=-g -pthread -lpthread -lm
DEPS = string_parser.h bank.h account.h
OBJECTS = part1.o part2.o string_parser.o
PROGRAMS = part1 part2

all: $(PROGRAMS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

part1: part1.o string_parser.o
	$(CC) -o $@ $^ $(CFLAGS)

part2: part2.o string_parser.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(PROGRAMS)