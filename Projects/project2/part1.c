#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "string_parser.h"

int main(int argc, char *argv[]) {
    /* TODO:
        * get input from file
        * read contents
        * parse the string as a command line variable
    */

    FILE* FP = NULL;
    if (argc == 2) {
        FP = fopen(argv[1], "r");
        // TODO: handle file error
    } else {
        perror("invalid arguments");
    }

    size_t len = 512;
    char* line_buf = malloc (len);
    char** lines = malloc (sizeof(char*));
    int line_number = 0;

    // TODO: For loop over each line of file and do string filler
    while (getline (&line_buf, &len, FP) != -1) {
        if (line_number != 0) {
            lines = realloc(lines, (line_number + 1) * sizeof(char *));
        }
        lines[line_number] = strdup(line_buf);
        printf("%s", line_buf);

        line_number++;
    }
    fclose(FP);

    command_line line_token_buffer;
    pid_t *pid_array;
    pid_array = malloc(sizeof(pid_t) * line_number);

    for (int i = 0; i < line_number; i++) {
        pid_array[i] = fork();
        if (pid_array[i] < 0) {
            perror("fork");
        }
        if (pid_array[i] == 0) {
            if (execvp(line_token_buffer.command_list[0], line_token_buffer.command_list) == -1) {
                perror("execvp");
            }
            exit(-1);
        }
    }

    for (int i = 0; i < line_number; i++) {
        waitpid(pid_array[i], NULL, 0);
    }

    for (int i = 0; i < line_number; i++) {
        free(lines[i]);
    }
    free(lines);

    free(pid_array);
    exit(0);
}