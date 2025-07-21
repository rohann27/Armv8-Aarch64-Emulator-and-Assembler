#include "encoding_functions.h"


static general_entry arithmetic_table[] = {
    {"add", 0},
    {"adds", 1},
    {"sub", 2},
    {"subs", 3},
    {"cmp", 3},    // alias for subs with rd = xzr
    {"cmn", 2},    // alias for adds with rd = xzr
    {"neg", 2},    // alias for sub with rn = xzr
    {"negs", 3},   // alias for subs with rn = xzr
    END_ENTRY
};

static dp_logical_entry_t logical_table[] = {
    {"and",  0, 0},
    {"bic",  0, 1}, // alias for and with N = 1
    {"orr",  1, 0},
    {"orn",  1, 1},
    {"eor",  2, 0},
    {"eon",  2, 1},
    {"ands", 3, 0},
    {"bics", 3, 1},
    {"tst",  3, 0}, // alias for ands with rd = xzr
    {"mov",  1, 0}, // alias for orr with rn = xzr
    {"mvn",  1, 1}, // alias for orn with rn = xzr
    {NULL, 0, 0}
};

static general_entry condition_table[] = {
    {"b.eq", 0},
    {"b.ne", 1},
    {"b.ge", 10},
    {"b.lt", 11},
    {"b.gt", 12},
    {"b.le", 13},
    {"b.al", 14},
    END_ENTRY
};

static general_entry multiplication_table[] = {
    {"madd", 0},
    {"msub", 1},
    {"mul",  0},  // ra = xzr
    {"mneg", 1},  // ra = xzr
    END_ENTRY
};

static general_entry wide_move_table[] = {
    {"movz", 2},
    {"movn", 0},
    {"movk", 3},
    END_ENTRY
};

void encode_dp_immediate (instruction_t* instr) {
    char *mnemonic = instr->mnemonic;
    reg_t rd = instr->rd;
    reg_t rn = instr->rn;
    operand_t operand = instr->operand;

    uint8_t sf = GET_SF(rd);

    uint32_t binary_rep = 0;

    uint8_t opc = 0, shift = 0;

    int found = lookup_general_entry_table(arithmetic_table, mnemonic, &opc);
    
    if (!found) {
        fprintf(stderr, "Error: Unknown immediate instruction: %s\n", mnemonic);
        return;
    }

    uint32_t imm12 = 0;
    if (operand.type == OP_IMM) { 
        //checks if it is in the range
        if (operand.value.imm < 0 || operand.value.imm > 0xFFF) {
            fprintf(stderr, "Error: Immediate value out of range (0-4095): %d\n", operand.value.imm);
            instr->encoded = 0;
            return;
        }
        imm12 = operand.value.imm & 0xFFF; 
        printf("imm12: %" PRId32 "\n", imm12);
    } else if (operand.type == OP_SHIFTED_IMM) {
        if (operand.value.shifted_imm.shift_amount != 0) {
            shift = 1;
        }
        imm12 = operand.value.shifted_imm.imm & 0xFFF;
        
        printf("imm12: %" PRId32 "\n", imm12);
    } else {
        fprintf(stderr, "Error: Unsupported operand type for DP Immediate.\n");
        instr->encoded = 0;
        return;
    }
    binary_rep |= (sf << 31);              
    binary_rep |= (opc << 29);             
    binary_rep |= (34 << 23); //100010
    binary_rep |= (shift << 22);         
    binary_rep |= (imm12 << 10);           
    binary_rep |= (rn.number << 5);       
    binary_rep |= rd.number; 

    instr->encoded = binary_rep;
} 

