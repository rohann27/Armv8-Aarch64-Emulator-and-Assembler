#include "parser.h"


parser_state_t *create_parser_state(void) {
    parser_state_t *state = malloc(sizeof(parser_state_t));

    if (!state) {
        fprintf(stderr, "Failed to allocate memory for parser state.\n");
        exit(EXIT_FAILURE);
    }

    // init fields
    state->tokens = token_list_create();
    state->instruction_count = 0;
    state->instruction_capacity = INITIAL_INSTRUCTION_CAPACITY;

    state->instructions = malloc(sizeof(instruction_t*) * state->instruction_capacity);
    assert(state->instructions != NULL);

    state->pc = 0;
    state->current_line = 1;
    return state;
}

void add_instruction_to_parser_state(parser_state_t *state, instruction_t *instr) {
    if (state->instruction_count+1 >= state->instruction_capacity) {
        state->instruction_capacity *= 2;
        state->instructions = realloc(state->instructions, state->instruction_capacity * sizeof(instruction_t *));
        assert(state->instructions != NULL);
    }
    state->instructions[state->instruction_count++] = instr;
}

void free_parser_state(parser_state_t *parser) {
    if (!parser) return;
    free_token_list(parser->tokens);
     // Free instructions
    for (int i = 0; i < parser->instruction_count; i++) {
        if (parser->instructions[i]) {
            free_instruction(parser->instructions[i]);
        }
    }
    free(parser->instructions);
    free(parser);
}

void parse_dp(instruction_t *instr, token_list_t tokens, token_t *mnemonic) {
    char *opc = mnemonic->value;
    
    if (IS_DP_MULTIPLY(opc)) {
        // First, get the registers as tokens
        token_t *rd_tok
            = expect(tokens, TOKEN_REGISTER, 
                "Expected a register token after multiply mnemonic\n"
            );
        token_t *rn_tok
            = expect(tokens, TOKEN_REGISTER, 
                "Expected a register token after first register in multiply\n"
            );
        token_t *rm_tok
            = expect(tokens, TOKEN_REGISTER, 
                "Expected a register token after second register in multiply\n"
            );
        token_t *ra_tok
            = expect(tokens, TOKEN_REGISTER, 
                "Expected a register token after third register in multiply\n"
            );
        // Then, create `reg_t` structs for each register token defined above
        reg_t rd = string_to_reg_t(rd_tok->value);
        reg_t rn = string_to_reg_t(rn_tok->value);
        reg_t rm = string_to_reg_t(rm_tok->value);
        reg_t ra = string_to_reg_t(ra_tok->value);

        // Set up the instruction
        instr->type = INSTR_MULTIPLY;
        instr->rd = rd;
        instr->rn = rn;
        instr->rm = rm;
        instr->ra = ra;
    } else if (IS_DP_TWO_OPERAND(opc)) {
         // First, get the registers as tokens
        token_t *rd_tok
            = expect(tokens, TOKEN_REGISTER, 
                "Expected a register token after two operand mnemonic\n"
            );
        token_t *rn_tok
            = expect(tokens, TOKEN_REGISTER, 
                "Expected a register token after first register in two operand instruction\n"
            );
       
        operand_t operand = parse_operand(tokens, instr);

        // Then, create `reg_t` structs for each register token defined above
        reg_t rd = string_to_reg_t(rd_tok->value);
        reg_t rn = string_to_reg_t(rn_tok->value);

        instr->rd = rd;
        instr->rn = rn;
        // if it is an alias, then parse accordingly
        if (IS_OPC_ALIAS(opc)) {
            strncpy(instr->mnemonic, resolve_alias_mnemonic(opc), sizeof(instr->mnemonic));
            reg_t rm = operand.value.reg;
            reg_t rzr = {.type = (rd.type == REG_X || rd.type == REG_XZR) ? REG_XZR : REG_WZR, .number = 31};
            instr->type = INSTR_MULTIPLY;
            instr->rm = rm;
            instr->ra = rzr;
            return;
        }
        instr->operand = operand;
    } else if (IS_DP_ONE_OPERAND_W_DEST(opc)) {
        // deal with aliases
        // First, get the registers as tokens
        token_t *rd_tok
            = expect(tokens, TOKEN_REGISTER, 
                "Expected a register token after two operand mnemonic\n"
            );
        // Then, create `reg_t` structs for each register token defined above
        reg_t rd = string_to_reg_t(rd_tok->value);
        instr->rd = rd;

        // The next token is an operand, which is either:
        operand_t operand = parse_operand(tokens, instr);
        
        if (IS_OPC_ALIAS(opc)) {
            strncpy(instr->mnemonic, resolve_alias_mnemonic(opc), sizeof(instr->mnemonic));
            reg_t rzr = {.type = (rd.type == REG_X || rd.type == REG_XZR) ? REG_XZR : REG_WZR, .number = 31};
            if (strcmp(opc, "mov") == 0) {
                reg_t rm = operand.value.reg;
                instr->rm = rm;
            } 
            instr->rn = rzr;
        }
        if (IS_WIDE_MOVE(opc)) {
            instr->type = INSTR_WIDE_MOVE;
            if (operand.type == OP_IMM) {
                instr->imm16 = operand.value.imm;
            }
            else if (operand.type == OP_SHIFTED_IMM) {
                instr->imm16 = operand.value.shifted_imm.imm;
                instr->shift_amount = operand.value.shifted_imm.shift_amount;
            }
                
        }
        instr->operand = operand;
    } else if (IS_DP_TWO_OPERAND_WO_DEST(opc)) {
        // First, get the registers as tokens
        token_t *rn_tok
            = expect(tokens, TOKEN_REGISTER, 
                "Expected a register token after two operand mnemonic\n"
            );
        // The next token is an operand, which is either:
        operand_t operand = parse_operand(tokens, instr);
        // Then, create `reg_t` structs for each register token defined above
        reg_t rn = string_to_reg_t(rn_tok->value);
        instr->operand = operand;
        if (IS_OPC_ALIAS(opc)) {
            strncpy(instr->mnemonic, resolve_alias_mnemonic(opc), sizeof(instr->mnemonic));
            reg_t rzr = {.type = (rn.type == REG_X || rn.type == REG_XZR) ? REG_XZR : REG_WZR, .number = 31};
            instr->rd =  rzr;
            instr->rn = rn;
            return;
        } 
        instr->rd = rn;
    } else {
        // FAIL?
    }
}

