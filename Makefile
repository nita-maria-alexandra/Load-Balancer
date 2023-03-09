CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
LOAD=load_balancer
SERVER=server
LINKED=LinkedList

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o $(LINKED).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(LINKED).o: $(LINKED).c $(LINKED).h
	$(CC) $(CFLAGS) $^ -c

pack:
	zip -FSr 314CA_NitaMariaAlexandra_Tema2.zip README Makefile *.c *.h

clean:
	rm -f *.o tema2 *.h.gch

.PHONY: pack clean run
