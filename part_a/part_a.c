/*
 * Author: Nurlan Dadashov (Student ID: 2019400300)
 * Creating a child process which will execute the blackbox and parent will print the output.
 * PS codes have beenn used.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
    char *blackbox = argv[1];
    char *output = argv[2];

    int fd = open(output, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd == -1) return -1;
    if (dup2(fd, STDOUT_FILENO) == -1) return -1;
    if (close(fd) == -1) return -1;

    // reading input for blackbox
    int input1, input2;
    scanf("%d", &input1);
    scanf("%d", &input2);

    int     p2c[2], c2p[2], c2perr[2];
    pid_t   pid;
    char    w_buf[100000], r_buf[100000];

    pipe(p2c);      // pipe is used to pass stdin to child process
    pipe(c2p);      // pipe is used to pass stdout to parent process
    pipe(c2perr);   // pipe is used to pass stderr to parent process

    pid = fork();

    if(pid == -1) return -1;

    if(pid > 0) {
        /* Code for the parent process. */

        close(p2c[0]);
        close(c2p[1]);
        close(c2perr[1]);

        /* Send input integers to the child process through the forward pipe (p2c). */
        sprintf(w_buf, "%d %d\n", input1, input2);
        write(p2c[1], w_buf, strlen(w_buf));

        /* Read the child's stdout through the reverse pipe (c2p). */
        int read1 = read(c2p[0], r_buf, sizeof(r_buf));
        if(read1 > 0) {
            printf("SUCCESS:\n%s", r_buf);
        }
        else {
            /* Read the child's stderr through the reverse pipe (c2perr). */
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

            printf("%s\n", result2);
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

        close(p2c[0]);
        close(p2c[1]);
        close(c2p[0]);
        close(c2p[1]);
        close(c2perr[0]);
        close(c2perr[1]);

        /* Executing blackbox */
        execl(blackbox, blackbox, r_buf, NULL);
    }
    return 0;
}