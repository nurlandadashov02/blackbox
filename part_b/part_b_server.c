/*
 * @author: Nurlan Dadashov (2019400300)
 * Modified Part A in way to have client which will make RPC to server which will create a child process. Then server will return the output to the clienr
 * PS codes have beenn used.
 * This file contains server code. 
 */

#include <unistd.h>
#include "part_b.h"

char ** execute_1_svc(input *argp, struct svc_req *rqstp)
{
	static char * result;

    int     p2c[2], c2p[2], c2perr[2];
    pid_t   pid;
    char    w_buf[100000], r_buf[100000];

    pipe(p2c);      // pipe is used to pass stdin to child process
    pipe(c2p);      // pipe is used to pass stdout to parent process
    pipe(c2perr);   // pipe is used to pass stderr to parent process

    pid = fork();

    if(pid > 0) {
        /* Code for the parent process. */

        close(p2c[0]);
        close(c2p[1]);
        close(c2perr[1]);

        /* Send input integers to the child process through the forward pipe (p2c). */
        sprintf(w_buf, "%d %d\n", argp->number1, argp->number2);
        write(p2c[1], w_buf, strlen(w_buf));

        /* Cleaning read buffer */
        memset(r_buf, 0, sizeof r_buf);

        /* Read the child's stdout through the reverse pipe (c2p). */
        int read1 = read(c2p[0], r_buf, sizeof(r_buf));

        if(read1 > 0) {
            char buf[100000] = "SUCCESS:\n";
            strcat(buf, r_buf);
            result = buf;
        }
        else {
            char *result2 = NULL;

            size_t length = 0;

            result2 = (char *) calloc(6, sizeof(char));
            strcat(result2, "FAIL:\n");
            length += 6;

            /* reading error from pipe */
            while(read(c2perr[0], r_buf, sizeof(r_buf)) > 0) {
                /* re-allocating memory */
                result2 = realloc(result2, length + strlen(r_buf));
                /* concatanating last read part to the main string */
                strncat(result2, r_buf, sizeof(r_buf)/sizeof(char));
                length += strlen(r_buf);
                memset(r_buf, 0, sizeof r_buf);
            };

            result = result2;
        }
    }
    else {
        /* Code for the child process. */

        /* The child associates the read end of forward pipe (p2c) with its own stdin. */
        dup2(p2c[0], STDIN_FILENO);
        /* The child associates the write end of reverse pipe (c2p) with its own stdout */
        dup2(c2p[1], STDOUT_FILENO);
        /* The child associates the write end of reverse pipe (c2perr) with its own stderr. */
        dup2(c2perr[1], STDERR_FILENO);

        close(c2p[0]);
        close(c2p[1]);
        close(c2perr[0]);
        close(c2perr[1]);
        close(p2c[0]);
        close(p2c[1]);

        /* Executing blackbox */
        execl(argp->path, argp->path, r_buf, NULL);
    }

	return &result;
}
