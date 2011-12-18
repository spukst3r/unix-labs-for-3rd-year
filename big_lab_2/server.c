#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>

/* Global vars */
int **children;
int sem = -1, shm = -1;
void *shm_data;
key_t key;

struct parameters {
	unsigned int max_children;
	size_t shm_size;
} params;

void clean_up();
void show_about();

void sig_handler(int sig);
void die(const char *message);
void show_help(const char *prog_name);
void default_params(struct parameters *p);
void parse_cmdline(int argc, char *argv[], struct parameters *p);

int server_main();

int main(int argc, char *argv[])
{
	struct sigaction sigact = {0};

	sigact.sa_handler = sig_handler;

	sigaction(SIGINT, &sigact, NULL);

	default_params(&params);
	parse_cmdline(argc, argv, &params);

	return server_main();
}

void sig_handler(int sig)
{
	switch (sig) {
		case SIGINT:
			fprintf(stderr, "\nCaught SIGINT, quitting...\n");
			break;
	}

	clean_up();
	exit(0);
}

void clean_up()
{
	int i;
	if (children) {
		for (i=0; i<params.max_children; i++)
			if (children[i])
				free(children[i]);
		free(children);
	}

	if (shm_data)
		if (shmdt(shm_data) < 0)
			fprintf(stderr, "Failed to detach shared memory: %s\n", strerror(errno));

	if (shm != -1)
		if (shmctl(shm, IPC_RMID, NULL) < 0)
			fprintf(stderr, "Failed to remove shared memory segment: %s\n", strerror(errno));

	if (sem != -1)
		if (semctl(sem, 0, IPC_RMID) < 0)
			die("semctl");
}

void default_params(struct parameters *p)
{
	p->max_children = 100;
	p->shm_size     = 512;
	key = ftok("server.c", 113);
}

void parse_cmdline(int argc, char *argv[], struct parameters *p)
{
	struct option long_options[] = {
		{ "help",         0, NULL, 'h' },
		{ "about",        0, NULL, 'a' },
		{ "max-children", 1, NULL, 'M' },
		{ "shm-size",     1, NULL, 's' },
		{ "clean-up",     0, NULL, 'c' },
		{ 0, 0, 0, 0 }
	};

	int val, opt_index = 0, arg;

	while ((val = getopt_long(argc, argv, "haM:c",
					long_options, &opt_index)) != -1) {

		switch (val) {
			case 'h':
				show_help(argv[0]);
				exit(0);

			case 'a':
				show_about();
				exit(0);

			case 'M':
				arg = atoi(optarg);
				if (arg < 1 || arg > 10000)
					die("Invalid maximum children value. Should be in [1..10000]");

				p->max_children = arg;
				break;

			case 's':
				arg = atoi(optarg);
				if (arg < 1)
					die("Invalid shared memory size");

				p->shm_size = arg;
				break;

			case 'c':
				sem = semget(key, 0, 0);
				shm = shmget(key, 0, 0);
				clean_up();
				exit(0);

			default:
				show_help(argv[0]);
				exit(-1);
		}
	}

	if (*(argv + optind)) {
		fprintf(stderr, "Invalid argument: %s\n\n", *(argv + optind));
		show_help(argv[0]);
		exit(-1);
	}
}

void show_help(const char *prog_name)
{
	fprintf(stderr, "Usage: %s [OPTIONS]\n"
			"\n"
			"Options:\n"
			"  -a, --about          Show program and author info\n"
			"  -h, --help           Show this help\n"
			"  -M, --max-children   Set maximum child processes amount, "
				"default 100\n"
			"  -s, --shm-size       Set shared memory size, default: 512\n"
			"  -c, --clean-up       Do clean-up (close all opened semaphores and "
				"shared memory). Use with care!\n"
			"\n",
			prog_name);
}

void show_about()
{
	fprintf(stderr, "Lab 2, made by Alyonka Kuzmina (062)\n");
}

void die(const char *message)
{
	if (errno)
		perror(message);
	else
		fprintf(stderr, "%s\n", message);

	exit(-1);
}

int server_main()
{
	sem = semget(key, 2, IPC_CREAT | IPC_EXCL);
	if (sem < 0)
		die("semget");

	shm = shmget(key, params.shm_size, IPC_CREAT | IPC_EXCL);
	if (shm < 0)
		die("shmget");

	while (1) {
	}

	return 0;
}

