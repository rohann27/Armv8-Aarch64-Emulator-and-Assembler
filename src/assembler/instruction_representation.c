#include "instruction_representation.h"

instruction_t* create_instruction(void) {
    instruction_t* instr = malloc(sizeof(instruction_t));

    if (!instr) {
        fprintf(stderr, "Error: Failed to allocate memory for instruction\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialize all fields to safe defaults
    instr->type = INSTR_UNKNOWN;
    memset(instr->mnemonic, 0, sizeof(instr->mnemonic));
    
    // Initialize registers to invalid state
    instr->rd = (reg_t){REG_UNDEFINED, -1};
    instr->rn = (reg_t){REG_UNDEFINED, -1};
    instr->rm = (reg_t){REG_UNDEFINED, -1};
    instr->ra = (reg_t){REG_UNDEFINED, -1};
    instr->rt = (reg_t){REG_UNDEFINED, -1};

    // Initialize operand
    instr->operand.type = OP_IMM;
    instr->operand.value.imm = 0;
    
    // Initialize addressing mode
    memset(&instr->address, 0, sizeof(addressing_mode_t));
    instr->address.type = UNSIGNED_IMM_ZERO_OFFSET;
    instr->address.value.literal.int_directive = 0;
    
    // Initialize dynamic strings
    instr->branch_label = NULL;
    instr->raw_line = NULL;
    
    // Initialize other fields
    instr->imm16 = 0;
    instr->shift_amount = 0;
    instr->directive_value = 0;
    instr->instr_address = 0x0;
    instr->encoded = 0;
    
    return instr;
}


void free_instruction(instruction_t* instr) {
    if (!instr) return;
    
    // // Free dynamic strings
    // if (instr->branch_label) {
    //     free(instr->branch_label);
    // }
    
    // if (instr->raw_line) {
    //     free(instr->raw_line);
    // }
    
    // Free addressing mode label if it exists
    // if (instr->address.type == LITERAL && instr->address.value.literal.label) {
    //     free(instr->address.value.literal.label);
    // }
    
    // Free the instruction itself
    free(instr);
}


shift_type string_to_shift_type(char* str) {
    if (!str) return LSL;
    
    if (strcmp(str, "lsl") == 0) return LSL;
    if (strcmp(str, "lsr") == 0) return LSR;
    if (strcmp(str, "asr") == 0) return ASR;
    if (strcmp(str, "ror") == 0) return ROR;
    return LSL; // Default
}

reg_t string_to_reg_t(char *regstr) {
    reg_t reg;

    if (strcmp(regstr, "sp") == 0) {
        reg = (reg_t) {.type = REG_SP, .number = 0};
    } else if (strcmp(regstr, "wzr") == 0) {
        reg = (reg_t) {.type = REG_WZR, .number = 31};
    } else if (strcmp(regstr, "xzr") == 0) {
        reg = (reg_t) {.type = REG_XZR, .number = 31};
    } else if (strcmp(regstr, "PC") == 0 || strcmp(regstr, "pc") == 0) {
        reg = (reg_t) {.type = REG_PC, .number = 0}; 
    } else if (strlen(regstr) >= 2 || strlen(regstr) <= 3) { 
        char sf = regstr[0];
        if (sf != 'x' && sf != 'w')
         { fprintf(stderr, "Invalid register format.\n"); exit(EXIT_FAILURE); }
        char no[2] = {regstr[1], regstr[2]};
        int reg_no = atoi(no);
        reg = (reg_t) {.type = (sf == 'w' ? REG_W : REG_X), .number = reg_no};
    } else {
        fprintf(stderr, "Invalid register format.\n"); exit(EXIT_FAILURE);
    }
    return reg;
}

