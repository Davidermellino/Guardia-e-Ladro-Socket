CC = gcc
CFLAGS = -Wall -g

all: UI_management guardia

guardia: guardia.o 
	$(CC) $(CFLAGS) -o guardia guardia.o -lncurses

UI_management: UI_management.o
	$(CC) $(CFLAGS) -o UI_management UI_management.o -lncurses

UI_management.o: UI_management.c main.h
	$(CC) $(CFLAGS) -c UI_management.c


guardia.o: guardia.c main.h
	$(CC) $(CFLAGS) -c guardia.c

server: UI_management
	./UI_management

client: guardia
	./guardia

clean:
	rm -f *.o
	rm -f UI_management
	rm -f guardia