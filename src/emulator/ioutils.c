#include <stdio.h>
#include <inttypes.h>
#include "ioutils.h"
#include "machine_state.h"
#include "utils.h"

void create_empty_file(
    const char *filename
) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to create file");
        exit(EXIT_FAILURE);
    }
    fclose(file);  // Close immediately to leave it empty
}

void write_to_file(
    const char *filename, 
    const char *text
) {
    FILE *file = load_file(filename, "w");  // "w" for write (overwrite)
    if (!file) {
        perror("Failed to open file for writing");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s", text);  // Writes the string to the file

    fclose(file);  // Always closes the file
}

size_t load_binary_to_memory(
    const char *filename, 
    uint8_t* memory
) {
    FILE *file = load_file(filename, "rb");

    size_t bytes_read = fread(memory, sizeof(uint8_t), MEMORY_SIZE, file);
    if (ferror(file)) {
        perror("Error reading file");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);

    return bytes_read;
}

FILE* load_file(
    const char* filename, 
    const char* mode
) {
    FILE *file = fopen(filename, mode);
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    return file;
}

void print_machine_state(
    STATE *state, 
    FILE *fout
) {
    // Registers
    fprintf(fout, "Registers:\n");
    for (int i = 0; i < NUM_REGISTERS; i++) {
        fprintf(fout, "X%02d = %016" PRIx64 "\n", i, state->registers[i]);
    }

    // PC
    fprintf(fout, "PC  = %016" PRIx64 "\n", state->pc);

    // PSTATE
    fprintf(fout, "PSTATE : ");
    fprintf(fout, "%c", state->pstate.N ? 'N' : '-');
    fprintf(fout, "%c", state->pstate.Z ? 'Z' : '-');
    fprintf(fout, "%c", state->pstate.C ? 'C' : '-');
    fprintf(fout, "%c\n", state->pstate.V ? 'V' : '-');

    // Non-zero memory
    fprintf(fout, "Non-zero Memory:\n");
    for (uint64_t addr = 0; addr + 3 < MEMORY_SIZE; addr += 4) {
        // Reads 4 bytes as a little-endian 32-bit word
        uint32_t word = load_word(state, addr);

        if (word != 0) {
            fprintf(fout, "0x%08" PRIx64 ": %08x\n", addr, word);
        }
    }

}