void parse_branch(instruction_t *instr, token_list_t tokens, token_t *mnemonic) {
    char *opc = mnemonic->value;
    instr->type = INSTR_BRANCH;

    if (strcmp(opc, "br") == 0) {
        token_t *xn = current_token(tokens);
        instr->rn = string_to_reg_t(xn->value);
        advance(tokens);
        return;
    }
    // parse the literal
    token_t *literal_token = current_token(tokens);
    advance(tokens);
    instr->branch_label = literal_token->value;
    return;
}

void parse_load_store(instruction_t *instr, token_list_t tokens, token_t *mnemonic) {
    char *opc = mnemonic->value;

    token_t *rt_tok
            = expect(tokens, TOKEN_REGISTER, 
                "Expected a register token after load/store mnemonic\n"
            );
    reg_t rt = string_to_reg_t(rt_tok->value);
    instr->rt = rt;

    // Set the addressing mode
    if (strcmp(opc, "ldr") == 0) {
        // check if the address is 
        addressing_mode_t addr = parse_addressing_mode(tokens, LOAD);
        instr->address = addr;
    } else if (strcmp(opc, "str") == 0) {
        addressing_mode_t addr = parse_addressing_mode(tokens, STORE);
        instr->address = addr;
    } else {
        // FAIL?
    }
    instr->type = INSTR_LOAD_STORE;
}

