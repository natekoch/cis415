CC=gcc
CFLAGS=-g -W -Wall
OBJECTS = lab4.o iobound.o
PROGRAMS = lab4 iobound

all: lab4 iobound

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

lab4: lab4.o
	$(CC) -o $@ $^ $(CFLAGS)

iobound: iobound.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(PROGRAMS)