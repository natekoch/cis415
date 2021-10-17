#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include "command.h"
#include "string_parser.h"

int main(int argc, char *argv[]) {
    bool filemode_on = false;
    if (argc == 1) {
        filemode_on = false;
    } else if(argc == 2) {
        if (strcmp(argv[1], "-f") == 0) {
            write(1,"No filename specified\n", 22);
            exit(EXIT_FAILURE);
        } else {
            write(1, "Invalid argument\n", 17);
            exit(EXIT_FAILURE);
        }
    } else if (argc == 3) {
        if (strcmp(argv[1], "-f") == 0) {
            filemode_on = true;
        } else {
            write(1, "Invalid arguments\n", 18);
            exit(EXIT_FAILURE);
        }
    } else {
        write(1, "Too many arguments\n", 19);
    }

    //declare line_buffer
    size_t len = 512;
    char* line_buf = malloc (len);

    FILE *FPin;
    if (filemode_on) {
        FPin = fopen(argv[2], "r");
        if (FPin == NULL)
            write(1, strerror(errno), strlen(strerror(errno)));
    } else {
        FPin = stdin;
    }

    command_line large_token_buffer;
    command_line small_token_buffer;

    //loop until the file is over
    if (!filemode_on)
        write(1, ">>> ", 4);
    while (getline (&line_buf, &len, stdin) != -1) {
        large_token_buffer = str_filler(line_buf, ";");

        for (int i = 0; large_token_buffer.command_list[i] != NULL; i++) {
            small_token_buffer = str_filler(large_token_buffer.command_list[i], " ");

            for (int j = 0; small_token_buffer.command_list[j] != NULL; j++) {
                if (strcmp(small_token_buffer.command_list[j], "ls") == 0) {
                    // args 0
                    listDir();
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "pwd") == 0) {
                    // args 0
                    showCurrentDir();
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "mkdir") == 0) {
                    // args 1
                    if (small_token_buffer.command_list[j+1] != NULL) {
                        makeDir(small_token_buffer.command_list[j + 1]);

                    } else {
                        write(1, "Invalid args: mkdir requires 1 argument for dir name\n", 53);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "cd") == 0) {
                    // args 1
                    if (small_token_buffer.command_list[j+1] != NULL) {
                        changeDir(small_token_buffer.command_list[j + 1]);
                    } else {
                        write(1, "Invalid args: cd requires 1 argument for dir name\n", 50);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "cp") == 0) {
                    // args 2
                    if (small_token_buffer.command_list[j+1] != NULL &&
                        small_token_buffer.command_list[j+2] != NULL) {
                        copyFile(small_token_buffer.command_list[j+1],
                                 small_token_buffer.command_list[j+2]);
                    } else {
                        write(1, "Invalid args: cp requires 2 arguments src and dest\n", 51);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "mv") == 0) {
                    // args 2
                    if (small_token_buffer.command_list[j+1] != NULL &&
                        small_token_buffer.command_list[j+2] != NULL) {
                        moveFile(small_token_buffer.command_list[j+1],
                                 small_token_buffer.command_list[j+2]);
                    } else {
                        write(1, "Invalid args: mv requires 2 arguments src and dest\n", 51);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "rm") == 0) {
                    // args 1
                    if (small_token_buffer.command_list[j+1] != NULL) {
                        deleteFile(small_token_buffer.command_list[j + 1]);
                    } else {
                        write(1, "Invalid args: rm requires 1 argument for file name\n", 51);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "cat") == 0) {
                    // args 1
                    if (small_token_buffer.command_list[j+1] != NULL) {
                        displayFile(small_token_buffer.command_list[j + 1]);
                    } else {
                        write(1, "Invalid args: cat requires 1 argument for file name\n", 52);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "exit") == 0) {
                    exit(EXIT_SUCCESS);
                } else {
                    write(1, "Error: Unrecognized command!\n", 29);
                    break;
                }
            }
            free_command_line(&small_token_buffer);
            memset (&small_token_buffer, 0, 0);
        }
        if (!filemode_on)
            write(1, ">>> ", 4);

        free_command_line(&large_token_buffer);
        memset (&large_token_buffer, 0, 0);
    }
    free(line_buf);

    if (FPin != stdin) {
        free(FPin);
    }

    exit(EXIT_SUCCESS);
}

