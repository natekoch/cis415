#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "string_parser.h"

int main(int argc, char *argv[]) {
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

    while (getline (&line_buf, &len, FP) != -1) {
        if (line_number != 0) {
            lines = realloc(lines, (line_number + 1) * sizeof(char *));
        }
        lines[line_number] = strdup(line_buf);
        line_number++;
    }
    fclose(FP);

    command_line line_token_buffer;
    pid_t *pid_array;
    pid_array = malloc(sizeof(pid_t) * line_number);

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    int sig;

    for (int i = 0; i < line_number; i++) {
        line_token_buffer = str_filler(lines[i], " ");
        pid_array[i] = fork();
        if (pid_array[i] < 0) {
            perror("fork");
        }
        if (pid_array[i] == 0) {
            sigwait(&sigset, &sig);
            if (execvp(line_token_buffer.command_list[0], line_token_buffer.command_list) == -1) {
                perror("execvp");
            }
            printf("got signal: %ls\n", &sig);
            exit(-1);
        }
        free_command_line(&line_token_buffer);
    }

    // send sigusr1
    for (int i = 0; i < line_number; i++) {
        kill(pid_array[i], SIGUSR1);
    }

    // send sigstop
    for (int i = 0; i < line_number; i++) {
        kill(pid_array[i], SIGSTOP);
    }

    // send sigcont
    for (int i = 0; i < line_number; i++) {
        kill(pid_array[i], SIGCONT);
    }

    for (int i = 0; i < line_number; i++) {
        waitpid(pid_array[i], NULL, 0);
    }

    for (int i = 0; i < line_number; i++) {
        free(lines[i]);
    }
    free(lines);
    free(line_buf);
    free(pid_array);
    exit(0);
}