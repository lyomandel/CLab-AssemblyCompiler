#include "command_manager.h"
/*command array containing all commands*/
const command cmd[] = { { ".entry", -1, entryAction }, { ".extern", -1,
		externAction }, { ".string", -1, stringAction }, { ".data", -1,
		dataAction }, { "mov", 0, movAction }, { "cmp", 1, cmpAction }, { "add",
		2, addAction }, { "sub", 3, subAction }, { "lea", 4, leaAction }, {
		"clr", 5, clrAction }, { "not", 6, notAction }, { "inc", 7, incAction },
		{ "dec", 8, decAction }, { "jmp", 9, jmpAction },
		{ "bne", 10, bneAction }, { "red", 11, redAction }, { "prn", 12,
				prnAction }, { "jsr", 13, jsrAction }, { "rts", 14, rtsAction },
		{ "stop", 15, stopAction } };
/*register array*/
const char *registerNames[] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" };
/*return t*r6,he command number, if not found return -1*/
int get_command(char* word) {
	int i;
	for (i = 0; (i < CMD_SIZE); i++) {
		if (strcmp(word, cmd[i].name) == 0)
			return i;
	}
	return -1;
}
/*get a string and returns the register number, if the string is not a register return -1*/
int get_register(char *word) {
	int i;
	if (strlen(word) != 2)
		return -1;
	for (i = 0; (i < (sizeof(registerNames) / sizeof(registerNames[0]))); i++) {
		if (strcmp(word, registerNames[i]) == 0)
			return i;
	}
	return -1;
}
/*return operand method number, in case of error -1. assigns operand data to *data*/
int get_operand_method(char *word, memWord *data) {
	int registerNum;
	/*is immidiate*/
	if (*word == METHOD_IM_SYM) {
		if (!is_number(word + 1)) {
			print_error("immediate operand without a number");
			return -1;
		}
		*data = (memWord) (atoi(word + 1));
		return METHOD_IM;
	}
	/*is register indirect*/
	if (*word == METHOD_R_INDIR_SYM) {
		registerNum = get_register(word + 1);
		if (registerNum != -1) {
			*data = (memWord) (registerNum);
			return METHOD_R_INDIR;
		} else {
			print_error("undefined register ");
			return -1;
		}
	}
	/*is direct register*/
	registerNum = get_register(word);
	if (registerNum != -1) {
		*data = (memWord) (registerNum);
		return METHOD_R_DIR;
	}
	/*is symbol*/
	if (!can_be_symbol(word)) {
		print_error("operand syntax error");
		return -1;
	}
	*data = 0;
	return METHOD_DIR;
}
/*get an operand method and return TRUE if it is a register operand method, otherwise FALSE*/
int is_op_reg(memWord op) {
	if (op == METHOD_R_INDIR || op == METHOD_R_DIR)
		return TRUE;
	else
		return FALSE;
}
/*set the ARE field for operands in the first run*/
void set_op_are(memWord *data, memWord op1, int opNum) {
	if (op1 == METHOD_DIR) {
		*data = 0;
		return;
	} else {
		if ((op1 == METHOD_R_DIR) || (op1 == METHOD_R_INDIR)) {
			if (opNum) {
				*data = *data << 3 | ARE_A
				;
			} else
				*data = *data << 6 | ARE_A
				;
			return;
		} else
			*data = *data << 3 | ARE_A
			;
	}
}
/*get a string and command number, handle command with no operands, return TRUE on success, otherwise false*/
int no_operand_command(char *line, int cmdNum) {
	if (!is_last_word(line)) {
		print_error("invalid operands");
		return FALSE;
	}
	memoryCode[IC] = cmd[cmdNum].value << 11 | ARE_A;
	IC++;
	return TRUE;
}
/*handles a command with two operands
 * returns true on success and 0 on fail, prints error
 * gets line, command number, number of fault operands that cause an error and the amount of such operands */
int second_operand_command(char *line, int cmdNum, int *faultOp, int size) {
	memWord cell, data2, op2;
	char *word;
	int len, i;
	/*set command bits*/
	cell = cmd[cmdNum].value << 11;
	/*validate no commas*/
	if (comma_check(&line) != 0) {
		print_error("invalid comma");
		return FALSE;
	}
	/*get operand*/
	if (!get_word(&line, &word, ',', &len))
		return FALSE;
	op2 = get_operand_method(word, &data2);
	free(word);
	if (op2 == -1)
		return FALSE;
	for (i = 0; i < size; i++) {
		if (op2 == faultOp[i]) {
			print_error("invalid operand method");
			return FALSE;
		}
	}
	/*validates last word in line*/
	if (!is_last_word(line)) {
		print_error("invalid paramters");
		return FALSE;
	}
	cell = cell | (op2 << 3) | ARE_A
	;
	memoryCode[IC] = cell;
	IC++;
	set_op_are(&data2, op2, 1);
	memoryCode[IC++] = data2;
	return TRUE;
}
/*handles a command with two operands
 * returns true on success and 0 on fail, prints error
 * gets line, command number, number of fault operands that cause an error and the amount of such operands */
