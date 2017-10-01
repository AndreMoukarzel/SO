CC=gcc
CFLAGS=-ansi -Wall -pedantic -O2 -lpthread
OBJECTS = corrida.c pista.o

all: corrida clean

corrida: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -o corrida

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o
