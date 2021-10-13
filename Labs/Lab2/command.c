#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include "command.h"

#define UNUSED __attribute__((unused))

void lfcat() {
    char cwd[300];
    char *currentDir UNUSED;
    char *filename;
    char *buf = NULL;
    size_t len = 512;

    write(1, "<<In lfcat(): Step-01: Function called>>\n", 41);
    currentDir = getcwd(cwd, sizeof(cwd));
    printf(" pwd: %s\n", cwd);

    DIR *dr;
    dr = opendir(cwd);
    struct dirent *de;

    if (dr == NULL) {
        printf("Could not open current directory");
        exit(EXIT_FAILURE);
    }

    write(1, "<<In lfcat(): Step-02: Listing all files in current dir.\n", 57);

    FILE *FPout = freopen("output.txt", "a+", stdout);

    buf = malloc(len);

    while((de = readdir(dr)) != NULL) {
        filename = de->d_name;
        if (strcmp(filename, ".") == 0 ||
            strcmp(filename, "..") == 0 ||
            strcmp(filename, "lab2") == 0 ||
            strcmp(filename, "output.txt") == 0) {
            continue;
        }

        write(1, "File: ", 6);
        write(1, filename, strlen(filename));
        write(1, "\n", 1);

        FILE *currFPtr;
        currFPtr = fopen (filename, "r");


        while (getline (&buf, &len, currFPtr) != -1) {
            write(1, buf, strlen(buf));
        }
        write(1, "\n--------------------------------------------------------------------------------\n", 82);
        fclose(currFPtr);

    }
    free(buf);
    closedir(dr);
    fclose(FPout);
}
