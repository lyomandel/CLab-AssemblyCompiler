#include "compiler.h"
#include "command_manager.h"
#include "utils.h"

FILE *finput, *fentry, *fextern, *fob;
int IC, DC, currLine, entryFlag, externFlag;
memWord memoryCode[MEMORY_SIZE];
memWord memoryData[MEMORY_SIZE];

symbolPtr symList;
/*open the input file a FILE *finput*/
int open_file(char *fileName) {
	char *nameFileInput;
	nameFileInput = get_file_name(fileName, ASSEMBLY_FILE_TYPE);
	finput = fopen(nameFileInput, "r");
	if (!(finput)) {
		printf("file %s does not exists! \n", nameFileInput);
		free(nameFileInput);
		return FALSE;
	}
	printf("\nCompiling to file :\n\n%s\n", nameFileInput);
	free(nameFileInput);
	return TRUE;
}

/*opens files related to filename, returns true if succeed false otherwise*/
int set_files(char *fileName) {
	char *nameFileEnt, *nameFileExt, *nameFileOb;
	char negOctal[LINE_SIZE];
	int i;
	symbolPtr itr;
	/* Setting name file entry */
	if (entryFlag) {
		nameFileEnt = get_file_name(fileName, ENT_FILE_TYPE);
		fentry = fopen(nameFileEnt, "w+");
		printf("%s\n", nameFileEnt);
	}
	/* Setting name file extern */
	if (externFlag) {
		nameFileExt = get_file_name(fileName, EXT_FILE_TYPE);
		fextern = fopen(nameFileExt, "w+");
		printf("%s\n", nameFileExt);
	}

	itr = symList;
	while (itr != NULL) {
		if (entryFlag)
			if (itr->type == ENTRY)
				fprintf(fentry, "%s\t%d\n", itr->name, itr->value);
		if (externFlag)
			if ((itr->type == EXTERNAL) && (itr->value != 0))
				fprintf(fextern, "%s\t%d\n", itr->name, itr->value);
		itr = itr->next;
	}
	/* Setting name file object */

	nameFileOb = get_file_name(fileName, OB_FILE_TYPE);
	/*write ob*/
	fob = fopen(nameFileOb, "w+");
	fprintf(fob, "%d\t%d\n", IC - 1, DC);
	for (i = 0; i < IC - 1; i++) {
		sprintf(negOctal, "%05o", memoryCode[i]);
		fprintf(fob, "%d\t%s\n", i + IC_START, negOctal + strlen(negOctal) - 5);
	}
	for (i = 0; i < DC; i++) {
		sprintf(negOctal, "%05o", memoryData[i]);
		fprintf(fob, "%d	%s \n", i + IC + IC_START - 1,
				negOctal + strlen(negOctal) - 5);
	}
	free(nameFileEnt);
	free(nameFileExt);
	free(nameFileOb);
	return TRUE;
}
/*first iteration over the input file, the file is set and opened before calling this function,
 * returns FALSE if printed any errors, otherwise returns TRUE*/
