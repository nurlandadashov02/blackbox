/*
 * @author: Nurlan Dadashov (2019400300)
 * Modified Part A in way to have client which will make RPC to server which will create a child process. Then server will return the output to the clienr
 * PS codes have beenn used.
 * This file contains client code. 
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "part_b.h"

void add_prog_1(char *blackbox, char *output, char *host)
{
	CLIENT *clnt;
	char * *result_1;
	input  execute_1_arg;

    int fd;
    fd = open(output, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) exit(-1);
    if (dup2(fd, STDOUT_FILENO) == -1) exit(-1);
    if (close(fd) == -1) exit(-1);

#ifndef	DEBUG
	clnt = clnt_create (host, ADD_PROG, ADD_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

    execute_1_arg.path = blackbox;
    scanf("%d", &execute_1_arg.number1);
    scanf("%d", &execute_1_arg.number2);

	result_1 = execute_1(&execute_1_arg, clnt);
	if (result_1 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}
	else {
        printf("%s", *result_1);
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int main (int argc, char *argv[])
{
    add_prog_1 (argv[1], argv[2], argv[3]);
	exit (0);
}
