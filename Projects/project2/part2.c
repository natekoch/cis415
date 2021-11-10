/*
 * Nate Koch
 * CIS 415
 * project 2 - part 2
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "string_parser.h"

int main(int argc, char *argv[]) {
    FILE* FP = NULL;
    // open file and argument error handling
    if (argc == 2) {
        FP = fopen(argv[1], "r");
        if (FP == NULL) {
            perror("fopen: ");
            exit(-1);
        }
    } else {
        printf("invalid arguments\n");
        exit(-1);
    }

    size_t len = 512;
    char* line_buf = malloc (len);
    char** lines = malloc (sizeof(char*));
    int line_number = 0;

    // get the commands from each line of the file
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

    int sig;
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    // Block with SIGUSR1 signal
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    // spawn child processes
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
            free_command_line(&line_token_buffer);
            for (int i = 0; i < line_number; i++) {
                free(lines[i]);
            }
            free(lines);
            free(line_buf);
            free(pid_array);
            exit(-1);
        }
        free_command_line(&line_token_buffer);
    }

    // print out pids
    printf("--- CHILD PIDS ---\n");
    for (int i = 0; i < line_number; i++) {
        printf("Child #%d: %d\n", i, pid_array[i]);
    }
    printf("------------------\n\n");

    sleep(5);
    // send SIGUSR1
    for (int i = 0; i < line_number; i++) {
        printf("!!! Sending SIGUSR1 to process: %d !!!\n", pid_array[i]);
        kill(pid_array[i], SIGUSR1);
    }
    sleep(5);
    printf("\n");
    // send SIGSTOP
    for (int i = 0; i < line_number; i++) {
        printf("!!! Sending SIGSTOP to process: %d !!!\n", pid_array[i]);
        kill(pid_array[i], SIGSTOP);
    }
    sleep(5);
    printf("\n");
    // send SIGCONT
    for (int i = 0; i < line_number; i++) {
        printf("!!! Sending SIGCONT to process: %d !!!\n", pid_array[i]);
        kill(pid_array[i], SIGCONT);
    }
    printf("\n");
    // wait for processes to exit
    for (int i = 0; i < line_number; i++) {
        waitpid(pid_array[i], NULL, 0);
        printf("@@@ Done waiting for process: %d to exit @@@\n", pid_array[i]);
    }

    // free allocated memory
    for (int i = 0; i < line_number; i++) {
        free(lines[i]);
    }
    free(lines);
    free(line_buf);
    free(pid_array);
    exit(0);
}