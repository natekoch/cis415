#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "command.h"

void listDir() {
    char *cwd = NULL;
    char *currentDir = NULL;
    currentDir = getcwd(cwd, 3000);

    DIR *dr;
    dr = opendir(currentDir);
    struct dirent *de;

    if (dr == NULL) {
        printf("Could not open current directory");
        exit(EXIT_FAILURE);
    }

    while((de = readdir(dr)) != NULL) {
        write(1, de->d_name, strlen(de->d_name));
        write(1, "\n", 1);
    }

    closedir(dr);
    free(currentDir);
} /*for the ls command*/


void showCurrentDir() {
    char *cwd = NULL;
    char *currentDir = NULL;
    currentDir = getcwd(cwd, 3000);
    write(1, currentDir, strlen(currentDir));
    write(1, "\n", 1);
    free(currentDir);
} /*for the pwd command*/

void makeDir(char *dirName) {
    mkdir(dirName, S_IRWXU);
} /*for the mkdir command*/

void changeDir(char *dirName) {
    chdir(dirName);
} /*for the cd command*/

void copyFile(char *sourcePath, char *destinationPath) {

} /*for the cp command*/

void moveFile(char *sourcePath, char *destinationPath) {

} /*for the mv command*/

void deleteFile(char *filename) {

} /*for the rm command*/

void displayFile(char *filename) {
    int fd;
    fd = open(filename, O_RDONLY);
    char *line_buf;
    ssize_t  count;
    line_buf = malloc(1024);
    while ((count = read(fd, line_buf, 1024)) > 0) {
        write(1, line_buf, count);
    }
    free(line_buf);
    close(fd);
} /*for the cat command*/