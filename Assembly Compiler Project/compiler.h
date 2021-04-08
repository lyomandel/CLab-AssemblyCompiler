/*
 * handles file opening, creating and first and second file iterations
 * */

#ifndef COMPILER_H_
#define COMPILER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "assembly_types.h"
#include "symbol_manager.h"
#include "command_manager.h"
/*compiles a given file, on success creates files accordingly, otherwise prints error messages in console */
void compile(char *fileName);

#endif