int first_iteration() {
	int error, isLabel, cmdIndex, wordLen;
	char line[LINE_SIZE], *split, sym[LABEL_LENGTH], *linePtr;
	symList = NULL;
	error = FALSE;
	currLine = 0;
	IC = 0;
	DC = 0;
	entryFlag = FALSE;
	externFlag = FALSE;
	isLabel = FALSE;
	/*read all lines until the end of file*/
	while (fgets(line, LINE_SIZE, finput) != NULL) {
		linePtr = &line[0];
		isLabel = FALSE;
		currLine++;
		/*is empty or comment line*/
		if (line[0] == ';' || line[0] == '\0' || line[0] == '\n')
			continue;
		if (!get_word(&linePtr, &split, ' ', &wordLen))
			return FALSE;
		/*is the first word a label*/
		if (split[strlen(split) - 1] == ':') {
			if (is_last_word(linePtr))
				continue;
			strcpy(sym, split);
			/*can the first word be a symbol*/
			sym[strlen(sym) - 1] = '\0';
			if (is_symbol(symList, sym)) {
				isLabel = TRUE;
				/*move to next word on string*/
				free(split);
				if (!get_word(&linePtr, &split, ',', &wordLen))
					return FALSE;
			} else {
				free(split);
				continue;
			}
		}
		/*get command index, if no symbol - first word, otherwise it was moved to the second*/
		cmdIndex = get_command(split);
		free(split);
		if (cmdIndex == -1) {
			print_error("invalid command name");
			error = TRUE;
			continue;
		}
		/*check if command is string or data*/
		if ((cmdIndex == I_STRING) || (cmdIndex == I_DATA)) {
			/*if there is a label, add it to the symbol list*/
			if (isLabel) {
				if (is_symbol_exist(symList, sym) == -1) {
					if (!(add_symbol(&symList, sym, DATA, LABEL, DC)))
						error = 1;
				} else {
					print_error("label already defined");
				}
			}
			/*handle the command*/
			if (!(cmd[cmdIndex].func(linePtr))) {
				error = 1;
			}
			continue;
		}
		/*check if command is entry, if true - continue - entry is handled in the second iteration*/
		if (cmdIndex == I_ENTRY) {
			entryFlag = TRUE;
			continue;
		}
		/*check if command is extern*/
		if (cmdIndex == I_EXTERN) {
			externFlag = TRUE;
			if (!(cmd[cmdIndex].func(linePtr))) {
				error = TRUE;
			}
			continue;
		}
		/*command is operation*/
		/*if there is a label, add it to the symbol list*/
		if (isLabel) {
			if (is_symbol_exist(symList, sym) == -1) {
				if (!(add_symbol(&symList, sym, CODE, LABEL, IC + IC_START)))
					error = TRUE;
			} else {
				print_error("label already defined");
			}
		}
		/*handle command accordingly*/
		if (!(cmd[cmdIndex].func(linePtr))) {
			error = TRUE;
		}
	}
	if (error)
		return FALSE;
	/*update all data symbols according to the memory code index*/
	update_data_symbols(&symList);
	return TRUE;
}
/*handles the second iteration, on success returns TRUE, otherwise returns FALSE*/
int second_iteration() {
	char line[LINE_SIZE], *split, *linePtr;
	int len, cmdIndex, error;
	len = 0;
	error = 0;
	IC = 0;
	/*set file to the beginning*/
	fseek(finput, 0, SEEK_SET);
	/*read all lines until the end of file*/
	while (fgets(line, LINE_SIZE, finput) != NULL) {
		linePtr = &line[0];
		currLine++;
		/*is empty or comment line*/
		if (line[0] == ';' || line[0] == '\0' || line[0] == '\n')
			continue;
		if (!get_word(&linePtr, &split, ' ', &len))
			return FALSE;
		/*check if the first word is a symbol*/
		if (split[strlen(split) - 1] == ':') {
			/*move to next word on string*/
			free(split);
			if (!get_word(&linePtr, &split, ' ', &len))
				return FALSE;
		}
		/*get command name*/
		cmdIndex = get_command(split);
		free(split);
		if (cmdIndex == -1) {
			print_error("invalid command name \n");
			error = TRUE;
			continue;
		}
		/*is entry command*/
		if (cmdIndex == I_ENTRY) {
			cmd[cmdIndex].func(linePtr);
			continue;
		}
		/*is data command*/
		if ((cmdIndex == I_DATA) || (cmdIndex == I_STRING)
				|| (cmdIndex == I_EXTERN))
			continue;
		/*update symbols accordingly*/
		else if (!(search_symbol_update_memory(symList, linePtr)))
			error = TRUE;
	}
	if (error)
		return FALSE;
	return TRUE;
}
/*set all memory code and memory data to 0*/
void init_memory() {
	int i;
	for (i = 0; i < MEMORY_SIZE; i++) {
		memoryCode[i] = 0;
		memoryData[i] = 0;
	}
}
/*compiles a given file (by file name without type), on success creates files accordingly, otherwise prints error messages in console */
void compile(char *fileName) {
	init_memory();
	if (!open_file(fileName)) {
		return;
	}
	if (first_iteration()) {
		if (second_iteration()) {
			if (!(set_files(fileName)))
				printf("\ncan't create output files \n");
			else
				printf("\nDone \n");
		}
		else {
			printf("\nCompilation Failed! \n");
		}
	} else {
		printf("\nCompilation Failed! \n");
	}
	free_symbol_list(&symList);
}
