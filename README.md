Unix labs for 3rd year students
================================

Compilation
-----------

* clone repo, cd to it's direcotory
* type `make` to compile all labs

### lab1

Examples of all 6 execXX functions
Usage: `./lab1 X`
where `X` is one of the following:

* 1 for `execl`
* 2 for `execlp`
* 3 for `execle`
* 4 for `execv`
* 5 for `execvp`
* 6 for `execve`

### lab2

Example of file I/O API

Usage: `./lab2`
This lab creates file, writes string to it, reads it and deletes file.
That's all, folks =)

### lab3

Usage of message queue

Usage: `./lab3.server`, then `./lab3.client X Y`
where `X` is message type, 1..3; `Y` is the message content

* If `X` is 1, `X**2` is returned by server;
* If `X` is 2, `abs(X)` is returned;
* If `X` is 3, server exits, message is untouched

### lab4

Usage of signals, contains 2 parts:

* 2 processes, when parents recieves SIGQUIT, child reports about this situation:
    * Usage: `./lab4.1`
    * Press C-\\ when you want to kill parent process
* Signals with semantics O\_o

