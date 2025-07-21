#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "machine_state.h"
#include "utils.h"
#include "bitwise_shifts.h"
#include "dp_register.h"

//handles different M's seperately
void data_proc_reg(
    STATE* state, 
    uint32_t instr
){
    uint32_t type_identifier = getRangeInt(instr, 27, 25);
    uint32_t M = getRangeInt(instr, 28, 28);
    if (type_identifier != 5){ 
        fprintf(stderr, "The instruction is not a Data Processing Instruction (Register)");
        exit(EXIT_FAILURE);
    }
    if (M == 0){ // Logical or Arithmetic
        arithmetic_logical_operations(state, instr);
    } else { // Multiply
        multiply_operations(state, instr);
    }
}

void arithmetic_logical_operations(
    STATE* state, 
    uint32_t instr
) {
    uint32_t sf = getRangeInt(instr, 31, 31); //0 -> 32 bit , 1 -> 64 bit
    uint32_t opc = getRangeInt(instr, 30, 29);
    uint32_t operation_type = getRangeInt(instr, 24, 24); //1 -> arth, 0 -> logic
    uint32_t shift_type = getRangeInt(instr, 23,22);
    uint32_t N = getRangeInt(instr, 21, 21);
    uint32_t rm = getRangeInt(instr, 20, 16);
    uint32_t operand = getRangeInt(instr, 15, 10); //shift amount
    uint32_t rn = getRangeInt(instr, 9, 5);
    uint32_t rd = getRangeInt(instr, 4, 0);

    int is_32bit = (sf == 0);
    
     if(shift_type == ROR && operation_type != 0) {
        fprintf(stderr, "ror shifts are only valid for logical instructions");
        exit(EXIT_FAILURE);    
        }
    
    uint64_t rn_value = get_register(state, rn, is_32bit);
    uint64_t rm_value = get_register(state, rm, is_32bit);
    uint64_t op2 = bitwise_shift(rm_value, shift_type, operand, !is_32bit); 

    if (operation_type == 0) { // Logical Operation
        uint64_t operand2 = (N == 1) ? ~op2 : op2; // N == 1 -> Negated op2
        uint64_t result;
        switch (opc) {
            case AND:
            case ANDS:
                result = rn_value & operand2;
                break;
            case ORR: 
                result = rn_value | operand2;
                break;
            case EOR: 
                result = rn_value ^ operand2;
                break;
            default:
                fprintf(stderr, "Unsupported logical opcode: %u\n", opc);
                exit(EXIT_FAILURE);
        }

        if (opc == ANDS) {
            state->pstate.N = (result >> (is_32bit ? 31 : 63)) & 1;
            state->pstate.Z = (result == 0);
            state->pstate.C = 0;
            state->pstate.V = 0;
        }

        if (rd != 31) { 
            set_register(state, rd, result, is_32bit);
        }
        
    } else { // Arithmetic Operation
         if(N != 0){
            fprintf(stderr, "21th bit has to be 0 for arithmetic operations");
            exit(EXIT_FAILURE);
        }
        perform_arithmetic_op(state, rd, rn_value, op2, opc, is_32bit);
    }
}

void multiply_operations(
    STATE* state, 
    uint32_t instr
){
    uint32_t sf = getRangeInt(instr, 31, 31);
    uint32_t opc = getRangeInt(instr, 30, 29); // has to be 00
    uint32_t opr = getRangeInt(instr, 24, 21); //has to be 1000
    uint32_t rm = getRangeInt(instr, 20, 16);
    uint32_t x = getRangeInt(instr, 15, 15);
    uint32_t ra = getRangeInt(instr, 14, 10);
    uint32_t rn = getRangeInt(instr, 9, 5);
    uint32_t rd = getRangeInt(instr, 4, 0);

    int is_32bit = (sf == 0);

    if (opc != 0){
        fprintf(stderr, "opc must be 00 for multiply operations");
        exit(EXIT_FAILURE);
    }

    if (opr != 8){
        fprintf(stderr, "opr must be 1000 for multiply operations");
        exit(EXIT_FAILURE);
    }

    uint64_t ra_value = get_register(state, ra, is_32bit);
    uint64_t rn_value = get_register(state, rn, is_32bit);
    uint64_t rm_value = get_register(state, rm, is_32bit);

    uint64_t result = (x == 0) ? 
                  (ra_value + (rn_value * rm_value)) 
                : (ra_value - (rn_value * rm_value));

    if (rd != 31) {
        set_register(state, rd, result, is_32bit);
    }
}