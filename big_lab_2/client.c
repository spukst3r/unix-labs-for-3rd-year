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
int sem = -1, shm = -1;
void *shm_data;
key_t key;

struct parameters {
	char *message;
	size_t shm_size;
} params;

int sem_op(int first, int second);

void clean_up();
void show_about();
void default_params(struct parameters *p);
void show_help(const char *prog_name);
void die(const char *message);
void parse_cmdline(int argc, char *argv[], struct parameters *p);

int main(int argc, char *argv[])
{
	int cmd = 0;
	char *buf;
	key = ftok("server.c", 113);

	default_params(&params);
	parse_cmdline(argc, argv, &params);

	sem = semget(key, 0, 0);
	if (sem < 0)
		die("semget");

	shm = shmget(key, params.shm_size, 0);
	if (shm < 0)
		die("shmget");

	shm_data = shmat(shm, NULL, 0);

	sem_op(0, -1);

	if (!strcmp(params.message, "exit"))
		cmd = -1;

	((int*)shm_data)[0] = cmd;
	buf = shm_data + sizeof(int);
	snprintf(buf, params.shm_size, "%s", params.message);

	sem_op(1, 0);
	sem_op(-1, -1);

	if (cmd >= 0)
		printf("Read from server: '%s'\n", buf);

	sem_op(0, 1);

	return 0;
}

void clean_up()
{
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

void die(const char *message)
{
	if (errno)
		perror(message);
	else
		fprintf(stderr, "%s\n", message);

	clean_up();
	exit(-1);
}

int sem_op(int first, int second)
{
	struct sembuf ops[2] = {0};

	ops[0].sem_num = 0;
	ops[0].sem_op  = first;
	ops[0].sem_flg = 0;

	ops[1].sem_num = 1;
	ops[1].sem_op  = second;
	ops[1].sem_flg = 0;

	return semop(sem, ops, 2);
}

void parse_cmdline(int argc, char *argv[], struct parameters *p)
{
	struct option long_options[] = {
		{ "help",         0, NULL, 'h' },
		{ "about",        0, NULL, 'a' },
		{ "shm-size",     1, NULL, 's' },
		{ 0, 0, 0, 0 }
	};

	int val, opt_index = 0, arg;

	while ((val = getopt_long(argc, argv, "has:",
					long_options, &opt_index)) != -1) {

		switch (val) {
			case 'h':
				show_help(argv[0]);
				exit(0);

			case 'a':
				show_about();
				exit(0);

			case 's':
				arg = atoi(optarg);
				if (arg < 1)
					die("Invalid shared memory size");

				p->shm_size = arg;
				break;

			default:
				show_help(argv[0]);
				exit(-1);
		}
	}

	if (*(argv + optind) == NULL) {
		show_help(argv[0]);
		exit(-1);
	}

	p->message = *(argv + optind);
}

void show_help(const char *prog_name)
{
	fprintf(stderr, "Usage: %s [OPTIONS] message\n"
			"\n"
			"Options:\n"
			"  -a, --about          Show program and author info\n"
			"  -h, --help           Show this help\n"
			"  -s, --shm-size       Set shared memory size, default: 512\n"
			"\n",
			prog_name);
}

void show_about()
{
	fprintf(stderr, "Lab 2, made by Alyonka Kuzmina (062)\n");
}

void default_params(struct parameters *p)
{
	p->message  = NULL;
	p->shm_size = 512;
}

