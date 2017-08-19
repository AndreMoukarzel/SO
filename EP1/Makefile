CC=gcc
CFLAGS=-ansi -Wall -pedantic -O2 -lreadline -ltermcap
OBJECTS=ep1sh.c stringReader.o

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

ep1sh: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o ep1sh

clean:
	rm -rf *.o