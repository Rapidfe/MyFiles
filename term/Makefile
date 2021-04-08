all: host client

host: host.c game.c
	gcc -pthread -o host host.c game.c

client: client.c
	gcc -pthread -o client client.c -lwiringPi

clean:
	rm host client
