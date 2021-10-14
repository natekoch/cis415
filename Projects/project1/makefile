CFLAGS=-g -W -Wall
OBJECTS=main.o command.o string_parser.o
PROGRAMS=pseudo-shell

pseudo-shell: $(OBJECTS)
	gcc -o $@ $(OBJECTS)

main.o: main.c
	gcc -c main.c

command.o: command.c
	gcc -c command.c

string_parser.o: string_parser.c string_parser.h
	gcc -c string_parser.c

clean:
	rm -f $(OBJECTS) $(PROGRAMS)