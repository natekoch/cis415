CFLAGS=-W -Wall
OBJECTS=Lab0.o
PROGRAMS=Lab0.out

Lab0.out: $(OBJECTS)
	gcc -o $@ $(OBJECTS)

Lab0.o: Lab0.c

clean:
	rm -f $(OBJECTS) $(PROGRAMS)
