#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define MTEXT_SIZE 128

struct msg {
	long mtype;
	char mtext[MTEXT_SIZE];
};

int main(int argc, char *argv[])
{
	int mq;
	struct msg msg;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s msg_type message\n", argv[0]);
		exit(-1);
	}

	mq = msgget(ftok("server.c", 89), 0);

	if (mq < 0) {
		perror("msgget");
		exit(-1);
	}

	msg.mtype = atoi(argv[1]);
	strcpy(msg.mtext, argv[2]);

	if (msgsnd(mq, &msg, strlen(msg.mtext) + 1, msg.mtype) < 0) {
		perror("msgsnd");
		exit(-1);
	}
	if (msgrcv(mq, &msg, MTEXT_SIZE, 4, 0) < 0) {
		perror("msgrcv");
		exit(-1);
	}

	printf("Answer: %s\n", msg.mtext);

	return 0;
}

