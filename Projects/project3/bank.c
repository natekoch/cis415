#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string_parser.h"
#include "bank.h"

int main(int argc, char *argv[]) {
    FILE* FP = NULL;
    // open file and argument error handling
    if (argc == 2) {
        FP = fopen(argv[1], "r");
        if (FP == NULL) {
            perror("fopen: ");
            exit(-1);
        }
    } else {
        printf("invalid arguments\n");
        exit(-1);
    }

    fclose(FP);
}

void* process_transaction(void* arg) {

}

void* update_balance(void* arg) {

}