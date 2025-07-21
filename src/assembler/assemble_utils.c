#include "assemble_utils.h"

void print_instructions_to_binary(FILE *file_out, instruction_t ** instructions, int instruction_count) {
    if (!file_out) return;
    for (int i = 0; i < instruction_count; i++) {
        uint32_t word = instructions[i]->encoded;
        fwrite(&word, sizeof(uint32_t), 1, file_out);
    }
    fclose(file_out);
}

const char *resolve_alias_mnemonic(const char *mnemonic) {
    if (strcmp(mnemonic, "cmp") == 0)     return "subs";
    if (strcmp(mnemonic, "cmn") == 0)     return "adds";
    if (strcmp(mnemonic, "neg") == 0)     return "sub";
    if (strcmp(mnemonic, "negs") == 0)    return "subs";
    if (strcmp(mnemonic, "tst") == 0)     return "ands";
    if (strcmp(mnemonic, "mvn") == 0)     return "orn";
    if (strcmp(mnemonic, "mov") == 0)     return "orr";
    if (strcmp(mnemonic, "mul") == 0)     return "madd";
    if (strcmp(mnemonic, "mneg") == 0)    return "msub";

    // Not an alias, return as-is
    return mnemonic;
}

int string_to_immediate(char *value) {
    return (uint32_t) strtol(value, NULL, 0); // base 0 auto-detects hex (0x) or decimal;
}

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