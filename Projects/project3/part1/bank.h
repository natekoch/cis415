#include "string_parser.h"
#ifndef BANK_H_
#define BANK_H_

void* process_transaction(void* arg);
void* update_balance(void* arg);
void* create_output_directory();

#endif