int two_operand_command(char* line, int cmdNum, int *faultOp1, int size1,
		int *faultOp2, int size2) {
	memWord cell, data1, data2, op1, op2;
	char *word;
	int len, i;
	/*set command bits*/
	cell = cmd[cmdNum].value << 11;
	/*validate no commas*/
	if (comma_check(&line) != 0) {
		print_error("invalid comma");
		return FALSE;
	}
	/*get 1st operand*/
	if (!get_word(&line, &word, ',', &len))
		return FALSE;
	op1 = get_operand_method(word, &data1);
	free(word);
	if (op1 == -1)
		return FALSE;
	for (i = 0; i < size1; i++) {
		if (op1 == faultOp1[i]) {
			print_error("invalid first operand method");
			return FALSE;
		}
	}
	/*validate only one comma*/
	if (comma_check(&line) != 1) {
		print_error("invalid comma");
		return FALSE;
	}
	/*get 2nd operand*/
	if (!get_word(&line, &word, ',', &len))
		return FALSE;
	op2 = get_operand_method(word, &data2);
	free(word);
	/*validates last word in line*/
	if (!is_last_word(line)) {
		print_error("invalid paramters");
		return FALSE;
	}
	if (op2 == -1)
		return FALSE;
	for (i = 0; i < size2; i++) {
		if (op2 == faultOp2[i]) {
			print_error("invalid second operand method");
			return FALSE;
		}
	}
	/*set command word*/
	cell = cell | (op1 << 7) | (op2 << 3) | ARE_A
	;
	memoryCode[IC] = cell;
	IC++;
	/*if two operands are register operand, write operand values in the same word, otherwise 2 words*/
	if (is_op_reg(op1) && is_op_reg(op2)) {
		data1 = (data1 << 6) | (data2 << 3) | ARE_A
		;
		memoryCode[IC++] = data1;
		return TRUE;
	} else {
		set_op_are(&data1, op1, 0);
		memoryCode[IC++] = data1;
		set_op_are(&data2, op2, 1);
		memoryCode[IC++] = data2;
		return TRUE;
	}
}

int entryAction(char* line) {
	char *word;
	int len;
	if (comma_check(&line) != 0) {
		print_error("invalid comma");
		return FALSE;
	}
	while (line && (line[0] != '\n') && (line[0] != '\0')) {
		if (!get_word(&line, &word, ',', &len))
			return FALSE;
		if (set_entry_symbol(&symList, word) == FALSE) {
			free(word);
			return FALSE;
		}
		free(word);
	}
	return TRUE;
}
int externAction(char* line) {
	char *word;
	symbol_type t;
	int len;
	while (line && (line[0] != '\n') && (line[0] != '\0')) {
		if (!get_word(&line, &word, ',', &len))
			return FALSE;
		if (get_symbol_type_by_name(&symList, word, &t)) {
			if (t != EXTERNAL) {
				print_error("external symbol already defined as another type");
				return FALSE;
			}
		}
		add_symbol(&symList, word, DATA, EXTERNAL, 0);
		free(word);
	}
	return TRUE;
}
int stringAction(char* line) {
	char* word;
	int i;
	if (comma_check(&line) != 0) {
		print_error("invalid comma");
		return FALSE;
	}
	/*search for start of string*/
	while (*line != '\0' && *line != '\n' && (*line == ' ' || *line == '\t'))
		line++;
	if (*line != '"') {
		print_error("invalid string");
		return FALSE;
	}
	word = line++;
	/*search for end of string*/
	while (*line != '\n' && *line != '\0' && *line != '"')
		line++;
	if (*line != '"') {
		print_error("invalid string");
		return FALSE;
	}
	/*check if the string is the only word*/
	if (!is_last_word(line + 1)) {
		print_error("invalid paramters");
		return FALSE;
	}
	/*copy string to memory*/
	for (i = 1; i < (line - word); i++) {
		memoryData[DC++] = word[i];
	}
	memoryData[DC++] = 0;
	return TRUE;
}
int dataAction(char* line) {
	char *word;
	int len, comma;
	/*validate no comma in the beginning*/
	if (comma_check(&line) != 0) {
		print_error("invalid comma");
		return FALSE;
	}
	/*get first word*/
	while (line && (*line != '\n') && (*line != '\0')) {
		if (!get_word(&line, &word, ',', &len))
			return FALSE;
		if (len == 0) {
			free(word);
			continue;
		}
		/*check if the data member is a number*/
		if (is_number(word)) {
			memoryData[DC] = atoi(word);
			DC++;
			free(word);
			comma = 0;
		} else {
			print_error("not a number");
			free(word);
			return FALSE;
		}
		/*check if it is the last word, if not search for comma*/
		if (!is_last_word(line)) {
			comma = comma_check(&line);
			if (comma != 1) {
				print_error("invalid comma");
				return FALSE;
			}
		}
	}
	/*if last word ends with a comma, return false*/
	if (comma) {
		print_error("invalid comma");
		return FALSE;
	}
	return TRUE;
}
/*command functions - each function sesub # 5, *r6ts the invalid operand methods, and the command number and uses the appropriate command fucntion*/
int movAction(char* line) {
	int size1, size2;
	int faultOp2[] = { METHOD_IM };
	size1 = 0;
	size2 = sizeof(faultOp2) / sizeof(faultOp2[0]);
	return two_operand_command(line, 4, 0, size1, faultOp2, size2); /*faultOp1 = 0, no faults, command number is 4*/
}

