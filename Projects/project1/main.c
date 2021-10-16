#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "command.h"
#include "string_parser.h"

int main(int argc, char *argv[]) {
    bool filemode_on = false;
    listDir();
    displayFile("string_parser.h");
    exit(EXIT_SUCCESS);
}

