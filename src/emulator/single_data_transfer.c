#include <inttypes.h>
#include "machine_state.h"
#include "utils.h"
#include "single_data_transfer.h"

void single_data_transfer(
    STATE* state, 
    uint32_t instr
) {
    uint32_t sf = getRangeInt(instr, 30, 30); 
    uint32_t L = getRangeInt(instr, 22, 22); // 1 -> load, 0 -> store 
    uint32_t U = getRangeInt(instr,24, 24);  // Unsigned offset flag
    uint32_t xn = getRangeInt(instr, 9, 5); // Base register
    uint32_t instr21 = getRangeInt(instr, 21, 21);
    uint32_t rt = getRangeInt(instr, 4, 0); // Target register
    uint64_t target_addr = 0;    
    uint64_t xn_val = get_register(state, xn, !sf);

    if (U) {
        // Address mode is unsigned immediate offset
        uint32_t imm12 = getRangeInt(instr, 21, 10);
        uint32_t uoffset = (sf == 1) ? imm12 * 8 : imm12 * 4;
        target_addr = xn_val + ((uint64_t) uoffset); // Zero extend offset 
    } else if (instr21) {
        // Address mode is register offset
        uint32_t xm = getRangeInt(instr, 20, 16);
        target_addr = xn_val + get_register(state, xm, 0);
    } else {
        // Address mode is pre/post index
        uint32_t I = getRangeInt(instr, 11, 11);
        uint32_t simm9_u = getRangeInt(instr, 20, 12);
        // Need to sign extend simm9 for negative values
        // 20 - 12 + 1 = 9 is the width of simm9
        int32_t simm9 = sign_extend(simm9_u, 9); 

        // Check if simm9 is in the range -256 to 255
        if (!(simm9 >= -256 && simm9 <= 255)) {
            fprintf(stderr, "simm9 must be in the range -256 to 255");
            exit(EXIT_FAILURE);
        }

        if (I == 1) {
            // Pre-indexed address mode
            target_addr = xn_val + simm9;
            // Update xn by adding the signed value simm9
            set_register(state, xn, target_addr, 0);
        } else {
            // Post-indexed address mode
            target_addr = xn_val;
            set_register(state, xn, target_addr + simm9, 0);
        }
    }

    if (L) { // Load operation: rt <- M[target_addr]
        if (sf) { // 64-bit register rt
            uint64_t value = load_doubleword(state, target_addr);
            set_register(state, rt, value, 0);
        } else { // 32-bit register rt
            uint32_t value = load_word(state, target_addr);
            set_register(state, rt, value, 1);
        }
    } else { // Store operation: M[target_addr] <- rt
        if (sf) {
            uint64_t value = get_register(state, rt, 0);
            store_doubleword(state, target_addr, value); 
        } else {
            uint32_t value = get_register(state, rt, 1);
            store_word(state, target_addr, value);
        }
    }
    
}

void load_from_literal(
    STATE *state, 
    uint32_t instr
) {
    uint32_t sf = getRangeInt(instr, 30, 30); // 1 -> 64 bits, 0 -> 32 bits
    uint32_t simm19 = getRangeInt(instr, 23, 5); // Offset
    uint32_t rt = getRangeInt(instr, 4, 0); // Target register
    // Need to sign extend simm19 for negative values
    int64_t target_addr = state->pc + (sign_extend_64(simm19 * 4, 21));
    if (sf) {
        // 64 bits
        uint64_t value = load_doubleword(state, target_addr);
        set_register(state, rt, value, 0);
    } else {
        // 32 bits
        uint32_t value = load_word(state, target_addr);
        set_register(state, rt, value, 1);
    }
    
}

void data_transfer(
    STATE *state, 
    uint32_t instr
) {
    // Check bits 25-28
    uint32_t op0 = getRangeInt(instr, 28, 25);
        if ((op0 & 5) != 4) {
            exit(EXIT_FAILURE);
        }
    uint8_t opcode = getRangeInt(instr, 31, 31);
    if (opcode == 1) {
        single_data_transfer(state, instr);
    } else {
        load_from_literal(state, instr);
    }
}
