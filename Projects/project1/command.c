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
    // get the current working directory
    currentDir = getcwd(cwd, 3000);

    DIR *dr;
    // open the current working directory
    dr = opendir(currentDir);
    struct dirent *de;

    //read each file name in the directory
    while((de = readdir(dr)) != NULL) {
        write(1, de->d_name, strlen(de->d_name));
        write(1, " ", 1);
    }
    write(1, "\n", 1);

    closedir(dr);
    free(currentDir);
} /*for the ls command*/

void showCurrentDir() {
    char *cwd = NULL;
    char *currentDir = NULL;
    // get the current working directory then write to stdout
    currentDir = getcwd(cwd, 3000);
    write(1, currentDir, strlen(currentDir));
    write(1, "\n", 1);
    free(currentDir);
} /*for the pwd command*/

void makeDir(char *dirName) {
    // make the directory with read write and execute permissions
    if (mkdir(dirName, S_IRWXU) == -1) {
        // handle any errors
        write(1, strerror(errno), strlen(strerror(errno)));
        write(1, "\n", 1);
    }
} /*for the mkdir command*/

void changeDir(char *dirName) {
    // change the current directory
    if (chdir(dirName) == -1) {
        // handle any errors
        write(1, strerror(errno), strlen(strerror(errno)));
        write(1, "\n", 1);
    }
} /*for the cd command*/

void copyFile(char *sourcePath, char *destinationPath) {
    char line_buf[1024];
    ssize_t  count;
    // open the source file
    int fd_read = open(sourcePath, O_RDONLY);
    if (fd_read == -1) {
        // handle open errors
        write(1, strerror(errno), strlen(strerror(errno)));
        write(1, "\n", 1);
        return;
    }
    // open the destination file and if it doesn't exist create a new one
    int fd_write = open(destinationPath, O_CREAT | O_WRONLY, 0666);
    // read from source and write to destination
    while ((count = read(fd_read, line_buf,  1024)) > 0) {
        write(fd_write, line_buf, count);
    }

    close(fd_read);
    close(fd_write);
} /*for the cp command*/

void moveFile(char *sourcePath, char *destinationPath) {
    char line_buf[1024];
    ssize_t  count;
    // open the source file
    int fd_read = open(sourcePath, O_RDONLY);
    if (fd_read == -1) {
        // handle any errors
        write(1, strerror(errno), strlen(strerror(errno)));
        write(1, "\n", 1);
        return;
    }
    // open the destination file and if it doesn't exist create a new one
    int fd_write = open(destinationPath, O_CREAT | O_WRONLY, 0666);
    if (fd_write == -1) {
        // handle any errors
        write(1, strerror(errno), strlen(strerror(errno)));
        write(1, "\n", 1);
    }
    // read from source and write to destination
    while ((count = read(fd_read, line_buf,  1024)) > 0) {
        write(fd_write, line_buf, count);
    }

    close(fd_read);
    close(fd_write);
    // delete or unlink the source file
    unlink(sourcePath);
} /*for the mv command*/

void deleteFile(char *filename) {
    // delete or unlink the file
    if (unlink(filename) == -1) {
        // handle any errors
        write(1, strerror(errno), strlen(strerror(errno)));
        write(1, "\n", 1);
    }
} /*for the rm command*/

void displayFile(char *filename) {
    int fd;
    // open the file with read only permissions
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        // handle any errors for file
        write(1, strerror(errno), strlen(strerror(errno)));
        write(1, "\n", 1);
        return;
    }

    char line_buf[1024];
    ssize_t  count;
    // read from file and write to stdout
    while ((count = read(fd, line_buf, 1024)) > 0) {
        write(1, line_buf, count);
    }

    close(fd);
} /*for the cat command*/