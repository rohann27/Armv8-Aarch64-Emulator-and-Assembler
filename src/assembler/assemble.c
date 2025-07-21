#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "symbol_table.h"
#include "tokens.h"
#include "parser.h"
#include "encoding_functions.h"
#include "assemble_utils.h"
#include "storing_labels.h"

int main(int argc, char **argv) {

   if (argc > 3 || argc < 2) {
        printf("Usage: ./assemble <file_in> [<file_out>]\n");
        return EXIT_FAILURE;
    }

    char* in_file_name = argv[1];
    FILE *file_in = load_file(in_file_name, "r");
    FILE* file_out; // Stdout or output file pointer

    if (argc == 3) {
        create_empty_file(argv[2]);
        file_out = load_file(argv[2], "wb");
    } else {
        file_out = stdout;
    }

    parser_state_t *parser_state = create_parser_state();

    // FIRST PASS:
    //     Populate the symbol table with labels and addresses
    symbol_table s_table = symbol_table_create();
    fill_symbol_table(file_in, s_table);

    // SECOND PASS:
    //     Tokenize, and parse instructions and fill in
    //     `parser_state->instructions`
    parse(parser_state, file_in);

    // After we've parsed the instructions (located inside
    // `parser_state->instructions` array), handle the instruction
    // to binary encoding logic
    encode_instructions(parser_state->instructions, parser_state->instruction_count, s_table);

    // Print the instructions to output file
    print_instructions_to_binary(file_out, parser_state->instructions, parser_state->instruction_count);

    free_parser_state(parser_state);
    symbol_table_free(s_table);
}
