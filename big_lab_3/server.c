#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define FIFO_DEFAULT_PATH "~/.kuzmina_fifo"
#define FIFO_DEFAULT_MODE 0640

struct parameters {
	char *fifo_path;
	mode_t fifo_mode;
} params;

int fifo = -1, sem = -1;
key_t key;

void clean_up();
void show_about();

void parse_cmdline(int argc, char *argv[], struct parameters *p);
void die(const char *message);
void show_help(const char *prog_name);
void default_params(struct parameters *p);

int server_main();
int sem_op(int first, int second);

int main(int argc, char *argv[])
{
	default_params(&params);
	parse_cmdline(argc, argv, &params);

	return server_main();
}

void default_params(struct parameters *p)
{
	key = ftok("server.c", 888);
	p->fifo_mode = FIFO_DEFAULT_MODE;
	p->fifo_path = FIFO_DEFAULT_PATH;
}

void parse_cmdline(int argc, char *argv[], struct parameters *p)
{
	struct option long_options[] = {
		{ "help",         0, NULL, 'h' },
		{ "about",        0, NULL, 'a' },
		{ "fifo-mode",    1, NULL, 's' },
		{ "fifo",         1, NULL, 'f' },
		{ 0, 0, 0, 0 }
	};

	int val, opt_index = 0, arg;

	while ((val = getopt_long(argc, argv, "has:f:",
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

			case 's':
				val = (int)strtol(optarg, (char**)NULL, 8);
				p->fifo_mode = val;
				break;

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

void clean_up()
{
	if (fifo != -1)
		close(fifo);

	if (sem != -1)
		if (semctl(sem, 0, IPC_RMID) < 0)
			die("semctl");
}

void show_help(const char *prog_name)
{
	fprintf(stderr, "Usage: %s [OPTIONS]\n"
			"\n"
			"Options:\n"
			"  -a, --about          Show program and author info\n"
			"  -h, --help           Show this help\n"
			"  -s, --fifo-mode=MODE Set fifo's permissions, default: %o\n"
			"  -f, --fifo=NAME      Set fifo's path, default: " FIFO_DEFAULT_PATH
			"\n",
			prog_name, FIFO_DEFAULT_MODE);
}

void show_about()
{
	fprintf(stderr, "Lab 3, made by Alyonka Kuzmina (062)\n");
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

int server_main()
{
	int cmd, tmp;
	char *buf;

	fifo = mkfifo(params.fifo_path, params.fifo_mode);
	if (mkfifo < 0)
		die("mkfifo");

	sem = semget(key, 2, IPC_CREAT | IPC_EXCL | 0640);
	if (sem < 0)
		die("semget");

	sem_op(0, 1);

	while (1) {
		sem_op(-1, 0);
		/*cmd = ((int*)shm_data)[0];
		if (cmd < 0)
			break;

		buf = shm_data + sizeof(int);
		tmp = atoi(buf);

		snprintf(buf, params.shm_size, "%d", tmp * tmp);*/

		sem_op(1, 1);
	}

	clean_up();

	return 0;
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

