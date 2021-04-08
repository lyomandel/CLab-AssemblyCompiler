#include "utils.h"

/*gets a string for name, a string for type, returns a string of both of them combined
 * returned string must be freed by calling function!.*/
char* get_file_name(char *name, char *type) {
	char *nameFileInput;
	nameFileInput = malloc(strlen(name) + strlen(type) + 1);
	if (!nameFileInput) {
		printf("\n cannot allocate memory\n");
		return NULL;
	}
	strcpy(nameFileInput, name);
	strcat(nameFileInput, type);
	return nameFileInput;
}

/*gets a string and extracts a string, put it into 'word' = malloc char*
 * len is the length of word,
 * puts the word into 'word', and it's length into 'len'
 * returns true on success false otherwise
 * the word has the be freed by calling function!*/
int get_word(char** input, char** word, char endChar, int* len) {
	int i;
	i = 0;
	*word = (char*) malloc(sizeof(char));
	*len = sizeof(char);
	if (!(*word)) {
		print_error("cannot allocate memory");
		return FALSE;
	}
	while ((*input)[i] == ' ' || ((*input)[i]) == '\t')
		i++;
	while (((*input)[i] && ((*input)[i] != endChar))
			&& ((*input)[i] != '\n' && (*input)[i] != ' ' && (*input)[i] != '\t')) {
		(*word)[*len - 1] = (*input)[i];
		(*len)++;
		*word = (char*) realloc(*word, *len);
		if (!(*word)) {
			printf("cannot allocate memory");
			return FALSE;
		}
		i++;
	}
	(*word)[*len - 1] = '\0';
	(*len)--;
	*input += i;
	return TRUE;
}

/*returns number of commas, changes input to start after comma*/
int comma_check(char** input) {
	int i, comma;
	comma = 0;
	i = 0;
	while ((*input)[i] == ' ' || (*input)[i] == '\t' || (*input)[i] == ',') {
		if ((*input)[i] == ',')
			comma++;
		i++;
	}
	(*input) += i;
	return comma;
}
/*returns true if word is last in line*/
int is_last_word(char *input) {
	if ((input[0] == '\n') || (input[0] == '\0'))
		return TRUE;
	while ((*input != '\n') && (*input != '\0')) {
		if ((*input != '\n') && (*input != '\t') && (*input != ' '))
			return FALSE;
		input++;
	}
	return TRUE;
}
/*return 1 if s is number, otherwise 0*/
int is_number(char* s) {
	int i;
	if (strlen(s) == 0)
		return FALSE;
	for (i = 0; i < strlen(s); i++)
		if (i == 0) {
			if ((s[i] != '-') && (s[i] != '+'))
				if ((isdigit(s[i]) == 0))
					return FALSE;
		} else if (isdigit(s[i]) == 0)
			return FALSE;
	return TRUE;
}

/*get a string and print it in an error format according to curent line*/
void print_error(char* error) {
	printf("ERROR line %d : %s \n", currLine, error);

}
