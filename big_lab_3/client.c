#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define FIFO_DEFAULT_PATH ".kuzmina_fifo"

/* Global vars */
int sem = -1, fifo = -1;
key_t key;

struct parameters {
	char *message,
		 *fifo_path;
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
	char buf[128];
	key = ftok("server.c", 888);

	default_params(&params);
	parse_cmdline(argc, argv, &params);

	fifo = open(params.fifo_path, O_RDWR);
	if (fifo < 0)
		die("open");

	sem = semget(key, 0, 0);
	if (sem < 0)
		die("semget");

	sem_op(0, -1);

	snprintf(buf, 128, "%s", params.message);
	write(fifo, buf, strlen(buf) + 1);

	sem_op(1, 0);
	sem_op(-1, -1);

	read(fifo, buf, 128);
	printf("Read from server: '%s'\n", buf);

	sem_op(0, 1);

	return 0;
}

void clean_up()
{
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
		{ "fifo-path",    1, NULL, 'f' },
		{ 0, 0, 0, 0 }
	};

	int val, opt_index = 0, arg;

	while ((val = getopt_long(argc, argv, "haf:",
					long_options, &opt_index)) != -1) {

		switch (val) {
			case 'h':
				show_help(argv[0]);
				exit(0);

			case 'a':
				show_about();
				exit(0);

			case 'f':
				p->fifo_path = optarg;
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
			"  -f, --fifo-path=PATH Set fifo path, default: " FIFO_DEFAULT_PATH
			"\n",
			prog_name);
}

void show_about()
{
	fprintf(stderr, "Lab 3, made by Alyonka Kuzmina (062)\n");
}

void default_params(struct parameters *p)
{
	p->message  = NULL;
	p->fifo_path = FIFO_DEFAULT_PATH;
}

