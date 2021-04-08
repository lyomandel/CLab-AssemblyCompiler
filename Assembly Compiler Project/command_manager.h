/*
 * manages all command related actions
 * */
#ifndef COMMAND_MANAGER_H_
#define COMMAND_MANAGER_H_
#include "assembly_types.h"
#include "symbol_manager.h"
#include "utils.h"
#include <string.h>

#define I_ENTRY 0
#define I_EXTERN 1
#define I_STRING 2
#define I_DATA 3
#define CMD_SIZE 20

/*functions dealing with each command */
int entryAction(char*);
int externAction(char*);
int stringAction(char*);
int dataAction(char*);
int movAction(char*);
int cmpAction(char*);
int addAction(char*);
int subAction(char*);
int notAction(char*);
int clrAction(char*);
int leaAction(char*);
int incAction(char*);
int decAction(char*);
int jmpAction(char*);
int bneAction(char*);
int redAction(char*);
int prnAction(char*);
int jsrAction(char*);
int rtsAction(char*);
int stopAction(char*);

/*return the command number, if not found return -1*/
int get_command(char*);
/*get a string and returns the register number, if the string is not a register return -1*/
int get_register(char *word) ;




#endif
