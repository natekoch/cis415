#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include "command.h"

void lfcat() {
    char cwd[300];
    char *currentDir;
    char *filename;
    int size = 0;
    char *buf = NULL;
    size_t len = 0;

    write(1, "<<In lfcat(): Step-01: Function called>>\n", 41);
    currentDir = getcwd(cwd, sizeof(cwd));
    printf(" pwd: %s\n", cwd);

    DIR *dr;
    dr = opendir(cwd);
    struct dirent *de;

    if (dr == NULL) {
        printf("Could not open current directory");
        return 0;
    }

    write(1, "<<In lfcat(): Step-02: Listing all files in current dir.\n", 57);

    FILE *FPout = freopen("output.txt", "a+", stdout);

    int fname_len = 0;
    while((de = readdir(dr)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 ||
            strcmp(de->d_name, "..") == 0 ||
            strcmp(de->d_name, "lab2") == 0 ||
            strcmp(de->d_name, "output.txt") == 0) {
            continue;
        }

        write(1, "File: ", 6);
        write(1, de->d_name, strlen(de->d_name));
        write(1, "\n", 1);

        FILE *currFPtr;
        currFPtr = fopen (de->d_name, "r");

        //declare line_buffer
        size_t len = 512;
        char* line_buf = malloc (len);

        while (getline (&line_buf, &len, currFPtr) != -1) {
            write(1, line_buf, strlen(line_buf));
        }
        write(1, "\n--------------------------------------------------------------------------------\n", 82);
        fclose(currFPtr);
        free(line_buf);
    }

    closedir(dr);
    fclose(FPout);
}
