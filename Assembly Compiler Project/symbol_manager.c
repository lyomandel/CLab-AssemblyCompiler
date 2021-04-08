#include "symbol_manager.h"
#define SAVED_WORDS 4
char *savedWords[SAVED_WORDS] = { "string", "data", "entry", "extern" };
/*returns symbol value, otherwise return -1*/
int is_symbol_exist(symbolPtr symList, char *label) {
	while (symList != NULL) {
		if (strcmp(symList->name, label) == 0) {
			return symList->value;
		}
		symList = symList->next;
	}
	return -1;
}

/*validates all conditions for a symbol except if it is on the list*/
int can_be_symbol(char *label) {
	int i;
	/*is label in the right length*/
	if (strlen(label) > LABEL_LENGTH) {
		print_error("symbol too long");
		return FALSE;
	}
	/*is label starts with an alphabetic letter*/
	if (!(isalpha(*label))) {
		print_error("a label must begin with an alphabetic letter");
		return FALSE;
	}
	/*does label contain only letters and numbers*/
	for (i = 1; i < strlen(label); i++) {
		if (!(isdigit(label[i])) && !(isalpha(label[i]))) {
			print_error("a label can contain only letters and numbers");
			return FALSE;
		}
	}
	if (get_command(label) != -1) {
		print_error("invalid symbol");
		return FALSE;
	}
	for (i = 0; i < SAVED_WORDS; i++) {
		if (strcmp(label, savedWords[i]) == 0) {
			print_error("invalid symbol");
			return FALSE;
		}

	}
	return TRUE;
}

/*validates a new symbol to add to the list*/
int is_symbol(symbolPtr symList, char *label) {
	if (!can_be_symbol(label))
		return FALSE;
	if (is_symbol_exist(symList, label) == -1)
		return TRUE;
	else {
		print_error("symbol already defined");
		return FALSE;
	}
}

/*get symbol list, name, and reference to type, assigns to reference the type of name, otherwise returns FALSE*/
int get_symbol_type_by_name(symbolPtr *h, char* word, symbol_type *t) {
	symbolPtr itr;
	if ((*h) == NULL) {
		return FALSE;
	} else {
		itr = *h;
		while (itr != NULL) {
			if (strcmp(itr->name,word)==0) {
				*t = itr->type;
				return TRUE;
			}
			itr = itr->next;
		}
		return FALSE;
	}
}
/*get symbol list, symbol name, reference to value and to type, update reference and returns true if updated succesfully, otherwise false*/
int get_symbol_value_and_type(symbolPtr *h, char* sym, memWord *val, memWord *t) {
	symbolPtr itr;
	if ((*h) == NULL) {
		return FALSE;
	} else {
		itr = *h;
		while (itr != NULL) {
			if (strcmp(sym, itr->name) == 0) {
				*val = itr->value;
				if (itr->type == EXTERNAL) {
					*t = ARE_E;
				} else
					*t = ARE_R;
				return TRUE;
			}
			itr = itr->next;
		}
		return FALSE;
	}
}
/*adds new symbol to symbol list, returns TRUE if on success, otherwise FALSE*/
int add_symbol(symbolPtr *h, char *sym, symbol_property sProperty,
		symbol_type sType, int coutVal) {
	symbolPtr tmp, itr;
	tmp = (symbolPtr) malloc(sizeof(symbolNode));
	if (!tmp) {
		print_error("cannot allocate memory");
		exit(0);
	}
	tmp->next = NULL;
	strcpy(tmp->name, sym);
	tmp->value = coutVal;
	tmp->property = sProperty;
	tmp->type = sType;
	if ((*h) == NULL) {
		*h = tmp;
	} else {
		itr = *h;
		while (itr->next != NULL)
			itr = itr->next;
		itr->next = tmp;
	}
	return TRUE;
}

/*gets a line and searches for a symbol, updates memory Code according to the symbol's value*/
int search_symbol_update_memory(symbolPtr symList, char* line) {
	char* word;
	int regCount;
	memWord symType;
	memWord symVal;
	int len;
	regCount = 0;
	if (!get_word(&line, &word, ',', &len))
		return FALSE;
	if (get_symbol_value_and_type(&symList, word, &symVal, &symType)) {
		IC++;
		memoryCode[IC] = (symVal << 3) | symType;
		/*add an external to symbol list for it to be written in a file*/
		if (symType == ARE_E)
			add_symbol(&symList, word, CODE, EXTERNAL, IC + IC_START);
	} else {
		IC++;
		if (*word == '*') {
			if (get_register(word + 1) != -1)
				regCount++;
		} else if (get_register(word) != -1)
			regCount++;
	}
	free(word);
	if (is_last_word(line)) {
		IC++;
		return TRUE;
	}
	if (comma_check(&line) != 1) {
		print_error("invalid comma");
		return FALSE;
	}
	if (!get_word(&line, &word, ',', &len))
		return FALSE;
	if (get_symbol_value_and_type(&symList, word, &symVal, &symType)) {
		IC++;
		memoryCode[IC] = (((memWord) symVal) << 3) | ((memWord) symType);
		/*add an external to symbol list for it to be written in a file*/
		if (symType == ARE_E)
			add_symbol(&symList, word, CODE, EXTERNAL, IC + IC_START);
	} else {
		if (*word == '*') {
			if (get_register(word + 1) != -1)
				regCount++;
		} else if (get_register(word) != -1)
			regCount++;
		if (regCount < 2)
			IC++;

	}
	IC++;
	free(word);
	return TRUE;
}

/*get a smymbolPtr pointer and update all data symbols according to the memory code index*/
void update_data_symbols(symbolPtr *h) {
	symbolPtr itr;
	if (!(*h))
		return;
	itr = *h;
	while (itr) {
		if ((itr->property == DATA) && (itr->type != EXTERNAL))
			itr->value += IC + IC_START;
		itr = (itr->next);
	}
}
/*get a symbolPtr pointer and a symbol name, find the symbol in the list and set it to ENTRY type*/
int set_entry_symbol(symbolPtr *h, char* sym) {
	symbolPtr itr;
	if ((*h) == NULL) {
		return FALSE;
	} else {
		itr = *h;
		while (itr != NULL) {
			if (strcmp(sym, itr->name) == 0) {
				if (itr->type == EXTERNAL){
					print_error("entry symbol already defined as external");
					return FALSE;
				}
				itr->type = ENTRY;
				return TRUE;
			}
			itr = itr->next;
		}
		print_error("no symbol to set as entry");
		return FALSE;
	}
}
/*free all symbols in the list*/
void free_symbol_list(symbolPtr *h) {
	symbolPtr tmp;
	if (!(*h))
		return;
	while (*h) {
		tmp = *h;
		*h = tmp->next;
		free(tmp);
	}
}