int cmpAction(char* line) {

	return two_operand_command(line, 5, 0, 0, 0, 0); /*faultOp1 = 0,faultOp2 = 0, no faults, command number is 5*/
}
int addAction(char* line) {
	int size1, size2;
	int faultOp2[] = { METHOD_IM };
	size1 = 0;
	size2 = sizeof(faultOp2) / sizeof(faultOp2[0]);
	return two_operand_command(line, 6, 0, size1, faultOp2, size2); /*faultOp1 = 0, no faults, command number is 6*/
}
int subAction(char* line) {
	int size1, size2;
	int faultOp2[] = { METHOD_IM };
	size1 = 0;
	size2 = sizeof(faultOp2) / sizeof(faultOp2[0]);
	return two_operand_command(line, 7, 0, size1, faultOp2, size2); /*faultOp1 = 0, no faults, command number is 7*/;
}

int leaAction(char* line) {
	int size1, size2;
	int faultOp1[] = { METHOD_IM, METHOD_R_DIR, METHOD_R_INDIR };
	int faultOp2[] = { METHOD_IM };
	size1 = sizeof(faultOp1) / sizeof(faultOp1[0]);
	size2 = sizeof(faultOp2) / sizeof(faultOp2[0]);
	return two_operand_command(line, 8, faultOp1, size1, faultOp2, size2); /* faultOp1 = 0,2,3 , faultOp2 = 1 , command number is 10*/
}

int clrAction(char* line) {
	int size, faultOp[] = { METHOD_IM };
	size = sizeof(faultOp) / sizeof(faultOp[0]);
	return second_operand_command(line, 9, faultOp, size); /* faultOp = 0 , command number is 9*/
}

int notAction(char* line) {
	int size, faultOp[] = { METHOD_IM };
	size = sizeof(faultOp) / sizeof(faultOp[0]);
	return second_operand_command(line, 10, faultOp, size); /* faultOp = 0 , command number is 8*/
}

int incAction(char* line) {
	int size, faultOp[] = { METHOD_IM };
	size = sizeof(faultOp) / sizeof(faultOp[0]);
	return second_operand_command(line, 11, faultOp, size); /* faultOp = 0 , command number is 11*/
}
int decAction(char* line) {
	int size, faultOp[] = { METHOD_IM };
	size = sizeof(faultOp) / sizeof(faultOp[0]);
	return second_operand_command(line, 12, faultOp, size); /* faultOp = 0 , command number is 12*/
}
int jmpAction(char* line) {
	int size, faultOp[] = { METHOD_IM, METHOD_R_DIR };
	size = sizeof(faultOp) / sizeof(faultOp[0]);
	return second_operand_command(line, 13, faultOp, size); /* faultOp = 0,3 , command number is 13*/
}
int bneAction(char* line) {
	int size, faultOp[] = { METHOD_IM, METHOD_R_DIR };
	size = sizeof(faultOp) / sizeof(faultOp[0]);
	return second_operand_command(line, 14, faultOp, size); /* faultOp = 0,3 , command number is 14*/
}
int redAction(char* line) {
	int size, faultOp[] = { METHOD_IM };
	size = sizeof(faultOp) / sizeof(faultOp[0]);
	return second_operand_command(line, 15, faultOp, size); /* faultOp = 0 , command number is 15*/
}
int prnAction(char* line) {
	return second_operand_command(line, 16, 0, 0); /* no fault operands,  command number is 16*/
}
int jsrAction(char* line) {
	int size, faultOp[] = { METHOD_IM, METHOD_R_DIR };
	size = sizeof(faultOp) / sizeof(faultOp[0]);
	return second_operand_command(line, 17, faultOp, size); /* faultOp = 0 , command number is 17*/
}
int rtsAction(char* line) {
	return no_operand_command(line, 18); /*command number is 18*/
}
int stopAction(char* line) {
	return no_operand_command(line, 19);/*command number is 19*/
}

