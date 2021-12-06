#include <pthread.h>
#include <sched.h>
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

pthread_t tid[NUM_THREADS], bank_tid;

int counter = 0;
int waiting_thread_count = 0;
int alive_thread_count = 0;

double num_transactions_per_thread = 0;

pthread_barrier_t sync_barrier;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; // lock for conditional wake up
pthread_cond_t condition = PTHREAD_COND_INITIALIZER; // condition for conditional wake up
pthread_mutex_t lock_counter = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_alive = PTHREAD_MUTEX_INITIALIZER;

int number_of_accounts = 0;
account* account_list;
int total_lines = 0;

int main(int argc, char *argv[]) {
    char** input_lines = NULL;
    read_input_file(argc, argv, &input_lines);

    create_accounts(&input_lines);

    create_output_directory();

    read_transactions(&input_lines);

    create_output_file();

    for (int i = 0; i < total_lines; i++) {
        free(input_lines[i]);
    }
    free(input_lines);
    free(account_list);

    pthread_barrier_destroy(&sync_barrier);

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
    lines[0] = malloc(sizeof(char*) * total_lines);
    while (getline (&line_buf, &len, FPin) != -1) {
        lines[0][line_number] = strdup(line_buf);
        line_number++;
    }
    free(line_buf);
    if (FPin) fclose(FPin);

    return NULL;
}

void* create_output_directory() {
    FILE* FPacct_out = NULL;
    mkdir("Output", S_IRWXU);
    chdir("Output");
    char output_fname[32];
    for (int i = 0; i < number_of_accounts; i++) {
        sprintf(output_fname, "account%d.txt", i);
        FPacct_out = fopen(output_fname, "w+");
        fprintf(FPacct_out, "account %d:\n", i);
        if (FPacct_out) fclose(FPacct_out);
        FPacct_out = NULL;
        memset(&output_fname[0], 0, sizeof(output_fname));
    }
    
    return NULL;
}

