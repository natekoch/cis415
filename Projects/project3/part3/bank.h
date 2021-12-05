#include "string_parser.h"
#ifndef BANK_H_
#define BANK_H_

void* process_transaction(void* arg);
void* update_balance(void* arg);
void* read_input_file(int num_args, char** arg_list, char*** lines);
void* create_accounts(char*** input_lines);
void* read_transactions(char*** input_lines);
void* create_output_directory();
void* update_account_files();
void* create_output_file();
void* spawn_threads();

typedef struct {
    command_line* transactions;
}transaction_arg;

#endif