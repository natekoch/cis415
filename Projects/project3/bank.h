#include "string_parser.h"
#ifndef BANK_H_
#define BANK_H_

void* process_transaction(command_line* transaction);
void* update_balance(void* arg);

#endif