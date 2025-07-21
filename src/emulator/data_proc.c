#include <limits.h>
#include "machine_state.h"
#include "utils.h"
#include "data_proc.h"

void data_proc_imm(
    STATE* state, 
    uint32_t instr
) {
    uint32_t sf = getRangeInt(instr, 31, 31);
    uint32_t opc = getRangeInt(instr, 30, 29);
    uint32_t opi = getRangeInt(instr, 25, 23);
    uint32_t rd = getRangeInt(instr, 4, 0);

    int is_32bit = (sf == 0);

    if (opi == ARITHMETIC) { // Arithmetic operation
        uint32_t sh = getRangeInt(instr, 22, 22);
        uint64_t imm12 = getRangeInt(instr, 21, 10);
        uint32_t rn = getRangeInt(instr, 9, 5);
        uint64_t rn_value = get_register(state, rn, is_32bit);

        // Shift imm12 to the left by 12 bits if sh == 1
        if (sh == SHIFT) imm12 = imm12 << 12;

        uint64_t op2 = imm12;

        perform_arithmetic_op(state, rd, rn_value, op2, opc, is_32bit);

    }
    else if (opi == WIDE_MOVE) { 
        long long imm16 = getRangeInt(instr, 20, 5);
        int hw = getRangeInt(instr, 22, 21);

        imm16 = imm16 << (hw * 16);

        if (opc == MOVN) { 
            set_register(state, rd, ~imm16, is_32bit); 
        }
        else if (opc == MOVZ) {
            set_register(state, rd, imm16, is_32bit);
        }
        else if (opc == MOVK) { 
            uint64_t val = get_register(state, rd, is_32bit);
            uint64_t res = ( val & ~(((uint64_t)UINT16_MAX) << (hw * 16))); 
            res = res | imm16; 
            set_register(state, rd, res, is_32bit);
        }
    }
    else {
        fprintf(stderr, "Unsupported opi field in data_proc_imm: %u\n", opi);
        exit(EXIT_FAILURE);
    }
}
