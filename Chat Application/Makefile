all: server client

client: client.c
	gcc -pthread client.c -o client -w

server: server.c
	gcc -pthread server.c -o server -w

clean:
	rm client server