void parse_instructions(parser_state_t *parser_state) {
    token_list_t tokens = parser_state->tokens;

    // Loop until we see an end of file token, which
    // indicates we consumed all the tokens in a file
    while (!match(tokens, TOKEN_EOF)) {

        // If we encounter a label, advance the current
        // token and ...
        if (match(tokens, TOKEN_LABEL)) {
            // Label already added to symbol table in pass 1, just expect
            // a new line after the label token and consume that token too
            // to continue with the next instruction
            expect(tokens, TOKEN_NEWLINE, "Expected newline after label");
            continue;
        }

        instruction_t *instr = create_instruction();

        // If a directive token is encountered, advance the 
        // current token and ...
        if (match(tokens, TOKEN_DIRECTIVE)) {
            // the directive value is now the current
            // token (match advances the cursor)
            token_t *dir_value = current_token(tokens);
            // advance to expect a new line after the value
            advance(tokens);
            // Set instruction's fields
            instr->type = INSTR_DIRECTIVE;
            instr->directive_value = string_to_immediate(dir_value->value);
        }
        // We expect a mnemonic token now... 
        else {
            // If we encounter a mnemonic token, advance the current token
            // so that it points to the token after the instruction mnemonic
            token_t *mnemonic_tok = expect(tokens, TOKEN_MNEMONIC, "Expected instruction mnemonic");
            // Set instruction's mnemonic value to the token's value
            strncpy(instr->mnemonic, mnemonic_tok->value, sizeof(instr->mnemonic));

            // Now, the current token points to the token after the 
            // instruction mnemonic

            // Parse the relevant instruction type
            if (IS_DP_MNEMONIC(instr->mnemonic)) {
                parse_dp(instr, tokens, mnemonic_tok);
            } else if (IS_BRANCHING_MNEMONIC(instr->mnemonic)) {
                parse_branch(instr, tokens, mnemonic_tok);
            } else if (IS_LOAD_STORE_MNEMONIC(instr->mnemonic)) {
                parse_load_store(instr, tokens, mnemonic_tok);
            } else {
                fprintf(stderr, "Unknown instruction mnemonic: %s\n", instr->mnemonic);
                exit(1);
            }
        }

        // After each instruction (mnemonic or directive), expect a new line ...
        expect(tokens, TOKEN_NEWLINE, "Expected newline after instruction or directive");
        
        // Save the parsed instruction to parser state, incrementing
        // isntruction address and parser pc
        instr->instr_address = parser_state->pc;
        parser_state->pc += PC_INC;
        add_instruction_to_parser_state(parser_state, instr);
    }
}

void parse(parser_state_t *parser_state, FILE *in_file) {
    // PARSING FLOW:
    //      1. Tokenise
    //      2. Parse
    //      3. Encode
    parser_state->tokens = tokenise(in_file);
    parse_instructions(parser_state);
    fclose(in_file);
}

operand_t parse_operand(token_list_t tokens, instruction_t *instr) {
    operand_t operand = {0};
    token_t *curr_token = current_token(tokens); 
    if (curr_token->type == TOKEN_REGISTER) {
        instr->type = INSTR_DATA_PROC_REG;
        advance(tokens);
        token_t *shift_token = current_token(tokens);
        if (shift_token->type == TOKEN_SHIFT) {
            advance(tokens);
            token_t *shift_value_tok = expect(tokens, TOKEN_IMMEDIATE, "Expected immediate shift value\n");
            uint32_t shift_amt = string_to_immediate(shift_value_tok->value);
            operand = (operand_t) {.type = OP_SHIFTED_REG, .value.shifted_reg = {
                .reg = string_to_reg_t(curr_token->value),
                .sh_type = string_to_shift_type(shift_token->value),
                .shift_amount = shift_amt
            }};
        } else {
            operand = (operand_t) {.type = OP_REG, .value.reg = string_to_reg_t(curr_token->value)};
        }
    } 
    else if (curr_token->type == TOKEN_IMMEDIATE) {
        instr->type = INSTR_DATA_PROC_IMM;
        advance(tokens);
        token_t *shift_token = current_token(tokens);
        if (shift_token->type == TOKEN_SHIFT) {
            advance(tokens);
            token_t *shift_value_tok = expect(tokens, TOKEN_IMMEDIATE, "Expected immediate shift value\n");
            uint32_t shift_amt = string_to_immediate(shift_value_tok->value);
            operand = (operand_t) {.type = OP_SHIFTED_IMM, .value.shifted_imm = {
                .imm = string_to_immediate(curr_token->value),
                .sh_type = string_to_shift_type(shift_token->value),
                .shift_amount = shift_amt
            }};
        } else {
            operand = (operand_t) {.type = OP_IMM, .value.imm = string_to_immediate(curr_token->value)};
        }
    }
    return operand;
}

