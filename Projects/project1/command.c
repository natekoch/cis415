#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "command.h"

void listDir() {
    getcwd(cwd, sizeof(cwd));
    write(1, cwd, sizeof(cwd));
    write(1, "\n", 1);
} /*for the ls command*/


void showCurrentDir() {

} /*for the pwd command*/

void makeDir(char *dirName) {

} /*for the mkdir command*/

void changeDir(char *dirName) {

} /*for the cd command*/

void copyFile(char *sourcePath, char *destinationPath) {

} /*for the cp command*/

void moveFile(char *sourcePath, char *destinationPath) {

} /*for the mv command*/

void deleteFile(char *filename) {

} /*for the rm command*/

