#ifndef UTILS_ASSEMBLE_H
#define UTILS_ASSEMBLE_H

#include <stdio.h>
#include "instruction_representation.h"

/**
 * @brief Writes encoded instructions to a binary file
 *
 * @param file_out File pointer opened in binary write mode
 * @param instructions Array of pointers to instruction_t structures
 * @param instruction_count Number of instructions to write
 */

void print_instructions_to_binary(FILE *, instruction_t **, int);

/**
 * @brief Resolves alias mnemonics to their actual instruction counterparts
 *
 * If the mnemonic is not an alias, it is returned unchanged
 *
 * @param mnemonic The mnemonic to resolve
 * @return A string representing the actual instruction mnemonic
 */

const char *resolve_alias_mnemonic(const char *);

/**
 * @brief Converts a string to an integer, detecting the numeric base
 *
 * Automatically detects base from string prefix
 *
 * @param value String containing the numeric value
 * @return The parsed integer value
 */

int string_to_immediate(char *);

/**
 * @brief Creates an empty file with the specified name
 *
 * If the file cannot be created, the function exits the program with failure
 *
 * @param filename Name of the file to create
 */

void create_empty_file(
    const char *filename
);

/**
 * @brief Opens a file with the given mode
 *
 * Exits the program with failure if the file cannot be opened
 *
 * @param filename Name of the file to open
 * @param mode Mode to open the file in
 * @return FILE pointer to the opened file
 */

FILE* load_file(
    const char* filename, 
    const char* mode
);

#endif
