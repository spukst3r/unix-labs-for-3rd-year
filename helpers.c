#include "helpers.h"

#include <stdio.h>
#include <stdlib.h>

void die(const char *message)
{
	fprintf(stderr, "%s\n", message);
	exit(-1);
}

