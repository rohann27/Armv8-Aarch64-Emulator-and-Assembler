#ifndef SINGLE_DATA_TRANSFER_H
#define SINGLE_DATA_TRANSFER_H

#include <stdio.h>
#include <stdlib.h>
#include "machine_state.h"


/**
 * Performs a single data transfer instruction (load or store).
 * This function executes a memory access operation using one of the 
 * following addressing modes:
 * - unsigned immediate offset
 * - register offset
 * - pre/post-indexed offset
 *
 * It handles both 32-bit and 64-bit register accesses depending on the `sf` bit.
 * For a load, the target register is updated with memory content.
 * For a store, the memory is updated with the value in the target register.
 *
 * @param state Pointer to the current machine state.
 * @param instr The 32-bit binary representation of the instruction.
 */
void single_data_transfer(
    STATE* state, 
    uint32_t instr
);


/**
 * Loads a value from a literal memory address into a register.
 *
 * This function calculates the memory address using PC-relative addressing:
 * `address = PC + simm19 * 4` and loads a value from that memory
 * location into a target register `rt`. It supports both 32-bit and 64-bit loads
 * depending on the `sf` bit.
 *
 * @param state Pointer to the current machine state.
 * @param instr The 32-bit binary representation of the instruction.
 */
void load_from_literal(
    STATE *state, 
    uint32_t instr
);

/**
 * Dispatches the memory access instruction based on the opcode.
 *
 * This function inspects specific bits of the instruction to determine
 * whether to perform load from literal or single data transfer operation.
 *
 * @param state Pointer to the current machine state.
 * @param instr The 32-bit binary representation of the instruction.
 */
void data_transfer(
    STATE *state, 
    uint32_t instr
);

#endif
