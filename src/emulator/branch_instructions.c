#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include "machine_state.h"
#include "utils.h"
#include "branch_instructions.h"

bool eval_cond(
    STATE *state, 
    uint32_t cond
) {
    switch (cond) {
        case EQ:
            return state->pstate.Z == 1;
        case NE:
            return state->pstate.Z == 0;
        case GE:
            return state->pstate.N == state->pstate.V;
        case LT:
            return state->pstate.N != state->pstate.V;
        case GT:
            return state->pstate.Z == 0 && state->pstate.N == state->pstate.V;
        case LE:
            return !(state->pstate.Z == 0 && state->pstate.N == state->pstate.V);
        case AL:
            return true;
        default:
        return false;
    }
}

// Branch unconditional function
// Extract bits 25-0 and calcuate the correct offset, which is added to The PC

void branch_unconditional(
    STATE *state, 
    uint64_t instr
) {
    uint32_t simm26 = getRangeInt(instr,25,0);
    int64_t offset = sign_extend_64(simm26 << 2, 28);

    state->pc += offset;
}

// Branch register function

void branch_register(
    STATE *state, 
    uint64_t instr
) {
    uint32_t xn = getRangeInt(instr,9,5);
    state->pc = get_register(state, xn, 0);
}

// Branch conditional function
// Extract 23-5, and get the condition bits that use the eval_cond function from above, as well as getting the offset to increment PC
// If it satisfies a certain condition the flags are checked accordingly and the PC is incrememented either by offset if it eval_cond is true otherwise increment PC by 4 to get next instruction

void branch_conditional(
    STATE *state, 
    uint64_t instr
) {
    uint32_t simm19 = getRangeInt(instr,23,5);
    uint32_t cond = getRangeInt(instr,3,0);
    int64_t offset = sign_extend_64(simm19 << 2, 21);

    if (eval_cond(state, cond)) {
        state->pc += offset;
    }
    else {
        state->pc +=  4;
    }
}

void branch(
    STATE *state, 
    uint32_t instr
) {
    // Check we are doing branching
    uint32_t op0 = getRangeInt(instr, 28, 25);
    if (op0 != 10 && op0 != 11) {
        fprintf(stderr, "Code failed");
        exit(EXIT_FAILURE);
    }

    uint32_t bit_31 = getRangeInt(instr, 31, 31);
    uint32_t bit_30 = getRangeInt(instr, 30, 30);
    if (bit_31) {
        branch_register(state, instr);
    }
    else if (bit_30) {
        branch_conditional(state, instr);
    }
    else {
        branch_unconditional(state, instr);
    }
}
