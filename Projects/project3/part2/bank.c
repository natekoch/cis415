#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/stat.h>
#include "string_parser.h"
#include "account.h"
#include "bank.h"

#define NUM_THREADS 10

pthread_t tid[NUM_THREADS];

int number_of_accounts = 0;
account* account_list;
int total_lines = 0;

//temp test values;
int num_t = 0;
int num_d = 0;
int num_w = 0;
int num_c = 0;
int num_f = 0;

int main(int argc, char *argv[]) {
    char** input_lines = NULL;
    read_input_file(argc, argv, &input_lines);

    create_accounts(&input_lines);
    
    read_transactions(&input_lines);

    create_output_directory();

    printf("T: %d\n", num_t);
    printf("D: %d\n", num_d);
    printf("C: %d\n", num_c);
    printf("W: %d\n", num_w);
    printf("F: %d\n", num_f);

    for (int i = 0; i < total_lines; i++) {
        free(input_lines[i]);
    }
    free(input_lines);
    free(account_list);

    exit(EXIT_SUCCESS);
}

void* read_input_file(int num_args, char** arg_list, char*** lines) {
    FILE* FPin = NULL;
    size_t len = 512;
    char* line_buf = malloc (len);

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
    int current_account = 0;
    int current_account_value = 0;
    char* stripped_line;

    // loop through file
    /*
    while (getline(&line_buf, &len, FPin) != -1) {
        total_lines++;
    }
    if (FPin) fclose(FPin);
    // reopen the file to reset the pointer
    FPin = fopen(arg_list[1], "r");
    if (FPin == NULL) {
        perror("fopen: ");
        exit(-1);
    }
    *lines = malloc(sizeof(char*) * total_lines);
    while (getline (&line_buf, &len, FPin) != -1) {
        *lines[line_number] = strdup(line_buf);
        line_number++;
    }
    */
    *lines = malloc(sizeof(char*));
    while (getline (&line_buf, &len, FPin) != -1) {
        if (line_number != 0) {
            *lines = realloc(*lines, (line_number + 1) * sizeof(char *));
        }
        lines[0][line_number] = strdup(line_buf);
        line_number++;
        total_lines++;
    }
    if (FPin) fclose(FPin);

    return NULL;
}

void* create_output_directory() {
    FILE* FPacct_out = NULL;
    FILE* FPout = NULL;
    mkdir("Output", S_IRWXU);
    chdir("Output");
    FPout = fopen("output.txt", "w+");
    char output_fname[32];
    for (int i = 0; i < number_of_accounts; i++) {
        sprintf(output_fname, "account%d.txt", i);
        FPacct_out = fopen(output_fname, "w+");
        fprintf(FPout, "%d balance:\t%.2f\n\n", i, account_list[i].balance);
        fprintf(FPacct_out, "account %d:\n", i);
        fprintf(FPacct_out, "Current Balance:\t%.2f\n", account_list[i].balance);
        if (FPacct_out) fclose(FPacct_out);
        FPacct_out = NULL;
        memset(&output_fname[0], 0, sizeof(output_fname));
    }
    if (FPout) fclose(FPout);
    return NULL;
}

void* create_accounts(char*** input_lines) {
    char** lines = *input_lines;
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
                pthread_mutex_init(&account_list[current_account].ac_lock, NULL);
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
    double num_transactions = total_lines-(number_of_accounts*5)-1;
    int current_thread = 0;
    char** lines = *input_lines;
    int start_line = number_of_accounts*5+1;
    int current_line = start_line;
    int transaction_line = 0;
    double num_transactions_per_thread = ceil(num_transactions/NUM_THREADS);
    int error;
    command_line** split_transactions = malloc(sizeof(command_line*) * NUM_THREADS);

    // populate split_transactions giving each thread the same amount of threads each. 
    while (lines[current_line] != NULL) {
        if (transaction_line == 0) {
            split_transactions[current_thread] = malloc (sizeof(command_line) * num_transactions_per_thread);
            transaction_line++;
        } else if (transaction_line <= num_transactions_per_thread) {
            split_transactions[current_thread][transaction_line-1] = 
                str_filler(lines[current_line], " ");
            transaction_line++;
            current_line++;
        } else {
            transaction_line = 0;
            current_thread++;
        }
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        error = pthread_create(&(tid[i]), 
                                NULL, 
                                &process_transaction, 
                                (void*)&(split_transactions[i]));
    }
    void* res;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], &res);
        free(res);
    }

    pthread_t bank_tid;
    pthread_create(&bank_tid, 
                    NULL, 
                    &update_balance,
                    NULL);
    pthread_join(bank_tid, &res);
    free(res);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        for (int j = 0; j < num_transactions_per_thread; j++) {
            if (split_transactions[i][j].command_list != NULL)
                free_command_line(&split_transactions[i][j]);
        }
        free(split_transactions[i]);
    } 
    free(split_transactions);

    return NULL;
}

