#include "machine_state.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

STATE *new_machine_state(void) {
    STATE *state = malloc(sizeof(STATE));
    if (!state) {
        perror("Failed to allocate STATE");
        exit(EXIT_FAILURE);
    }
    // Initialize every member to 0
    memset(state, 0, sizeof(STATE));
    // With exception of PSTATE: N=0, Z=1, C=0, V=0
    state->pstate.Z = 1;
    return state;
}

void free_machine_state(
    STATE *state
) {
    if (state != NULL) 
        free(state);
}

uint64_t get_register(
    STATE *state, 
    int index, 
    int is_32bit
) {
    if (index < 0 || index > 31) {
        fprintf(stderr, "Register index out of bounds: %d\n", index);
        exit(1);
    }

    if (index == 31) return 0;  // xzr or wzr
    uint64_t value = state->registers[index];
    return is_32bit ? (uint32_t)value : value;
}

void set_register(
    STATE *state, 
    int index, 
    uint64_t value, 
    int is_32bit
) {
    if (index < 0 || index > 31) {
        fprintf(stderr, "Register index out of bounds: %d\n", index);
        exit(1);
    }

    if (index == 31) return;  // Writing to xzr/wzr is ignored
    if (is_32bit)
        state->registers[index] = (uint32_t)value;
    else
        state->registers[index] = value;
}

void print_registers(
    STATE *state
) {
    for (int i = 0; i < NUM_REGISTERS; i++) {
        printf("X%02d = %" PRId64 "\n", i, state->registers[i]);
    }
    printf("PC  = %" PRId64 "\n", state->pc);
}

void print_pstate(
    STATE *state
) {
    printf("PSTATE : %c%c%c%c\n",
        state->pstate.N ? 'N' : '-',
        state->pstate.Z ? 'Z' : '-',
        state->pstate.C ? 'C' : '-',
        state->pstate.V ? 'V' : '-'
    );
}

void print_memory(
    STATE *state
) {
    for (uint64_t i = 0; i < MEMORY_SIZE; i += 4) {
        uint32_t word = state->memory[i] |
                       (state->memory[i + 1] << 8) |
                       (state->memory[i + 2] << 16) |
                       (state->memory[i + 3] << 24);
        if (word != 0) {
            printf("0x%08lx: 0x%08x\n", i, word);
        }
    }
}

