#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "helpers.h"

void parent_sig_handler(int s);
void child_sig_handler(int s);

pid_t child;

int main(int argc, char *argv[])
{
	struct sigaction sigact = {0};

	sigact.sa_handler = parent_sig_handler;

	sigaction(SIGQUIT, &sigact, NULL);

	switch ((child = fork())) {
		case 0:
			sigact.sa_handler = child_sig_handler;
			sigaction(SIGUSR1, &sigact, NULL);
			while (1);

		default:
			while (1);

		case -1:
			die("fork");
			break;
	}

	return 0;
}

void parent_sig_handler(int s)
{
	kill(child, SIGUSR1);
	exit(0);
}

void child_sig_handler(int s)
{
	printf("Parent got SIGQUIT!\n");
	exit(0);
}

