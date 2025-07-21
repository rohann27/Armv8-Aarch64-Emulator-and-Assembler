#ifndef BRANCH_INSTRUCTIONS_H
#define BRANCH_INSTRUCTIONS_H

#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>

/**
 * @enum ConditionCode
 * @brief Enumerates the condition codes used for conditional branching.
 *
 * These values correspond to condition codes used in ARM64 branching instructions.
 */

typedef enum {
    EQ = 0x0,
    NE = 0x1,
    GE = 0xA,
    LT = 0xB,
    GT = 0xC,
    LE = 0xD,
    AL = 0xE
} ConditionCode;

/**
 * @brief Evaluates whether a condition code is satisfied given the processor state.
 * 
 * @param state Pointer to the current machine state.
 * @param cond The condition code to evaluate.
 * @return true if the condition is satisfied; false otherwise.
 */

void branch(
    STATE *state, 
    uint32_t instr
);

/**
 * @brief Executes a branch instruction.
 *
 * Determines the type of branch based on the instruction bits and executes
 * the appropriate branch (unconditional, conditional, or register).
 * 
 * @param state Pointer to the current machine state.
 * @param instr The 32-bit encoded instruction.
 */

void branch_unconditional(
    STATE *state, 
    uint64_t instr
);

/**
 * @brief Performs an unconditional branch.
 *
 * Extracts a signed offset from the instruction and updates the PC accordingly.
 * 
 * @param state Pointer to the current machine state.
 * @param instr The 64-bit encoded instruction.
 */

void branch_register(
    STATE *state, 
    uint64_t instr
);

/**
 * @brief Performs a register-based branch.
 *
 * Sets the PC to the value stored in a specified register.
 * 
 * @param state Pointer to the current machine state.
 * @param instr The 64-bit encoded instruction.
 */

void branch_conditional(
    STATE *state, 
    uint64_t instr
);

/**
 * @brief Performs a conditional branch.
 *
 * Evaluates a condition code and updates the PC with an offset if the condition is true.
 * Otherwise, the PC is incremented by 4 (to fetch the next instruction).
 * 
 * @param state Pointer to the current machine state.
 * @param instr The 64-bit encoded instruction.
 */

bool eval_cond(
    STATE *state, 
    uint32_t cond
);


#endif 
