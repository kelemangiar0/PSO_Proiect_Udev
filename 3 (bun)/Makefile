CC=gcc
CFLAGS=-Wall

all: client server

client: client.o
	$(CC) $(CFLAGS) client.o -o client

server: server.o
	$(CC) $(CFLAGS) server.o -o server

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

clean:
	rm -f *.o client server
