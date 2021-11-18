#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string_parser.h"
#include "account.h"
#include "bank.h"

int number_of_accounts = 0;
account* account_list;

int main(int argc, char *argv[]) {
    FILE* FPin = NULL;
    // open file and argument error handling
    if (argc == 2) {
        FPin = fopen(argv[1], "r");
        if (FPin == NULL) {
            perror("fopen: ");
            exit(-1);
        }
    } else {
        printf("invalid arguments\n");
        exit(-1);
    }

    size_t len = 512;
    char* line_buf = malloc (len);
    int line_number = 0;
    command_line line; 
    int current_account = 0;
    int current_account_value = 0; 

    // loop through file
    while (fgets(&line_buf, &len, FP) != -1) {
        if (line_number == 0) {
            number_of_accounts = atoi(line_buf);
            account_list = malloc (number_of_accounts * sizeof(account));
        } else if (line_number > 0 && line_number <= number_of_accounts*5+1) {
            if (current_account_value == 0) { // index number line
                current_account_value++;
                continue;
            } else if (current_account_value == 1) { // account number line
                strncpy(line_buf, account_list[current_account].account_number, 17);
                current_account_value++;
            } else if (current_account_value == 2) { // password
                strncpy(line_buf, account_list[current_account].password,  9);
                current_account_value++;
            } else if (current_account_value == 3) { // initial balance
                account_list[current_account].balance = atof(line_buf);
                current_account_value++;
            } else if (current_account_value == 4) { // reward rate
                account_list[current_account].reward_rate = atof(line_buf);
                current_account++;
                current_account_value = 0;
            }
        } else {
            line = str_filler(line_buf, " ");
            process_transaction(&line);
            //update_balance();
            free_command_line(&line);
        }
        line_number++;
    }

    FILE* FPout = NULL;
    FPout = fopen("output.txt", "w+");
    for (int i = 0; i < number_of_accounts; i++) {
        fprintf(FPout, "%d balance:  %f\n\n", i, account_list[i].balance);
    }

    free(account_list);
    free(line_buf);
    fclose(FPin);
    fclose(FPout);

    exit(EXIT_SUCCESS);
}

void* process_transaction(command_line* transaction) {
    // check for transaction type
    char type[1];
    strncpy(type, transaction->command_list[0], 1);
    account src_account;
    account dest_account;
    char* src_account_number;
    char* password; 
    char* amount; 
    char* dest_account_number;
    int src_found = 0;
    int dest_found = 0;
    int password_match = 0; 

    // if transaction is transfer
    if (strncmp(type, "T", 1) == 0) {
        src_account_number = transaction->command_list[1];
        password = transaction->command_list[2];
        dest_account_number = transaction->command_list[3];
        amount = transaction->command_list[4];

        for (int i = 0; i < number_of_accounts; i++) {
            if (src_found != 1 && strncmp(account_list[i].account_number, src_account_number, 17) == 0) {
                src_account = account_list[i];
                src_found = 1;
                if (strncmp(account_list[i].password, password, 9) != 0) {
                    printf("Error: invalid password, account number: %s\n", src_account_number);
                    break;
                } else {
                    password_match = 1;
                }
            }
            if (dest_found != 1 && strncmp(account_list[i].account_number, dest_account_number, 17) == 0) {
                dest_account = account_list[i];
                dest_found = 1;
            }
        }

        if (password_match && src_found && dest_found) {
            account_list[atoi(src_account_number)].balance -= atof(amount);
            account_list[atoi(dest_account_number)].balance += atof(amount);
        } else {
            printf("Error: invalid account info.\n");
        }
    }
    // if transaction is check balance
    else if (strncmp(type, "C", 1) == 0) {
        src_account_number = transaction->command_list[1];
        password = transaction->command_list[2];

        for (int i = 0; i < number_of_accounts; i++) {
            if (src_found != 1 && strncmp(account_list[i].account_number, src_account_number, 17) == 0) {
                src_account = account_list[i];
                src_found = 1;
                if (strncmp(account_list[i].password, password, 9) != 0) {
                    printf("Error: invalid password, account number: %s\n", src_account_number);
                    break;
                } else {
                    password_match = 1;
                }
            }
        }

        if (password_match && src_found) {
            printf("Account: %s: balance: %f\n", src_account_number, account_list[atoi(src_account_number)].balance);
        } else {
            printf("Error: invalid account info.\n");
        }
    }
    // if transaction is deposit
    else if (strncmp(type, "D", 1) == 0) {
        src_account_number = transaction->command_list[1];
        password = transaction->command_list[2];
        amount = transaction->command_list[3];

        for (int i = 0; i < number_of_accounts; i++) {
            if (src_found != 1 && strncmp(account_list[i].account_number, src_account_number, 17) == 0) {
                src_account = account_list[i];
                src_found = 1;
                if (strncmp(account_list[i].password, password, 9) != 0) {
                    printf("Error: invalid password, account number: %s\n", src_account_number);
                    break;
                } else {
                    password_match = 1;
                }
            }
        }

        if (password_match && src_found) {
            account_list[atoi(src_account_number)].balance += atof(amount);
        } else {
            printf("Error: invalid account info.\n");
        }
    }
    // if transaction is withdraw
    else if (strncmp(type, "W", 1) == 0) {
        src_account_number = transaction->command_list[1];
        password = transaction->command_list[2];
        amount = transaction->command_list[3];

        for (int i = 0; i < number_of_accounts; i++) {
            if (src_found != 1 && strncmp(account_list[i].account_number, src_account_number, 17) == 0) {
                src_account = account_list[i];
                src_found = 1;
                if (strncmp(account_list[i].password, password, 9) != 0) {
                    printf("Error: invalid password, account number: %s\n", src_account_number);
                    break;
                } else {
                    password_match = 1;
                }
            }
        }

        if (password_match && src_found) {
            account_list[atoi(src_account_number)].balance -= atof(amount);
        } else {
            printf("Error: invalid account info.\n");
        }
    }
    // else
    else {
        printf("Error: Transaction type is invalid.\n");
    }
}

void* update_balance(void* arg) {
    
}