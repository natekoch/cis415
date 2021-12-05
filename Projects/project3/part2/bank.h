#include "string_parser.h"
#ifndef BANK_H_
#define BANK_H_

void* process_transaction(void* arg);
void* update_balance(void* arg);
char** read_input_file(int num_args, char** arg_list);
void* create_accounts(char*** input_lines);
void* read_transactions(char*** input_lines);
void* create_output_directory();
void* spawn_threads();

#endif