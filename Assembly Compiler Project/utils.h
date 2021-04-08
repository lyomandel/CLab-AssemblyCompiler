#ifndef UTILS_H_
#define UTILS_H_
#include "assembly_types.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
/*gets a string for name, a string for type, returns a string of both of them combined
 * returned string must be freed by calling function!.*/
char* get_file_name (char * name, char* nameType);
/*returns true if word is last in line*/
int is_last_word(char *input);
/*return 1 if s is number, otherwise 0*/
int is_number(char* s);
/*gets a string and extracts a string, put it into 'word' = malloc char*
 * len is the length of word,
 * puts the word into 'word', and it's length into 'len'
 * returns true on success false otherwise
 * the word has the be freed by calling function!*/
int get_word(char** input, char** word, char endChar, int* len);
/*returns number of commas, changes input to start after comma*/
int comma_check(char** input);
/*get a string and print it in an error format according to curent line*/
void print_error(char* error);

#endif