void* process_transaction(void* arg) {
    command_line** transactions_ptr = (command_line**) arg;
    command_line* transactions = *transactions_ptr;
    //command_line* transaction;
    int current_transaction = 0;
    account* src_account;
    account* dest_account;
    char* src_account_number;
    char* password; 
    char* amount; 
    char* dest_account_number;
    int src_found = 0;
    int dest_found = 0;
    int password_match = 0; 
    
    while (transactions[current_transaction].command_list != NULL) {
        // Reset variables
        src_account = NULL;
        dest_account = NULL;
        src_account_number = NULL;
        password = NULL; 
        amount = NULL; 
        dest_account_number = NULL;
        src_found = 0;
        dest_found = 0;
        password_match = 0; 
        //transaction = &transactions[current_transaction];
        
        // if transaction is transfer
        printf("%s\n", transactions[current_transaction].command_list[0]);
        if (strncmp(transactions[current_transaction].command_list[0], "T", 1) == 0) {
            src_account_number = transactions[current_transaction].command_list[1];
            password = transactions[current_transaction].command_list[2];
            dest_account_number = transactions[current_transaction].command_list[3];
            amount = transactions[current_transaction].command_list[4];

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
                if (dest_found != 1 && strncmp(account_list[i].account_number, dest_account_number, 17) == 0) {
                    dest_account = &account_list[i];
                    dest_found = 1;
                }
            }

            if (password_match == 1 && src_found == 1 && dest_found == 1) {
                pthread_mutex_lock(&src_account->ac_lock);
                src_account->balance -= atof(amount);
                src_account->transaction_tracter += atof(amount);
                pthread_mutex_unlock(&src_account->ac_lock);
                pthread_mutex_lock(&dest_account->ac_lock);
                dest_account->balance += atof(amount);
                pthread_mutex_unlock(&dest_account->ac_lock);
                num_t++;
            } else {
                printf("T Error: invalid account info.\n");
                num_f++;
            }
        }
        // if transaction is check balance
        else if (strncmp(transactions[current_transaction].command_list[0], "C", 1) == 0) {
            src_account_number = transactions[current_transaction].command_list[1];
            password = transactions[current_transaction].command_list[2];

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
                pthread_mutex_lock(&src_account->ac_lock);
                printf("Account: %s: balance: %f\n", src_account_number, src_account->balance);
                pthread_mutex_unlock(&src_account->ac_lock);
                num_c++;
            } else {
                printf("C Error: invalid account info.\n");
                num_f++;
            }
        }
        // if transaction is deposit
        else if (strncmp(transactions[current_transaction].command_list[0], "D", 1) == 0) {
            src_account_number = transactions[current_transaction].command_list[1];
            password = transactions[current_transaction].command_list[2];
            amount = transactions[current_transaction].command_list[3];

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
                pthread_mutex_lock(&src_account->ac_lock);
                src_account->balance += atof(amount);
                src_account->transaction_tracter += atof(amount);
                pthread_mutex_unlock(&src_account->ac_lock);
                num_d++;
            } else {
                printf("D Error: invalid account info.\n");
                num_f++;
            }
        }
        // if transaction is withdraw
        else if (strncmp(transactions[current_transaction].command_list[0], "W", 1) == 0) {
            src_account_number = transactions[current_transaction].command_list[1];
            password = transactions[current_transaction].command_list[2];
            amount = transactions[current_transaction].command_list[3];

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
                pthread_mutex_lock(&src_account->ac_lock);
                src_account->balance -= atof(amount);
                src_account->transaction_tracter += atof(amount);
                pthread_mutex_unlock(&src_account->ac_lock);
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
        current_transaction++;
    }
    pthread_exit(NULL);

    return NULL;
}

void* update_balance(void* arg) {
    for (int i = 0; i < number_of_accounts; i++) {
        account_list[i].balance += 
            account_list[i].transaction_tracter * account_list[i].reward_rate;
    }
    return NULL;
}