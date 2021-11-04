#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "string_parser.h"

int main(int argc, char *argv[]) {
    /* TODO:
        * get input from file
        * read contents
        * parse the string as a command line variable
    */

    FILE* FP;
    FP = fopen(argv[1], "r");

    command_line line_buffer;
    pid_t *pid_array;
    pid_array = malloc(sizeof(pid_t) * n);

    // TODO: For loop over each line of file and do string filler

    char *argList[] = {"./iobound", "-seconds", "10", NULL};
    for (int i = 0; i < n; i++) {
        pid_array[i] = fork();
        if (pid_array[i] < 0) {
            perror("fork");
        }
        if (pid_array[i] == 0) {
            if (execvp("./iobound", argList) == -1) {
                perror("execvp");
            }
            exit(-1);
        }
    }
    for (int i = 0; i < n; i++) {
        waitpid(pid_array[i], NULL, 0);
    }

    // TODO: free command line

    free(pid_array);
    exit(0);
}