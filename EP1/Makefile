CC=gcc
CFLAGS=-ansi -Wall -pedantic -O2
EP1SHELL=ep1sh.c stringReader.o
EP1SIM = ep1sim.c fileReader.o pilha.o

all: ep1sh ep1sim clean

ep1sh: $(EP1SHELL)
	$(CC) $(EP1SHELL) $(CFLAGS) -lreadline -ltermcap -o ep1sh

ep1sim: $(EP1SIM)
	$(CC) $(EP1SIM) $(CFLAGS) -lpthread -o ep1sim

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o