void encode_dp_register(instruction_t *instr) {
    char *mnemonic = instr->mnemonic;
    reg_t rd = instr->rd;
    reg_t rn = instr->rn;
    reg_t rm = instr->rm;
    operand_t operand = instr->operand;

    uint32_t binary_rep = 0;

    uint8_t sf = GET_SF(rd);

    uint8_t opc = 0, shift = 0, is_arithmetic = 0, N = 0, shift_amount = 0;


    if (lookup_general_entry_table(arithmetic_table, mnemonic, &opc)) {
        is_arithmetic = 1;
    } else {
        bool found = false;
        for (int i = 0; logical_table[i].mnemonic != NULL; i++){
            if (strcmp(mnemonic, logical_table[i].mnemonic) == 0) {
                opc = logical_table[i].opc;
                N = logical_table[i].N;
                is_arithmetic = 0;
                found = true;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "Error: Unknown DP-register mnemonic: %s\n", mnemonic);
            instr->encoded = 0;
            return;
        }
    }

    if (operand.type == OP_SHIFTED_REG){
        shift = operand.value.shifted_reg.sh_type;
        rm = operand.value.shifted_reg.reg; 
        shift_amount = operand.value.shifted_reg.shift_amount;
    } else if (operand.type == OP_REG) { //default
        shift = LSL;
        rm = operand.value.reg;
        shift_amount = 0; //already 0, but for clarity
    } else {
        fprintf(stderr, "Error: Invalid operand type for DP-Register\n");
        instr->encoded = 0;
        return;
    }

    binary_rep |= (sf << 31);
    binary_rep |= (opc << 29);
    binary_rep |= (5 << 25); // 0101 both for logical and arithmetic
    binary_rep |= (is_arithmetic << 24);  // 1 for arithmetic 0 for logical
    binary_rep |= (shift << 22); 
    binary_rep |= (N << 21); // the N bit           
    binary_rep |= (rm.number << 16);        
    binary_rep |= (shift_amount << 10);               
    binary_rep |= (rn.number << 5);         
    binary_rep |= rd.number;                

    instr->encoded = binary_rep;
}

void encode_multiply(instruction_t *instr) {
    char *mnemonic = instr->mnemonic;
    reg_t rd = instr->rd;
    reg_t rn = instr->rn;
    reg_t ra = instr->ra;
    reg_t rm = instr->rm;

    uint32_t binary_rep = 0;
    uint8_t opc = 0;
    uint8_t sf = GET_SF(rd);
    if (strcmp(mnemonic, "mull") == 0 || strcmp(mnemonic, "mneg") == 0){
        ra.number = 31;
    }

    bool found = lookup_general_entry_table(multiplication_table, mnemonic, &opc);

    if (!found) {
        fprintf(stderr, "Error: Unknown multiply mnemonic: %s\n", mnemonic);
        instr->encoded = 0;
        return;
    }

    binary_rep |= (sf << 31);             
    binary_rep |= (216 << 21);            // 0 0 1 1 0 1 1 0 0 0      
    binary_rep |= (rm.number << 16); 
    binary_rep |= (opc << 15);    // negate or not                   
    binary_rep |= (ra.number << 10);      
    binary_rep |= (rn.number << 5);       
    binary_rep |= rd.number;              

    instr->encoded = binary_rep;
}

void encode_wide_move(instruction_t *instr) {
    const char* mnemonic = instr->mnemonic;
    reg_t rd = instr->rd;
    uint16_t imm16 = instr->imm16;          
    uint8_t shift_amount = instr->shift_amount;

    uint32_t binary_rep = 0;
    uint8_t sf = GET_SF(rd);

    uint8_t opc = 0;
    bool found = lookup_general_entry_table(wide_move_table, mnemonic, &opc);

    if (!found) {
        fprintf(stderr, "Error: Unknown wide move mnemonic: %s\n", mnemonic);
        instr->encoded = 0;
        return;
    }

    if (shift_amount % 16 != 0 || shift_amount > 48) {
        fprintf(stderr, "Error: Invalid shift amount %d for wide move\n", shift_amount);
        instr->encoded = 0;
        return;
    }

    uint8_t hw = shift_amount / 16;

    binary_rep |= (sf << 31);              
    binary_rep |= (opc << 29);             
    binary_rep |= (37 << 23);         // 1 0 0 1 0 1
    binary_rep |= (hw << 21);               
    binary_rep |= (imm16 << 5);            
    binary_rep |= rd.number;                

    instr->encoded = binary_rep;
}

