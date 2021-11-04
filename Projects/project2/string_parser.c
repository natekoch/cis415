/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdlib.h>
#include <string.h>
#include "string_parser.h"

#define _GNU_SOURCE

int count_token (char* buf, const char* delim)
{
	/*
	*	#1.	Check for NULL string
	*	#2.	iterate through string counting tokens
	*		Cases to watchout for
	*			a.	string start with delimeter
	*			b. 	string end with delimeter
	*			c.	account NULL for the last token
	*	#3. return the number of token (note not number of delimeter)
	*/

    int count = 0;

    //check for NULL string
    if (buf == NULL)
        return count;

    //iterate through string counting tokens
    for (unsigned long i = 0; i < strlen(buf); i++) {
        if (buf[i] == *delim) {
            count++;
        }
    }
    /* SPECIAL CASES */
    // acount for NULL token
    count++;
    // delim at beginning of string
    if (buf[0] == *delim)
        count--;
    // delim at the end of string
    if (buf[strlen(buf)-1] == *delim)
        count--;

    return count;
}

command_line str_filler (char* buf, const char* delim)
{
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
    char *cp_buf;
    char *saveptr1, *saveptr2;

    // create command_line variable to be filled
    command_line command;

    cp_buf = strdup(buf);
    // remove newline character at end of line
    large = strtok_r(cp_buf, "\n", &saveptr1);
    // get number of tokens in the string
    command.num_token = count_token(large, delim);
    // malloc the array of tokens by the number of tokens
    command.command_list = malloc(sizeof(char*)*(command.num_token+1));
    // iterate through array to add in the tokens
    for (int i = 0; i <= command.num_token; i++, large = NULL) {
        // find out the tokens
        small = strtok_r(large, delim, &saveptr2);
        if (small == NULL) {
            // set the last index to NULL
            command.command_list[i] = NULL;
            break;
        }
        // allocated and add the tokens to each index
        command.command_list[i] = strdup(small);
    }
    free(cp_buf);
    return command;
}


void free_command_line(command_line* command)
{
	/*
	*	#1.	free the array base num_token
	*/
    for (int i = 0; i < command->num_token; i++) {
        free(command->command_list[i]);
    }
    free(command->command_list);
}
