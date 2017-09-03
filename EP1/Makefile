CC=gcc
CFLAGS=-ansi -Wall -pedantic -O2 -lreadline -ltermcap
OBJECTS=ep1sh.c stringReader.o

all: ep1sh clean

ep1sh: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o ep1sh

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o