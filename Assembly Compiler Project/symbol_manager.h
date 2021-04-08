/*
 * manages all symbol related actions
 * */
#ifndef SYMBOL_MANAGER_H_
#define SYMBOL_MANAGER_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "assembly_types.h"
#include "utils.h"
#include "command_manager.h"

/*get symbol list, name, and reference to type, assigns to reference the type of name, otherwise returns FALSE*/
int get_symbol_type_by_name(symbolPtr *h, char* word, symbol_type *t);
/*returns symbol value, otherwise return -1*/
int is_symbol_exist(symbolPtr symList, char *label);
/*validates all conditions for a symbol except if it is on the list*/
int can_be_symbol(char *label);
/*validates a new symbol to add to the list*/
int is_symbol(symbolPtr symList, char *label);
/*adds new symbol to symbol list, returns TRUE if on success, otherwise FALSE*/
int add_symbol(symbolPtr *h, char *sym, symbol_property sProperty,
		symbol_type sType, int coutVal);
/*get a symbolPtr pointer and a symbol name, find the symbol in the list and set it to ENTRY type*/
int set_entry_symbol(symbolPtr *h, char* sym);
/*get symbol list, symbol name, reference to value and to type, update reference and returns true if updated succesfully, otherwise false*/
int get_symbol_value_and_type(symbolPtr *h, char* sym, memWord *val, memWord *t);
/*gets a line and searches for a symbol, updates memory Code according to the symbol's value*/
int search_symbol_update_memory(symbolPtr symList, char* line);
/*get a smymbolPtr pointer and update all data symbols according to the memory code index*/
void update_data_symbols(symbolPtr *h);
/*free all symbols in the list*/
void free_symbol_list(symbolPtr *h);

#endif
