#ifndef DATA_PROC_H
#define DATA_PROC_H

#include "machine_state.h"

#define ARITHMETIC 2
#define WIDE_MOVE 5

#define SHIFT 1

#define MOVN 0
#define MOVZ 2
#define MOVK 3

/**
 * data_proc_imm - Decodes and executes a data-processing immediate instruction.
 *
 * Supports two main instruction classes:
 *   1. ARITHMETIC     (e.g., ADD(S), SUB(S)): performs operations with a 12-bit immediate,
 *      with optional shifting by 12 bits.
 *   2. WIDE_MOVE      (e.g., MOVZ, MOVN, MOVK): builds 32/64-bit values using 16-bit chunks
 *      placed into specific half-word positions.
 *
 * @param state pointer to the current machine state (registers, etc.)
 * @param instr 32-bit encoded ARM instruction
 *
 * Behavior:
 *   - Decodes fields from the instruction (sf, opc, opi, rd, imm, etc.)
 *   - Selects the operation type based on `opi` and `opc`
 *   - Applies appropriate transformations (e.g., zeroing, NOT, inserting 16-bit chunks)
 *   - Updates the destination register with the result
 *
 * Exits with failure if the instruction class (opi) is unsupported.
 */

void data_proc_imm(
    STATE* state, 
    uint32_t instr
);

#endif
