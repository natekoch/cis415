CC=gcc
CFLAGS=-g -W -Wall
DEPS = string_parser.h bank.h string_parser.h
OBJECTS = bank.o string_parser.o
PROGRAMS = bank

all: $(PROGRAMS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

bank: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(PROGRAMS)