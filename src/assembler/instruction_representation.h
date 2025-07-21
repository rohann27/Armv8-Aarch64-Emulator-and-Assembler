#ifndef INSTRUCTION_REPRESENTATION_H
#define INSTRUCTION_REPRESENTATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <regex.h>
#include <stdbool.h>

#include "../emulator/bitwise_shifts.h"

/**
 * @brief Types of ARM instructions recognized by the assembler
 */

typedef enum {
    INSTR_DATA_PROC_IMM,     // Data processing with immediate
    INSTR_DATA_PROC_REG,     // Data processing with register
    INSTR_MULTIPLY,          // Multiply instructions
    INSTR_WIDE_MOVE,         // Wide move instructions
    INSTR_BRANCH,            // Branch instructions
    INSTR_LOAD_STORE,        // Load/store instructions
    INSTR_DIRECTIVE,         // Assembler directives
    INSTR_UNKNOWN
} instruction_type_t;

/**
 * @brief Register types
 */

typedef enum {
    REG_X, 
    REG_W, 
    REG_SP, 
    REG_XZR, 
    REG_WZR, 
    REG_PC, 
    REG_UNDEFINED
} reg_type_t;

/**
 * @brief Describes a CPU register
 */

typedef struct {
    reg_type_t type;
    int number;
} reg_t;

/**
 * @brief Represents a generic operand in an instruction
 */

typedef struct {
    enum { OP_REG, OP_IMM, OP_SHIFTED_REG, OP_SHIFTED_IMM } type;
    union {
        reg_t reg;
        int32_t imm;
        struct {
            reg_t reg;
            shift_type sh_type; // "lsl", "lsr", "asr", "ror"
            uint32_t shift_amount;
        } shifted_reg;
        struct {
            int32_t imm;
            shift_type sh_type;
            uint32_t shift_amount;
        } shifted_imm;
    } value;
} operand_t;

/**
 * @brief Addressing mode types for load/store operations
 */

typedef enum {
    UNSIGNED_IMM_ZERO_OFFSET, UNSIGNED_IMM_OFFSET, PRE_IND, POST_IND, REGISTER, LITERAL
} addressing_mode_type_t;

/**
 * @brief Union representing literal values or labels in address modes
 */

typedef union {
    char *label;
    uint32_t int_directive;
} literal_t;

/**
 * @brief Load/store data transfer type
 */

typedef enum {
    LOAD, STORE
} dt_type;

/**
 * @brief Addressing mode information for memory-related instructions
 */

typedef struct {
    dt_type dt;
    addressing_mode_type_t type;
    union {
        reg_t xn;
        struct {
            reg_t xn;
            uint32_t imm;
        } unsigned_offset;
        struct {
            reg_t xn;
            int32_t simm;
        } pre_post_indexed;
        struct {
            reg_t xn;
            reg_t xm;
        } register_offset;
        literal_t literal;
    } value;
} addressing_mode_t;

/**
 * @brief The main instruction structure used in the assembler
 *
 * Represents one parsed instruction with all fields needed for
 * code generation or execution
 */

// Parsed instruction structure
typedef struct {
    instruction_type_t type;
    char mnemonic[16];           // Instruction mnemonic (e.g., "add", "ldr")
    
    // Registers (not all instructions use all of these)
    reg_t rd;               // Destination register
    reg_t rn;               // First source register
    reg_t rm;               // Second source register
    reg_t ra;               // Third register (for multiply)
    reg_t rt;               // Transfer register (for load/store)
    
    // Operands
    operand_t operand;           // Main operand (immediate, register, etc.)
    addressing_mode_t address;            // Address string for load/store
    char *branch_label;             // Branch target (label or register)
    
    // Wide move specific
    uint16_t imm16;              // 16-bit immediate for wide moves
    int shift_amount;            // Shift amount for wide moves
    
    // Directive specific
    uint32_t directive_value;    // Value for .int directive
    
    // Metadata
    uint32_t instr_address;      // Instruction address
    char *raw_line;              // original string 

    uint32_t encoded;            // encoded binary representation
} instruction_t;

/**
 * @brief Allocates and initializes a new instruction
 *
 * @return Pointer to a heap-allocated instruction_t object
 */

instruction_t* create_instruction(void);


/**
 * @brief Frees the memory allocated for an instruction
 *
 * @param instr Pointer to the instruction to free
 */

void free_instruction(instruction_t* instr);

/**
 * @brief Converts a string to a reg_t struct
 *
 * Exits the program with an error if the format is invalid
 *
 * @param regstr The string representation of a register
 * @return Corresponding reg_t structure
 */

reg_t string_to_reg_t(char *regstr);

/**
 * @brief Converts a shift mnemonic to shift_type
 *
 * @param str The shift type string
 * @return The corresponding shift_type enum value
 */

shift_type string_to_shift_type( char* str);

#endif