void encode_branch(instruction_t* instr, symbol_table symbols) {
    if (!instr) {
        fprintf(stderr, "Error: instruction is NULL\n");
        instr->encoded = 0;
        return;
    }

    const char* mnemonic = instr->mnemonic;
    const char* label = instr->branch_label;

    

    uint32_t target_address = symbol_table_get(symbols, (char *)label);
    int32_t offset = ((int32_t)target_address - (int32_t)instr->instr_address) >> 2;

    uint32_t binary_rep = 0;

    // Conditional branch
    if (strncmp(mnemonic, "b.", 2) == 0) { 
        if (!symbol_table_find(symbols, (char *)label)) {
            fprintf(stderr, "Error: Undefined label '%s' on address %d\n", label, instr->instr_address);
            instr->encoded = 0;
            return;
        }
        binary_rep |= (84 << 24);  // fixed upper 8 bits 01010100

        if (offset < -(1 << 18) || offset >= (1 << 18)) {
            fprintf(stderr, "Error: Branch offset out of range on line %d\n", instr->instr_address);
            instr->encoded = 0;
            return;
        }

        binary_rep |= ((offset & 0x7FFFF) << 5);  // imm19

        bool found = false;
        for (int i = 0; condition_table[i].mnemonic != NULL; i++) {
            if (strcmp(mnemonic, condition_table[i].mnemonic) == 0) {
                binary_rep |= condition_table[i].opc;  // cond bits
                found = true;
                break;
            }
        }

        if (!found) {
            fprintf(stderr, "Error: Unknown condition mnemonic '%s' on line %d\n", mnemonic, instr->instr_address);
            instr->encoded = 0;
            return;
        }
    }

    // Unconditional: b
    else if (strcmp(mnemonic, "b") == 0) {
        if (!symbol_table_find(symbols, (char *)label)) {
            fprintf(stderr, "Error: Undefined label '%s' on address %d\n", label, instr->instr_address);
            instr->encoded = 0;
            return;
        }
        if (!instr->branch_label) {
            fprintf(stderr, "Instruction does not have branch label\n");
            instr->encoded = 0;
            return;
        }
        uint8_t opcode = 5;

        if (offset < -(1 << 25) || offset >= (1 << 25)) {
            fprintf(stderr, "Error: Branch offset out of range on line %d\n", instr->instr_address);
            instr->encoded = 0;
            return;
        }

        binary_rep |= (opcode << 26);             // bits 31–26
        binary_rep |= (offset & 0x03FFFFFF);      // imm26
    }
    else if (strcmp(mnemonic, "br") == 0) {
        uint32_t bin_val = 3508160;
        int reg_no = instr->rn.number;
        printf("reg no: %d\n", reg_no);
        binary_rep |= (bin_val << 10);             // bits 31–26
        binary_rep |= ((reg_no & 0x1F) << 5);      // imm26
    }

    else {
        fprintf(stderr, "Error: Unsupported branch mnemonic '%s' on line %d\n", mnemonic, instr->instr_address);
        instr->encoded = 0;
        return;
    }

    instr->encoded = binary_rep;
}

