#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
	int mq = msgget(ftok("server.c", 89), IPC_CREAT | 0640), t, i, stop = 0;
	struct msg msg;

	if (mq < 0) {
		perror("msgget");
		exit(-1);
	}

	while (1) {
		for (i=1; i<4; i++)
			if (msgrcv(mq, &msg, MTEXT_SIZE, i, IPC_NOWAIT) > 0) {
				printf("msg:\n  type: %li\n  text: %s\n\n", msg.mtype, msg.mtext);

				t = atoi(msg.mtext);
				switch (msg.mtype) {
					case 1:
						snprintf(msg.mtext, MTEXT_SIZE, "%d", t * t);
						break;
					case 2:
						snprintf(msg.mtext, MTEXT_SIZE, "%d", abs(t));
						break;
					case 3:
						printf("Recieved 3 as msgtype, exiting\n");
						stop = 1;
						break;
					default:
						fprintf(stderr, "Invalid message type!\n");
				}

				msg.mtype = 4;
				if (msgsnd(mq, &msg, strlen(msg.mtext) + 1, 4) < 0) {
					perror("msgsnd");
					exit(-1);
				}
				if (stop)
					exit(0);
			}
	}

	return 0;
}

