#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "command.h"
#include "string_parser.h"

int main(int argc, char *argv[]) {
    // handle arguments and files passed through argv at runtime
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

    //declare line_buffer for reading from file
    size_t len = 512;
    char* line_buf = malloc (len);

    // handle what the input file is and output stream
    FILE *FPin = NULL;
    FILE *FPout = NULL;
    if (filemode_on) {
        FPout = freopen("output.txt", "w+", stdout);
        FPin = fopen(argv[2], "r");
        if (FPin == NULL) {
            write(1, strerror(errno), strlen(strerror(errno)));
            write(1, "\n", 1);
            free(line_buf);
            fclose(FPout);
            exit(EXIT_FAILURE);
        }
    } else {
        FPin = stdin;
    }
    // declare command_line buffers
    command_line large_token_buffer;
    command_line small_token_buffer;
    command_line tiny_token_buffer;

    char *destPath = NULL;

    //loop until the file is over
    if (!filemode_on)
        write(1, ">>> ", 4);
    while (getline (&line_buf, &len, FPin) != -1) {
        // separate line by ;
        large_token_buffer = str_filler(line_buf, ";");

        for (int i = 0; large_token_buffer.command_list[i] != NULL; i++) {
            // separate line by " "
            small_token_buffer = str_filler(large_token_buffer.command_list[i], " ");

            for (int j = 0; small_token_buffer.command_list[j] != NULL; j++) {
                if (strcmp(small_token_buffer.command_list[j], "ls") == 0) {
                    // args 0
                    if (small_token_buffer.command_list[j+1] == NULL) {
                        listDir();
                    } else {
                        write(1, "Error! Unsupported parameters for command: ls\n", 46);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "pwd") == 0) {
                    // args 0
                    if (small_token_buffer.command_list[j+1] == NULL) {
                        showCurrentDir();
                    } else {
                        write(1, "Error! Unsupported parameters for command: pwd\n", 47);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "mkdir") == 0) {
                    // args 1
                    if (small_token_buffer.command_list[j+1] != NULL) {
                        makeDir(small_token_buffer.command_list[j + 1]);
                    } else {
                        write(1, "Error! Unsupported parameters for command: mkdir\n", 49);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "cd") == 0) {
                    // args 1
                    if (small_token_buffer.command_list[j+1] != NULL) {
                        changeDir(small_token_buffer.command_list[j + 1]);
                    } else {
                        write(1, "Error! Unsupported parameters for command: cd\n", 46);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "cp") == 0) {
                    // args 2
                    if (small_token_buffer.command_list[j+1] != NULL &&
                        small_token_buffer.command_list[j+2] != NULL) {
                        // get just the file name from the source path
                        tiny_token_buffer = str_filler(small_token_buffer.command_list[j+1], "/");
                        struct stat s;
                        if (stat(small_token_buffer.command_list[j+2], &s) == 0) {
                            if (s.st_mode & S_IFDIR) {
                                destPath = malloc(strlen(small_token_buffer.command_list[j+1]) +
                                        strlen(small_token_buffer.command_list[j+2]));
                                // add file name to the destination path
                                strcpy(destPath, small_token_buffer.command_list[j+2]);
                                strcat(destPath, "/");
                                strcat(destPath,
                                       tiny_token_buffer.command_list[tiny_token_buffer.num_token-1]);
                                copyFile(small_token_buffer.command_list[j+1],
                                         destPath);
                                free(destPath);
                            } else {
                                copyFile(small_token_buffer.command_list[j+1],
                                         small_token_buffer.command_list[j+2]);
                            }
                        }
                    } else {
                        write(1, "Error! Unsupported parameters for command: cp\n", 46);
                    }
                    // free the command line and set memory to 0
                    free_command_line(&tiny_token_buffer);
                    memset (&tiny_token_buffer, 0, 0);
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "mv") == 0) {
                    // args 2
                    if (small_token_buffer.command_list[j+1] != NULL &&
                        small_token_buffer.command_list[j+2] != NULL) {
                        // get just the file name from the source path
                        tiny_token_buffer = str_filler(small_token_buffer.command_list[j+1], "/");
                        struct stat s;
                        if (stat(small_token_buffer.command_list[j+2], &s) == 0) {
                            if (s.st_mode & S_IFDIR) {
                                destPath = malloc(strlen(small_token_buffer.command_list[j+1]) +
                                                  strlen(small_token_buffer.command_list[j+2]));
                                // add file name to the destination path
                                strcpy(destPath, small_token_buffer.command_list[j+2]);
                                strcat(destPath, "/");
                                strcat(destPath,
                                       tiny_token_buffer.command_list[tiny_token_buffer.num_token-1]);
                                moveFile(small_token_buffer.command_list[j+1],
                                         destPath);
                                free(destPath);
                            } else {
                                moveFile(small_token_buffer.command_list[j+1],
                                         small_token_buffer.command_list[j+2]);
                            }
                        }
                    } else {
                        write(1, "Error! Unsupported parameters for command: mv\n", 46);
                    }
                    // free the command line and set memory to 0
                    free_command_line(&tiny_token_buffer);
                    memset (&tiny_token_buffer, 0, 0);
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "rm") == 0) {
                    // args 1
                    if (small_token_buffer.command_list[j+1] != NULL) {
                        deleteFile(small_token_buffer.command_list[j + 1]);
                    } else {
                        write(1, "Error! Unsupported parameters for command: rm\n", 46);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "cat") == 0) {
                    // args 1
                    if (small_token_buffer.command_list[j+1] != NULL) {
                        displayFile(small_token_buffer.command_list[j + 1]);
                    } else {
                        write(1, "Error! Unsupported parameters for command: cat\n", 47);
                    }
                    break;
                } else if (strcmp(small_token_buffer.command_list[j], "exit") == 0) {
                    // exit the shell
                    exit(EXIT_SUCCESS);
                } else {
                    // default option for unrecognized commands given
                    write(1, "Error! Unrecognized command:", 28);
                    write(1, small_token_buffer.command_list[j], strlen(small_token_buffer.command_list[j]));
                    write(1, "\n", 1);
                    break;
                }
            }
            // free the command line and set memory to 0
            free_command_line(&small_token_buffer);
            memset (&small_token_buffer, 0, 0);
        }
        if (!filemode_on)
            write(1, ">>> ", 4);
        // free the command line and set memory to 0
        free_command_line(&large_token_buffer);
        memset (&large_token_buffer, 0, 0);
    }

    // free and close memory and files respectively
    free(line_buf);
    if (FPin != stdin && FPin != NULL)
        fclose(FPin);
    if (filemode_on && FPout != NULL)
        fclose(FPout);

    // exit here runs for when in filemode with no exit called
    // and EOF is reached
    exit(EXIT_SUCCESS);
}

