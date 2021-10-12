CFLAGS=-g -W -Wall
OBJECTS=main.o command.o
PROGRAMS=lab2

lab2: $(OBJECTS)
	gcc -o $@ $(OBJECTS)

main.o: main.c

command.o: command.c

clean:
	rm -f $(OBJECTS) $(PROGRAMS)