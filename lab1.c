#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
	char *args[] = { argv[0], "execl,", "execlp,", "execle,", "execv,",
		"execvp", "-", "execute", "a", "file", NULL };
	char *fake_env[] = { "USER=root (haha)", NULL };

	if (argc == 2) {
		int type = atoi(argv[1]);

		switch (type) {
			case 1:
				if (execl(argv[0], argv[0], "Wow!", "it works!", NULL) < 0) {
					perror("execl");
					exit(-1);
				}
				break;
			case 2:
				if (execlp(argv[0], argv[0], "Hmm...", "You typed '2'", NULL) < 0) {
					perror("execlp");
					exit(-1);
				}
				break;
			case 3:
				if (execle(argv[0], argv[0], "It's", "execle", "talking!",
							"Note the changed $USER", NULL, fake_env) < 0) {
					perror("execle");
					exit(-1);
				}
				break;
			case 4:
				if (execv(argv[0], args) < 0) {
					perror("execv");
					exit(-1);
				}
				break;
			case 5:
				if (execvp(argv[0], args) < 0) {
					perror("execvp");
					exit(-1);
				}
				break;
			case 6:
				if (execve(argv[0], args, fake_env) < 0) {
					perror("execve");
					exit(-1);
				}
				break;
			default:
				die("Please suppy number in 1..6");
				break;
		}
	} else {
		int i;

		for (i=1; i<argc; i++)
			printf("%s ", argv[i]);
		printf("\nUSER: %s\n", getenv("USER"));
	}
	return 0;
}

