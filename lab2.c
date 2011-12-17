#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "helpers.h"

#define F_NAME "lab2.txt"

int main(int argc, char *argv[])
{
	int f = open(F_NAME, O_CREAT | O_WRONLY, 0640);
	char str[] = "This is my fancy file!";
	if (f < 0) {
		perror("open");
		exit(-1);
	}

	write(f, str, strlen(str) + 1);

	close(f);

	f = open(F_NAME, O_RDONLY);
	if (f < 0) {
		perror("open");
		exit(-1);
	}
	read(f, str, sizeof str);
	printf("Read from file: '%s'\n", str);
	close(f);

	if (unlink(F_NAME) < 0) {
		perror("unlink");
	}
	return 0;
}

