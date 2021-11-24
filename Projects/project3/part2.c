#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string_parser.h"
#include "account.h"
#include "bank.h"

#define NUM_THREADS 10

int number_of_accounts = 0;
account* account_list;
int num_transactions = 0;

//temp test values;
int num_t = 0;
int num_d = 0;
int num_w = 0;
int num_c = 0;
int num_f = 0;

int main(int argc, char *argv[]) {
    char** input_lines;
    input_lines = read_input_file(argc, argv);

    create_accounts(&input_lines);
    
    read_transactions(&input_lines);
    
    update_balance(NULL);

    create_output_file();

    free(input_lines);
    free(account_list);

    exit(EXIT_SUCCESS);
}

char** read_input_file(int num_args, char** arg_list) {
    FILE* FPin = NULL;
    size_t len = 512;
    char* line_buf = malloc (len);
    char** output_strings = malloc(sizeof(char*));

    // open file and argument error handling
    if (num_args == 2) {
        FPin = fopen(arg_list[1], "r");
        if (FPin == NULL) {
            perror("fopen: ");
            exit(-1);
        }
    } else {
        printf("invalid arguments\n");
        exit(-1);
    }

    int line_number = 0;
    command_line line; 
    int current_account = 0;
    int current_account_value = 0;
    char* stripped_line;

    // loop through file
    while (getline (&line_buf, &len, FPin) != -1) {
        if (line_number != 0) {
            output_strings = realloc(output_strings, (line_number + 1) * sizeof(char *));
        }
        output_strings[line_number] = strdup(line_buf);
        line_number++;
    }

    if (FPin) fclose(FPin);

    return output_strings;
}

void* create_output_file() {
    FILE* FPout = NULL;
    FPout = fopen("output.txt", "w+");
    for (int i = 0; i < number_of_accounts; i++) {
        fprintf(FPout, "%d balance:\t%.2f\n\n", i, account_list[i].balance);
    }
    if (FPout) fclose(FPout);
    return NULL;
}

void* create_accounts(char*** input_lines) {
    char** lines = *input_lines;
    printf("%s", lines[0]);
    int current_line = 0;
    int current_account = 0;
    int current_account_value = 0;
    char* stripped_line;
    while(current_line <= number_of_accounts*5) {
        if (current_line == 0) {
            number_of_accounts = atoi(*input_lines[current_line]);
            account_list = malloc (number_of_accounts * sizeof(account));
        } else {
            if (current_account_value == 0) { // index number linesssss
                current_account_value++;
                account_list[current_account].transaction_tracter = 0;
            } else if (current_account_value == 1) { // account number line
                stripped_line = strndup(lines[current_line], strlen(lines[current_line]));
                stripped_line[strlen(stripped_line)-1] = '\0';
                strncpy(account_list[current_account].account_number, stripped_line, 17);
                current_account_value++;
                free(stripped_line);
            } else if (current_account_value == 2) { // password
                stripped_line = strndup(lines[current_line], strlen(lines[current_line]));
                stripped_line[strlen(stripped_line)-1] = '\0';
                strncpy(account_list[current_account].password, stripped_line, 9);
                current_account_value++;
                free(stripped_line);
            } else if (current_account_value == 3) { // initial balance
                account_list[current_account].balance = atof(lines[current_line]);
                current_account_value++;
            } else if (current_account_value == 4) { // reward rate
                account_list[current_account].reward_rate = atof(lines[current_line]);
                current_account++;
                current_account_value = 0;
            }
        }
        current_line++;
    }
    return NULL;
}

void* read_transactions(char*** input_lines) {
    char** lines = *input_lines;
    int current_line = number_of_accounts*5+1;
    command_line transaction;
    while (lines[current_line] != NULL) {
        transaction = str_filler(lines[current_line], " ");
        process_transaction(&transaction);
        free_command_line(&transaction);
        current_line++;
    }
}

