#include "helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void die(const char *message)
{
	if (errno != 0)
		perror(message);
	else
		fprintf(stderr, "%s\n", message);
	exit(-1);
}

