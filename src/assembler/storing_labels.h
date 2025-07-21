#ifndef STORING_LABELS_H
#define STORING_LABELS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "symbol_table.h"

#define MAX_LINE_LEN 256

/**
 * @brief Checks if a line contains an assembly instruction
 * 
 * Determines whether the given line represents an instruction line
 * The line must start with an alphabetic character and not contain a colon (:)
 * 
 * @param line The input null-terminated string line
 * @return int Returns 1 if the line is an instruction, otherwise 0
 */

int is_instruction(const char *line);

/**
 * @brief Checks if a line contains an integer directive
 * 
 * Determines whether the line starts with the ".int" directive
 * 
 * @param line The input null-terminated string line
 * @return int Returns 1 if the line starts with ".int", otherwise 0
 */

int is_int_directive(const char *line);

/**
 * @brief Reads an assembly file and fills the symbol table with label addresses
 * 
 * This function scans through the input file line-by-line, identifying labels
 * and their corresponding memory addresses. Labels are lines ending with a colon (:)
 * The address increments by 4 for every instruction or integer directive found
 * 
 * @param in The input FILE pointer to the assembly source file
 * @param table The symbol_table to be filled with labels and their addresses
 */

void fill_symbol_table(FILE *in, symbol_table table);

#endif
