/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GNU_SOURCE

int count_token (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	Check for NULL string
	*	#2.	iterate through string counting tokens
	*		Cases to watchout for
	*			a.	string start with delimeter
	*			b. 	string end with delimeter
	*			c.	account NULL for the last token
	*	#3. return the number of token (note not number of delimeter)
	*/
    char *token;
    int count = 0;

    while ((token = strtok_r(buf, delim, &buf))) {
        count++;
    }
    return count;
}

command_line str_filler (char* buf, const char* delim)
{
	//TODO：
	/*
	*	#1.	create command_line variable to be filled and returned
	*	#2.	count the number of tokens with count_token function, set num_token. 
    *           one can use strtok_r to remove the \n at the end of the line.
	*	#3. malloc memory for token array inside command_line variable
	*			based on the number of tokens.
	*	#4.	use function strtok_r to find out the tokens 
    *   #5. malloc each index of the array with the length of tokens,
	*			fill command_list array with tokens, and fill last spot with NULL.
	*	#6. return the variable.
	*/
    char *large, *small;
    command_line *command;
    command = (command_line*)malloc(sizeof(command_line));

    large = strtok_r(buf, "\n", &buf);
    command->num_token = count_token(large, delim);

    command->command_list = (char **)malloc(sizeof(char*)*command->num_token);

    for (int i = 0; i < command->num_token; i++) {
        small = strtok_r(large, delim, &large);
        command->command_list[i] = malloc(sizeof(char)*sizeof(small));
        command->command_list[i] = small;
    }

    return *command;
}


void free_command_line(command_line* command)
{
	//TODO：
	/*
	*	#1.	free the array base num_token
	*/
    for (int i = 0; i < command->num_token; i++) {
        free(command->command_list[i]);
    }
    free(command->command_list);
    free(command);
}
