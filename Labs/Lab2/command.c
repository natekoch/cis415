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

    closedir(dr);
}
