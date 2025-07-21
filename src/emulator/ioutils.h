#ifndef IOUTILS_H
#define IOUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "emulate.h"
#include "machine_state.h"

/**
 * Loads a binary file into memory.
 *
 * Reads binary data from a file into the machine's memory array, up to
 * a maximum of `MEMORY_SIZE` bytes.
 *
 * @param filename Path to the binary file to load.
 * @param memory Pointer to the memory buffer where contents will be written.
 * @return The number of bytes successfully read.
 */
size_t load_binary_to_memory(
    const char *filename, 
    unsigned char* memory
);

/**
 * Opens a file safely with error checking.
 *
 * @param filename Path to the file.
 * @param mode File open mode (e.g., "r", "w", "rb", etc.).
 * @return A `FILE*` pointer to the opened file.
 */
FILE* load_file(
    const char* filename, 
    const char* mode
);

/**
 * Creates an empty file at the specified path.
 *
 * @param filename Path to the file to be created or cleared.
 */
void create_empty_file(
    const char *filename
);

/**
 * Writes text to a file, overwriting any existing content.
 *
 * @param filename Path to the file.
 * @param text The text to be written to the file.
 */
void write_to_file(
    const char *filename, 
    const char *text
);

/**
 * Prints the entire machine state to a given file stream.
 *
 * Outputs the following to the specified output file:
 * - Contents of all general-purpose registers (X0–X30)
 * - The PC
 * - The processor flags PSTATE
 * - All non-zero 4-byte memory values
 *
 * @param state Pointer to the machine state.
 * @param fout File stream to print to (e.g., stdout or a file).
 */
void print_machine_state(
    STATE *state, 
    FILE *fout
);

#endif
