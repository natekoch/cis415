/*
 * Nate Koch
 * CIS 415
 * project 2 - part 4
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "string_parser.h"


static volatile sig_atomic_t counter = 1;
void on_signal_alarm(int sig) {
    (void)sig;
    counter = 1;
    printf("~~~ Parent process (%d) - switching context ~~~\n", getpid());
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
    sigaddset(&sigset, SIGCONT);
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
            // wait for SIGCONT
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

    char process_exited[line_number];
    // print out child pids
    printf("\n--- CHILD PIDS ---\n");
    // set all child processes to not yet exited (0)
    for (int i = 0; i < line_number; i++) {
        process_exited[i] = 0;
        printf("Child %d: %d\n", i, pid_array[i]);
    }
    printf("------------------\n\n");

    int status;
    int current_process = 0;
    int num_terminated = 0;
    while (1) {
        if (counter == 1) {
            alarm(2);
            // send SIGSTOP to all children
            for (int i = 0; i < line_number; i++) {
                if (process_exited[i] == 0) {
                    kill(pid_array[i], SIGSTOP);
                }
            }
            printf("> Current running process: %d\n\n", pid_array[current_process]);
            // send SIGCONT to the current process
            if (process_exited[current_process] != 1) {
                kill(pid_array[current_process], SIGCONT);
            }

            FILE *proc_file = NULL;
            char fname_buf[128];
            char proc_line[128];
            printf("\n///// PROC STATUS TABLE \\\\\\\\\\\n");
            for (int i = 0; i < line_number; i++) {
                sprintf(fname_buf, "/proc/%d/status", pid_array[i]);
                proc_file = fopen(fname_buf, "r");
                if (proc_file) {
                    printf("%%%%%% CHILD %d STATUS %%%%%%\n", i);
                    while (fgets(proc_line, 128, proc_file)) {
                        if (strncmp(proc_line, "Name:", 5) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "State:", 6) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "Pid:", 4) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "PPid:", 5) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "Threads:", 8) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "VmPeak:", 7) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "VmSize:", 7) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "SigQ:", 5) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "SigPnd:", 7) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "SigBlk:", 7) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "SigIgn:", 7) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "SigCgt:", 7) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "Mems_allowed_list:", 18) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "voluntary_ctxt_switches:", 24) == 0) {
                            printf("%s", proc_line);
                        }
                        if (strncmp(proc_line, "nonvoluntary_ctxt_switches:", 27) == 0) {
                            printf("%s", proc_line);
                        }
                    }
                    fclose(proc_file);
                    printf("%%%%%% END CHILD %d STATUS %%%%%%\n\n", i);
                }
            }
            printf("\\\\\\\\\\ END OF PROC STATUS TABLE /////\n\n");

            // check if the process has exited
            if (process_exited[current_process] != 1) {
                waitpid(pid_array[current_process], &status, WNOHANG | WUNTRACED | WCONTINUED);
                if (WIFEXITED(status)) {
                    printf("~ process : %d exited ~\n\n", pid_array[current_process]);
                    num_terminated++;
                    process_exited[current_process] = 1;
                }
            }
            if (num_terminated == line_number) {
                printf("##### ALL PROCESSES FINISHED #####\n");
                break;
            }
            // set current_process to an non-exited process
            while (1) {
                current_process++;
                if (current_process == line_number) {
                    current_process = 0;
                }
                if (process_exited[current_process] == 0) {
                    break;
                }
            }
            // reset the alarm counter
            counter = 0;
        }
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