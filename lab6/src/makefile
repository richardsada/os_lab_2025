all: testserv test

testserv: server.c
	gcc -o testserv server.c

test: client.c
	gcc -o test client.c

clean:
	rm testserv test
