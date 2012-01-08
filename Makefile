CC = gcc
BINARIES = lab1 lab2 lab3.server lab3.client lab4.1
SUBDIRS = big_lab_2 big_lab_3

all: ${BINARIES} subdirs

lab1: lab1.o helpers.o

lab2: lab2.o helpers.o

lab4.1: lab4.1.o helpers.o

helpers.o: helpers.c helpers.h
	${CC} -c $<

.c.o:
	${CC} -c $^

.PHONY: subdirs ${SUBDIRS}

subdirs: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@

.PHONY: clean

clean:
	rm -f ${BINARIES}
	rm -f *.txt
	for dir in ${SUBDIRS}; do \
		${MAKE} clean -C $$dir; \
	done

