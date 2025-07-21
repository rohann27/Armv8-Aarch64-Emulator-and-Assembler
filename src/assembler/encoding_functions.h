#ifndef ENCODING_FUNCTIONS_H
#define ENCODING_FUNCTIONS_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "instruction_representation.h"
#include "symbol_table.h"  
#include "../emulator/bitwise_shifts.h"

#define END_ENTRY {NULL, 0}
#define GET_SF(reg) ((reg.type == REG_X || reg.type == REG_XZR) ? 1 : 0)

typedef struct {
    const char* mnemonic;
    uint8_t opc;     // 00, 01, 10, 11
    uint8_t N;       // 0 for normal, 1 for NOT variants
} dp_logical_entry_t;

typedef struct {
    const char* mnemonic;
    uint8_t opc;              
} general_entry;



void encode_dp_immediate(instruction_t* instr);
void encode_dp_register(instruction_t* instr);
void encode_multiply(instruction_t* instr);
void encode_wide_move(instruction_t* instr);
void encode_branch(instruction_t* instr, symbol_table symbols);
void encode_load_store(instruction_t* instr, symbol_table symbols);
void encode_directive(instruction_t* instr);
void encode_unknown(instruction_t* instr);
void encode_instructions(instruction_t **instrs, int instruction_count, symbol_table sym_table);
bool lookup_general_entry_table(general_entry *table, const char *mnemonic, uint8_t *opc);

#endif
