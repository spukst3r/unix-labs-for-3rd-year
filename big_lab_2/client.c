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
const int shm_size = 512;
void *shm_data;
key_t key;

void clean_up();
void die(const char *message);
int sem_op(int first, int second);

int main(int argc, char *argv[])
{
	int cmd = 0;
	char *buf;
	key = ftok("server.c", 113);

	if (argc != 2) {
		fprintf(stderr, "Usage: %s number\nEnter 'exit' instead of number to "
				"shutdown server\n", argv[0]);
		exit(-1);
	}

	sem = semget(key, 0, 0);
	if (sem < 0)
		die("semget");

	shm = shmget(key, shm_size, 0);
	if (shm < 0)
		die("shmget");

	shm_data = shmat(shm, NULL, 0);

	sem_op(0, -1);

	if (!strcmp(argv[1], "exit"))
		cmd = -1;

	((int*)shm_data)[0] = cmd;
	buf = shm_data + sizeof(int);
	snprintf(buf, shm_size, "%s", argv[1]);

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

