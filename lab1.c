#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "helpers.h"

int main(int argc, char *argv[])
{
	char *args[] = { argv[0], argv[0], "Wow:", "execl,", "execlp,", "execle,", "execv,",
		"execvp", "-", "execute", "a", "file", NULL };
	char *fake_env[] = { "USER=root (haha)", NULL };

	if (argc == 2) {
		int type = atoi(argv[1]);

		switch (type) {
			case 1:
				if (execl(argv[0], argv[0], "Wow!", "it works!", NULL) < 0)
					die("execl");
				break;

			case 2:
				if (execlp("echo", "echo", "Hmm...", "You typed '2'", NULL) < 0)
					die("execlp");
				break;

			case 3:
				if (execle(argv[0], argv[0], "It's", "execle", "talking!",
							"Note the changed $USER", NULL, fake_env) < 0)
					die("execle");
				break;

			case 4:
				if (execv(argv[0], args + 1) < 0)
					die("execv");
				break;

			case 5:
				if (execvp("echo", args) < 0)
					die("execvp");
				break;

			case 6:
				if (execve(argv[0], args + 1, fake_env) < 0)
					die("execve");
				break;

			default:
				die("Please suppy number in 1..6");
		}
	} else {
		int i;

		for (i=1; i<argc; i++)
			printf("%s ", argv[i]);
		printf("\nUSER: %s\n", getenv("USER"));
	}
	return 0;
}

