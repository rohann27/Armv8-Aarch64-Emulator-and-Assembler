#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "emulate.h"
#include "ioutils.h"
#include "machine_state.h"
#include "single_data_transfer.h"
#include "data_proc.h"
#include "utils.h"
#include "branch_instructions.h"
#include "dp_register.h"

int main(int argc, char **argv) {
    
    if (argc > 3 || argc < 2) {
        printf("Usage: ./emulate <file_in> [<file_out>]\n");
        return EXIT_FAILURE;
    }

    char* in_file_name = argv[1];
    FILE* file_out; // Stdout or output file pointer

    if (argc == 3) {
        create_empty_file(argv[2]);
        file_out = load_file(argv[2], "w");
    } else {
        file_out = stdout;
    }

    // Loads a new state
    STATE *machine_state = new_machine_state();

    load_binary_to_memory(in_file_name, machine_state->memory);

    while (!machine_state->is_halted) {
        // Fetch
        uint32_t instr_curr = fetch_next_instruction(machine_state);
        // After decoding the instruction, if we see that it's a branch instruction do not change pc
        // If it's not, add 4 

        // Decode logic
        uint32_t op0 = getRangeInt(instr_curr, 28, 25);

        if (instr_curr == HALT_INSTR) {
            machine_state->is_halted = 1;
        } else if (IS_BRANCH(op0)) {
            branch(machine_state, instr_curr);
        } else if (IS_DP_IMM(op0)) {
            data_proc_imm(machine_state, instr_curr);
            machine_state->pc += PC_INCREMENT;
        } else if (IS_DP_REG(op0)) {
            data_proc_reg(machine_state, instr_curr);
            machine_state->pc += PC_INCREMENT;
        } else if (IS_LOAD_STORE(op0)) {
            data_transfer(machine_state, instr_curr);
            machine_state->pc += PC_INCREMENT;
        } else {
            fprintf(stderr, "The instruction does not exist");
            machine_state->is_halted = 1;
        }
    }

    print_machine_state(machine_state, file_out);

    return EXIT_SUCCESS;
}
