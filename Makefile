CC = gcc
BINARIES = lab1 lab2 lab3.server lab3.client

all: ${BINARIES}

lab1: lab1.o helpers.o

lab2: lab2.o helpers.o

helpers.o: helpers.c helpers.h
	${CC} -c $<

.c.o:
	${CC} -c $^

clean:
	rm -f *.o
	rm -f ${BINARIES}
	rm -f *.txt

