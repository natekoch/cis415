CC=gcc
CFLAGS=-g -W -Wall
DEPS = string_parser.h command.h
OBJECTS = main.o string_parser.o command.o
PROGRAMS = pseudo-shell

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

pseudo-shell: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(PROGRAMS)