void encode_load_store(instruction_t* instr, symbol_table symbols) {
    if (!instr) return;

    uint32_t binary_rep = 0;

    reg_t rt = instr->rt;
    reg_t base = {REG_UNDEFINED, -1};
    uint8_t sf = GET_SF(rt);
    uint8_t opc = (instr->address.dt == LOAD) ? 1 : 0;
    uint8_t size = (sf == 1) ? 3 : 2;  // 64-bit = 11, 32-bit = 10
    int scale = (sf == 1) ? 8 : 4;

    addressing_mode_t addr = instr->address;
    switch (addr.type) {
        
        case UNSIGNED_IMM_OFFSET: 
        case UNSIGNED_IMM_ZERO_OFFSET: {
            uint32_t imm = addr.value.unsigned_offset.imm;
            base = addr.value.unsigned_offset.xn;

            if ((imm % scale != 0) || (imm / scale > 0xFFF)) {
                fprintf(stderr, "Error: Offset must be a multiple of %d and <= %d\n", scale, 0xFFF * scale);
                return;
            }

            if (addr.type == UNSIGNED_IMM_ZERO_OFFSET) {
                imm = 0;
            }

            uint32_t imm12 = imm / scale;

            binary_rep |= (size << 30);              // [31–30] size
            binary_rep |= (7 << 27);                // [29–27] fixed bits for load/store
            binary_rep |= (0 << 26);                // [26] = 0 for load/store (not literal)
            binary_rep |= (1 << 24);                 // [24] U = 1 (unsigned offset)
            binary_rep |= (opc << 22);               // [22] L = 1 (load), 0 (store)
            binary_rep |= (imm12 << 10);             // [21–10] offset
            binary_rep |= (base.number << 5);        // [9–5] base register
            binary_rep |= rt.number;                 // [4–0] Rt
            break;
        }

        case PRE_IND: 
        case POST_IND: {
            int32_t simm9 = addr.value.pre_post_indexed.simm;
            base = addr.value.pre_post_indexed.xn;

            if (simm9 < -256 || simm9 > 255) {
                fprintf(stderr, "Error: simm9 out of range (-256 to 255)\n");
                return;
            }

            uint32_t encoded_simm9 = (uint32_t)(simm9 & 0x1FF);
            int i = addr.type == POST_IND ? 0 : 1;

            binary_rep |= (size << 30);
            binary_rep |= (7 << 27);
            binary_rep |= (0 << 26);                 // [26] = 0 (not literal)
            binary_rep |= (0 << 24);                 // [24] U = 0 (pre/post-indexed)
            binary_rep |= (i << 11);                 // [11] I = 1 (pre-indexed)
            binary_rep |= (1 << 10);                 // [11] I = 1 (pre-indexed)
            binary_rep |= (opc << 22);               // [22] load/store
            binary_rep |= (encoded_simm9 << 12);     // [20–12] simm9
            binary_rep |= (base.number << 5);
            binary_rep |= rt.number;
            break;
        }

        case REGISTER: {
            base = addr.value.register_offset.xn;
            reg_t offset_reg = addr.value.register_offset.xm;

            binary_rep |= (size << 30);
            binary_rep |= (7 << 27);
            binary_rep |= (0 << 26);                  // Not literal
            binary_rep |= (opc << 22);
            binary_rep |= (1 << 21);                  // Option bit for register offset
            binary_rep |= (offset_reg.number << 16);
            binary_rep |= (26 << 10);  // offset register
            binary_rep |= (base.number << 5);
            binary_rep |= rt.number;
            break;
        }

        case LITERAL: {
            const char* label = instr->address.value.literal.label;
            printf("label: %s\n", label);
            uint32_t target_addr;

            if (label == NULL) {
                // immediate 
                target_addr = instr->address.value.literal.int_directive;
            } else {
                if (!label || !symbol_table_find(symbols, (char*)label)) {
                    fprintf(stderr, "Error: Undefined label '%s' on line %d\n", label, instr->instr_address);
                    instr->encoded = 0; 
                    return;
                }
                target_addr = symbol_table_get(symbols, (char*)label);
            }
           
            uint32_t pc = instr->instr_address;  // assuming each instruction is 4 bytes
            int32_t offset = ((int32_t)target_addr - (int32_t)pc) / 4;

            if (offset < -262144 || offset > 262143) {  // simm19 range
                fprintf(stderr, "Error: Literal offset out of range on line %d\n", instr->instr_address);
                instr->encoded = 0;
                return;
            }

            uint8_t sf = (instr->rt.type == REG_X) ? 1 : 0;

            binary_rep |= (sf << 30);
            binary_rep |= (3 << 27);       // bits 29–27 for Load literal
            binary_rep |= ((offset & 0x7FFFF) << 5); // simm19 in bits 23–5
            binary_rep |= instr->rt.number;
            break;
        }

        default:
            fprintf(stderr, "Error: Unsupported addressing mode for load/store\n");
            return;
    }
    instr->encoded = binary_rep;
}


void encode_directive(instruction_t* instr) {
    instr->encoded = instr->directive_value;
}

void encode_unknown(instruction_t* instr) {
    fprintf(stderr, "Error: Unknown instruction on line %d", instr->instr_address);
    instr->encoded = 0;
}

bool lookup_general_entry_table(general_entry *table, const char *mnemonic, uint8_t *opc) {
    for (int i = 0; table[i].mnemonic != NULL; i++){
        if (strcmp(mnemonic, table[i].mnemonic) == 0) {
            *opc = table[i].opc;
            return true;
        }
    }
    return false;
}

void encode_instructions(instruction_t **instrs, int instruction_count, symbol_table sym_table) {
    for (int i = 0; i < instruction_count; i++) {
        instruction_t *instr = instrs[i];
        switch (instr->type) {
            case INSTR_DATA_PROC_IMM: {
                encode_dp_immediate(instr);
                break;
            }
            case INSTR_DATA_PROC_REG: {
                encode_dp_register(instr);
                break;
            }
            case INSTR_MULTIPLY: {
                encode_multiply(instr);
                break;
            }
            case INSTR_WIDE_MOVE: {
                encode_wide_move(instr);
                break;
            }
            case INSTR_BRANCH: {
                encode_branch(instr, sym_table);
                break;
            }
            case INSTR_LOAD_STORE: {
                encode_load_store(instr, sym_table);
                break;
            }
            case INSTR_DIRECTIVE: {
                encode_directive(instr);
                break;
            }
            case INSTR_UNKNOWN: {
                encode_unknown(instr);
                break;
            }
            
        }
    }
}