addressing_mode_t parse_addressing_mode(token_list_t tokens, dt_type type) {
    addressing_mode_t addr;
    addr.dt = type;
    token_t *current_tok = current_token(tokens);
    if (type == LOAD && current_tok->type == TOKEN_LITERAL) {
        // literal addressing mode
        // literal can be either an integer or a label
        literal_t lit_val = {0};
        // if current_tok[0] == # --> integer else label
        if (isdigit(current_tok->value[0])) {
            lit_val.int_directive = string_to_immediate(current_tok->value);
        } else {
            lit_val.label = current_tok->value;
        }
        addr.value.literal = lit_val;
        addr.type = LITERAL;
        advance(tokens);
        return addr;
    }  
    expect(tokens, TOKEN_LBRACKET, "Expected '[' after rt in load/store instruction");
    token_t *xn_tok = expect(tokens, TOKEN_REGISTER, "Expected a register in addressing mode\n");
    reg_t xn = string_to_reg_t(xn_tok->value);
    if (current_token(tokens)->type == TOKEN_RBRACKET) {
        if (peek_next(tokens)->type == TOKEN_IMMEDIATE) {
            // post-indexed addressing mode
            advance(tokens);
            token_t *simm_tok = current_token(tokens);
            addr.type = POST_IND;
            addr.value.pre_post_indexed.xn = xn;
            addr.value.pre_post_indexed.simm = string_to_immediate(simm_tok->value);
            advance(tokens);
            return addr;
        } else if (peek_next(tokens)->type == TOKEN_NEWLINE) {
            // zero unsigned offset addressing mode
            addr.type = UNSIGNED_IMM_ZERO_OFFSET;
            addr.value.unsigned_offset.xn = xn;
            addr.value.unsigned_offset.imm = 0;
            advance(tokens);
            return addr;
        } else {
            fprintf(stderr, "Error, invalud addressing mode");
            exit(EXIT_FAILURE);
        }
    } 
    if (current_token(tokens)->type == TOKEN_REGISTER) {
        // register offset
        token_t *xm_tok = current_token(tokens);
        reg_t xm = string_to_reg_t(xm_tok->value);
        advance(tokens);
        expect(tokens, TOKEN_RBRACKET, "Expected ']' after xm in register offset addressing mode");
        addr.type = REGISTER;
        addr.value.register_offset.xn = xn;
        addr.value.register_offset.xm = xm;
        return addr;
    }
    if (current_token(tokens)->type == TOKEN_IMMEDIATE) {
        token_t *immtok = current_token(tokens);
        advance(tokens);
        expect(tokens, TOKEN_RBRACKET, "Expected ']' after immediate operand in addressing mode");
        token_t *curr_tok = current_token(tokens);
        if (curr_tok->type == TOKEN_EXCLAMATION) {
            // Pre-indexed
            advance(tokens);
            addr.type = PRE_IND;
            addr.value.pre_post_indexed.xn = xn;
            addr.value.pre_post_indexed.simm = string_to_immediate(immtok->value);
            return addr;
        }
        if (curr_tok->type == TOKEN_NEWLINE) {
            // Unsinged offset
            addr.type = UNSIGNED_IMM_OFFSET;
            addr.value.unsigned_offset.xn = xn;
            addr.value.unsigned_offset.imm = string_to_immediate(immtok->value);
        } else {
            fprintf(stderr, "Error: invalid addresing mode");
            exit(EXIT_FAILURE);
        }  
    }
    return addr;
}
