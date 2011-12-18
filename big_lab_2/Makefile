CC = gcc
BINARIES = server client

all: server client

server: server.o

client: client.o

.c.o:
	${CC} -c $^

clean:
	rm -f *.o
	rm -f ${BINARIES}
