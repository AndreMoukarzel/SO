CC=gcc
CFLAGS=-ansi -Wall -pedantic -O2
OBJECTS = corrida.c ciclista.o

all: corrida clean

corrida: $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) -lpthread -o corrida

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o
