#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "command.h"

int main(int argc, char const** argv) {
    (void)argc;
    (void)argv;

    //declare line_buffer
    size_t len = 128;
    char* line_buf = malloc (len);

    //loop until the file is over
    write(1, ">>> ", 4);
    while (getline (&line_buf, &len, stdin) != -1) {
        if (strncmp(line_buf, "lfcat", 5) == 0) {
            //call lfcat
            lfcat();
            break;
        } else {
            write(1, "Error: Unrecognized command!\n", 29);
            write(1, ">>> ", 4);
        }
    }
    free(line_buf);
    exit(EXIT_SUCCESS);
}