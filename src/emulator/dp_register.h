#ifndef DP_REGISTER
#define DP_REGISTER

#include <stdio.h>
#include <stdlib.h>
#include "machine_state.h"


/**
 * @def AND
 * @brief Opcode value for logical AND operation.
 * 
 * When N bit is 1, this represents BIC (Bit Clear) operation.
 */

#define AND 0 

/**
 * @def ORR
 * @brief Opcode value for logical OR operation.
 * 
 * When N bit is 1, this represents ORN (OR NOT) operation.
 */

#define ORR 1  

/**
 * @def EOR
 * @brief Opcode value for logical Exclusive OR (XOR) operation.
 * 
 * When N bit is 1, this represents EON (Exclusive OR NOT) operation.
 */

#define EOR 2

/**
 * @def ANDS
 * @brief Opcode value for logical AND operation that also updates status flags.
 * 
 * When N bit is 1, this represents BICS (Bit Clear and update flags) operation.
 */
 
#define ANDS 3

/**
 * @brief Handles the execution of Data Processing instructions of type "Register".
 * 
 * This function decodes the instruction to determine whether it is a logical/arithmetic 
 * operation or a multiply operation and dispatches it accordingly.
 * 
 * @param state Pointer to the CPU machine state.
 * @param instr 32-bit encoded ARM instruction to execute.
 */

void data_proc_reg(
    STATE* state, 
    uint32_t instr
);

/**
 * @brief Executes arithmetic or logical operations on registers.
 * 
 * Decodes the instruction fields to determine operation type (arithmetic or logical),
 * shift type, registers involved, and performs the operation while updating the machine state.
 * 
 * @param state Pointer to the CPU machine state.
 * @param instr 32-bit encoded ARM instruction specifying the operation.
 */

void arithmetic_logical_operations(
    STATE* state, 
    uint32_t instr
);

/**
 * @brief Executes multiply operations, including multiply-add and multiply-subtract.
 * 
 * Validates instruction fields specific to multiply operations and calculates the
 * result accordingly, updating the appropriate register in the machine state.
 * 
 * @param state Pointer to the CPU machine state.
 * @param instr 32-bit encoded ARM instruction specifying the multiply operation.
 */
 
void multiply_operations(
    STATE* state, 
    uint32_t instr
);

#endif
