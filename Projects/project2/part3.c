#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "string_parser.h"

int counter;
void on_signal_alarm(int sig) {
    counter = 1;
    printf("Process: %d - received SIGALRM: %d\n", getpid(), sig);
}

void on_signal_sigusr1(int sig) {
    printf("Process: %d - received SIGUSR1: %d\n", getpid(), sig);
}

void on_signal_stop(int sig) {
    printf("Process: %d - received SIGSTOP: %d", getpid(), sig);
}

void on_signal_cont(int sig) {
    printf("Process: %d - received SIGCONT: %d", getpid(), sig);
}


int main(int argc, char *argv[]) {
    signal(SIGALRM, on_signal_alarm);
    alarm(2);

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
            if (sig) on_signal_sigusr1(sig);
            signal(SIGSTOP, on_signal_stop);
            signal(SIGCONT, on_signal_cont);
            if (execvp(line_token_buffer.command_list[0], line_token_buffer.command_list) == -1) {
                perror("execvp");
            }
            exit(-1);
        }
        free_command_line(&line_token_buffer);
    }

    // send SIGUSR1
    for (int i = 0; i < line_number; i++) {
        kill(pid_array[i], SIGUSR1);
    }

    /*
    sleep(5);
    // send SIGSTOP
    for (int i = 0; i < line_number; i++) {
        kill(pid_array[i], SIGSTOP);
    }
    sleep(5);
    // send SIGCONT
    for (int i = 0; i < line_number; i++) {
        kill(pid_array[i], SIGCONT);
    }
    */
    int process_exited[line_number] = {0};

    int status;
    int current_process = 0;
    int num_terminated = 0;
    while (1) {
        printf("hello");
        if (counter == 1) {
            alarm(2);
            for (int i = 0; i < line_number; i++) {
                kill(pid_array[i], SIGSTOP);
            }
            kill(pid_array[current_process], SIGCONT);
            current_process++;
            if (current_process == line_number) {
                current_process = 0;
            }
            counter = 0;
            // check remaining processes status
        }
    }


    // wait for processes to exit
    for (int i = 0; i < line_number; i++) {
        waitpid(pid_array[i], NULL, 0);
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