void* process_transaction(command_line* transaction) {
    // check for transaction type
    char type[1];
    strncpy(type, transaction->command_list[0], 1);
    account* src_account;
    account* dest_account;
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
                src_account = &account_list[i];
                src_found = 1;
                if (strncmp(account_list[i].password, password, 9) != 0) {
                    printf("%s - %s\n", account_list[i].password, password);
                    printf("Error: invalid password, account number: %s\n", src_account_number);
                    break;
                } else {
                    password_match = 1;
                }
            }
            if (dest_found != 1 && strncmp(account_list[i].account_number, dest_account_number, 17) == 0) {
                dest_account = &account_list[i];
                dest_found = 1;
            }
        }

        if (password_match == 1 && src_found == 1 && dest_found == 1) {
            src_account->balance -= atof(amount);
            src_account->transaction_tracter += atof(amount);
            dest_account->balance += atof(amount);
            num_t++;
        } else {
            printf("pswd: %d; sf: %d; df: %d\n", password_match, src_found, dest_found);
            printf("T Error: invalid account info.\n");
            num_f++;
        }
    }
    // if transaction is check balance
    else if (strncmp(type, "C", 1) == 0) {
        src_account_number = transaction->command_list[1];
        password = transaction->command_list[2];

        for (int i = 0; i < number_of_accounts; i++) {
            if (src_found != 1 && strncmp(account_list[i].account_number, src_account_number, 17) == 0) {
                src_account = &account_list[i];
                src_found = 1;
                if (strncmp(account_list[i].password, password, 9) != 0) {
                    printf("Error: invalid password, account number: %s\n", src_account_number);
                    break;
                } else {
                    password_match = 1;
                }
            }
        }

        if (password_match == 1 && src_found == 1) {
            printf("Account: %s: balance: %f\n", src_account_number, src_account->balance);
            num_c++;
        } else {
            printf("C Error: invalid account info.\n");
            num_f++;
        }
    }
    // if transaction is deposit
    else if (strncmp(type, "D", 1) == 0) {
        src_account_number = transaction->command_list[1];
        password = transaction->command_list[2];
        amount = transaction->command_list[3];

        for (int i = 0; i < number_of_accounts; i++) {
            if (src_found != 1 && strncmp(account_list[i].account_number, src_account_number, 17) == 0) {
                src_account = &account_list[i];
                src_found = 1;
                if (strncmp(account_list[i].password, password, 9) != 0) {
                    printf("Error: invalid password, account number: %s\n", src_account_number);
                    break;
                } else {
                    password_match = 1;
                }
            }
        }

        if (password_match == 1 && src_found == 1) {
            src_account->balance += atof(amount);
            src_account->transaction_tracter += atof(amount);
            num_d++;
        } else {
            printf("D Error: invalid account info.\n");
            num_f++;
        }
    }
    // if transaction is withdraw
    else if (strncmp(type, "W", 1) == 0) {
        src_account_number = transaction->command_list[1];
        password = transaction->command_list[2];
        amount = transaction->command_list[3];

        for (int i = 0; i < number_of_accounts; i++) {
            if (src_found != 1 && strncmp(account_list[i].account_number, src_account_number, 17) == 0) {
                src_account = &account_list[i];
                src_found = 1;
                if (strncmp(account_list[i].password, password, 9) != 0) {
                    printf("Error: invalid password, account number: %s\n", src_account_number);
                    break;
                } else {
                    password_match = 1;
                }
            }
        }

        if (password_match == 1 && src_found == 1) {
            src_account->balance -= atof(amount);
            src_account->transaction_tracter += atof(amount);
            num_w++;
        } else {
            printf("W Error: invalid account info.\n");
            num_f++;
        }
    }
    // else invalid transaction
    else {
        printf("Error: Transaction type is invalid.\n");
        num_f++;
    }

    return (void*) transaction;
}

void* update_balance(void* arg) {
    for (int i = 0; i < number_of_accounts; i++) {
        account_list[i].balance += 
            account_list[i].transaction_tracter * account_list[i].reward_rate;
    }
    return NULL;
}