void* update_account_files() {
    FILE* FPacct_out = NULL;

    char output_fname[32];
    for (int i = 0; i < number_of_accounts; i++) {
        sprintf(output_fname, "account%d.txt", i);
        FPacct_out = fopen(output_fname, "a");
        fprintf(FPacct_out, "Current Balance:\t%.2f\n", account_list[i].balance);
        if (FPacct_out) fclose(FPacct_out);
        FPacct_out = NULL;
        memset(&output_fname[0], 0, sizeof(output_fname));
    }
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
    num_transactions_per_thread = ceil(num_transactions/NUM_THREADS);
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

    pthread_barrier_init(&sync_barrier, NULL, NUM_THREADS + 2);

    // Create worker threads
    for (int i = 0; i < NUM_THREADS; i++) {
        error = pthread_create(&(tid[i]), 
                                NULL, 
                                &process_transaction, 
                                (void*)&(split_transactions[i]));
        if (error != 0)
            printf("\nThread can't be created :[%s]", strerror(error));
        else
            alive_thread_count++;
    }
    // Create bank thread
    error = pthread_create(&bank_tid, 
                    NULL, 
                    &update_balance,
                    NULL);
    if (error != 0)
        printf("\nThread can't be created :[%s]", strerror(error));

    pthread_barrier_wait(&sync_barrier);

    void* res;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], &res);
        free(res);
    }
    pthread_join(bank_tid, &res);
    free(res);

    for (int i = 0; i < number_of_accounts; i++) {
        pthread_mutex_destroy(&account_list[i].ac_lock);
    }
    pthread_barrier_destroy(&sync_barrier);

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

    printf("Worker Thread : %lu created but waiting\n", pthread_self());
    pthread_barrier_wait(&sync_barrier);
    printf("Worker Thread : %lu started working\n", pthread_self());
    
    while (transactions[current_transaction].command_list != NULL && current_transaction < num_transactions_per_thread) {
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
        
        // if transaction is transfer
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
                        //printf("Error: invalid password, account number: %s\n", src_account_number);
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
                // update src account
                pthread_mutex_lock(&src_account->ac_lock);
                src_account->balance -= atof(amount);
                src_account->transaction_tracter += atof(amount);
                pthread_mutex_unlock(&src_account->ac_lock);
                //update dest account
                pthread_mutex_lock(&dest_account->ac_lock);
                dest_account->balance += atof(amount);
                pthread_mutex_unlock(&dest_account->ac_lock);
                // update counter
                pthread_mutex_lock(&lock_counter);
                counter++;
                pthread_mutex_unlock(&lock_counter);
            } else {
                //printf("T Error: invalid account info.\n");
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
                        //printf("Error: invalid password, account number: %s\n", src_account_number);
                        break;
                    } else {
                        password_match = 1;
                    }
                }
            }

            if (password_match == 1 && src_found == 1) {
                //pthread_mutex_lock(&src_account->ac_lock);
                //printf("Account: %s: balance: %f\n", src_account_number, src_account->balance);
                //pthread_mutex_unlock(&src_account->ac_lock);
            } else {
                //printf("C Error: invalid account info.\n");
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
                        //printf("Error: invalid password, account number: %s\n", src_account_number);
                        break;
                    } else {
                        password_match = 1;
                    }
                }
            }

            if (password_match == 1 && src_found == 1) {
                // update account
                pthread_mutex_lock(&src_account->ac_lock);
                src_account->balance += atof(amount);
                src_account->transaction_tracter += atof(amount);
                pthread_mutex_unlock(&src_account->ac_lock);
                // update counter
                pthread_mutex_lock(&lock_counter);
                counter++;
                pthread_mutex_unlock(&lock_counter);
            } else {
                //printf("D Error: invalid account info.\n");
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
                        //printf("Error: invalid password, account number: %s\n", src_account_number);
                        break;
                    } else {
                        password_match = 1;
                    }
                }
            }

            if (password_match == 1 && src_found == 1) {
                // update account
                pthread_mutex_lock(&src_account->ac_lock);
                src_account->balance -= atof(amount);
                src_account->transaction_tracter += atof(amount);
                pthread_mutex_unlock(&src_account->ac_lock);
                // update counter
                pthread_mutex_lock(&lock_counter);
                counter++;
                pthread_mutex_unlock(&lock_counter);
            } else {
                //printf("W Error: invalid account info.\n");
            }
        }
        // else invalid transaction
        else {
            //printf("Error: Transaction type is invalid.\n");
        }
        current_transaction++;
        if (counter >= 5000) {
            printf("Worker Thread : %lu waiting, counter reached 5000\n", pthread_self());
            pthread_mutex_lock(&mtx);
            waiting_thread_count++;
            pthread_cond_wait(&condition, &mtx);
            pthread_mutex_unlock(&mtx);
        }
    }
    pthread_mutex_lock(&lock_alive);
    alive_thread_count--;
    pthread_mutex_unlock(&lock_alive);
    printf("Worker Thread : %lu exited, #transactions fulfilled: %d\n", pthread_self(), current_transaction);
    pthread_exit(NULL);

    return NULL;
}

void* update_balance(void* arg) {
    printf("Bank Thread : %lu created but waiting\n", pthread_self());
    pthread_barrier_wait(&sync_barrier);
    printf("Bank Thread : %lu started working\n", pthread_self());
    int update_times = 0;
    while (1) {
        if (waiting_thread_count == alive_thread_count || alive_thread_count == 0) {
            pthread_mutex_lock(&mtx);
            for (int i = 0; i < number_of_accounts; i++) {
                pthread_mutex_lock(&account_list[i].ac_lock);
                account_list[i].balance += 
                    account_list[i].transaction_tracter * account_list[i].reward_rate;
                pthread_mutex_unlock(&account_list[i].ac_lock);
            }
            update_times++;
            update_account_files();
            if (alive_thread_count == 0) {
                printf("Bank Thread : %lu Updated %d times\n", pthread_self(), update_times);
                break;
            } else {
                printf("Bank Thread : %lu Updated time #%d\n", pthread_self(), update_times);
                printf("Bank Thread : %lu resuming worker threads.\n", pthread_self());
                waiting_thread_count = 0;
                counter = 0;
                pthread_cond_broadcast(&condition);
            }
            pthread_mutex_unlock(&mtx);
        } else {
            sched_yield();
        }
    }
    pthread_exit(NULL);
    return NULL;
}