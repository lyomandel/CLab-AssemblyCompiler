/*
 	 most of the types and definitions for assembly program
 */

#ifndef ASSEMBLY_TYPES_H_
#define ASSEMBLY_TYPES_H_

#include <stdio.h>
#include <stdlib.h>
/*definitions*/
#define TRUE 1
#define FALSE 0
#define LABEL_LENGTH 31
#define LINE_SIZE 100
#define MEMORY_SIZE 4096
#define IC_START 100
#define DC_START 100
#define TOKEN " \t\n"
#define ASSEMBLY_FILE_TYPE ".as"
#define EXT_FILE_TYPE ".ext"
#define ENT_FILE_TYPE ".ent"
#define OB_FILE_TYPE ".ob"
#define ADDRESS_METHOD_SIZE 4
#define METHOD_IM 1
#define METHOD_IM_SYM '#'
#define METHOD_DIR 2
#define METHOD_R_INDIR 4
#define METHOD_R_INDIR_SYM '*'
#define METHOD_R_DIR 8
#define ARE_A 4
#define ARE_R 2
#define ARE_E 1
/*types*/
typedef short memWord;
/*command data structure*/
typedef struct {
	char *name;
	memWord value;
	int (*func)(char*);
} command;


/*symbols/label data structures*/
typedef struct symbolNode * symbolPtr;
typedef enum {
	LABEL, ENTRY, EXTERNAL
} symbol_type;
typedef enum {
	CODE, DATA
} symbol_property;
typedef struct symbolNode {
	char name[LABEL_LENGTH];
	int value;
	symbol_property property;
	symbol_type type;
	symbolPtr next;
} symbolNode;

/*Globals*/
extern int IC;
extern int DC;
extern int currLine;
/*Code and Data image are allowed to be in a fixed size array, otherwise would be kept in a list*/
extern memWord memoryCode[MEMORY_SIZE];
extern memWord memoryData[MEMORY_SIZE];
extern const command cmd[];
extern symbolPtr symList;

#endif /* ASSEMBLY_TYPES_H_ */
