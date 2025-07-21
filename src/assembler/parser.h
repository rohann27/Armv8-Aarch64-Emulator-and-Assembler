#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <string.h>
#include "instruction_representation.h"
#include "assemble_utils.h"
#include "symbol_table.h"
#include "tokens.h"

#define PC_INC 0x4

#define INITIAL_INSTRUCTION_CAPACITY 256

/**
 * @def IS_DP_MULTIPLY(opc)
 * @brief Check if the opcode is a multiply type data processing instruction
 */

#define IS_DP_MULTIPLY(opc) ( \
    strcmp((opc), "madd")  == 0 || strcmp((opc), "msub") == 0 \
)

/**
 * @def IS_DP_TWO_OPERAND(opc)
 * @brief Check if the opcode is a two operand data processing instruction
 */

#define IS_DP_TWO_OPERAND(opc) ( \
    strcmp((opc), "add")  == 0 || strcmp((opc), "adds") == 0 || \
    strcmp((opc), "sub")  == 0 || strcmp((opc), "subs") == 0 || \
    strcmp((opc), "and")  == 0 || strcmp((opc), "ands") == 0 || \
    strcmp((opc), "bic")  == 0 || strcmp((opc), "bics") == 0 || \
    strcmp((opc), "eor")  == 0 || strcmp((opc), "eon") == 0 || \
    strcmp((opc), "orr")  == 0 || strcmp((opc), "mul") == 0 || \
    strcmp((opc), "orn")  == 0 || strcmp((opc), "mneg")  == 0 \
)

/**
 * @def IS_DP_ONE_OPERAND_W_DEST(opc)
 * @brief Check if the opcode is a one operand data processing instruction with a destination register
 */

#define IS_DP_ONE_OPERAND_W_DEST(opc) ( \
    strcmp((opc), "neg")  == 0 || strcmp((opc), "negs") == 0 || \
    strcmp((opc), "mvn")  == 0 || strcmp((opc), "mov") == 0 || \
    strcmp((opc), "movz")  == 0 || strcmp((opc), "movk") == 0 || \
    strcmp((opc), "movn")  == 0 \
)


/**
 * @def IS_DP_TWO_OPERAND_WO_DEST(opc)
 * @brief Check if the opcode is a two operand data processing instruction without a destination register
 */

#define IS_DP_TWO_OPERAND_WO_DEST(opc) ( \
    strcmp((opc), "cmp")  == 0 || strcmp((opc), "cmn") == 0 || \
    strcmp((opc), "tst")  == 0 \
)


/**
 * @def IS_OPC_ALIAS(opc)
 * @brief Check if the opcode is an alias instruction
 */

#define IS_OPC_ALIAS(opc) ( \
    strcmp((opc), "cmp")  == 0 || strcmp((opc), "cmn") == 0 || \
    strcmp((opc), "neg")  == 0 || strcmp((opc), "negs") == 0 || \
    strcmp((opc), "tst")  == 0 || strcmp((opc), "mvn") == 0 || \
    strcmp((opc), "mov")  == 0 || strcmp((opc), "mul") == 0 || \
    strcmp((opc), "mneg")  == 0 \
)

/**
 * @def IS_WIDE_MOVE(opc)
 * @brief Check if the opcode is a wide move instruction
 */

#define IS_WIDE_MOVE(opc) ( \
    strcmp((opc), "movz")  == 0 || strcmp((opc), "movk") == 0 || \
    strcmp((opc), "movn")  == 0 \
)


/**
 * @struct parser_state_t
 * @brief Holds the state of the parser during assembly instruction parsing
 *
 * @var tokens List of tokens being parsed
 * @var instructions Dynamic array of pointers to parsed instructions
 * @var instruction_count Number of instructions parsed so far
 * @var instruction_capacity Current capacity of the instructions array
 * @var pc Program counter (address) for the current instruction
 * @var current_line Current line number in the input source file
 */

typedef struct {
    token_list_t tokens; // list of tokens
    instruction_t **instructions; // instruction array
    int instruction_count; 
    int instruction_capacity;
    uint32_t pc;
    int current_line;
} parser_state_t;

/**
 * @brief Creates and initializes a new parser state
 * @return Pointer to the newly created parser_state_t
 */

parser_state_t *create_parser_state(void);

/**
 * @brief Adds an instruction to the parser state's instruction array
 * @param state Pointer to the parser state
 * @param instr Pointer to the instruction to add
 */

void add_instruction_to_parser_state(parser_state_t *, instruction_t *);

/**
 * @brief Frees all memory associated with the parser state
 * @param state Pointer to the parser state to free
 */

void free_parser_state(parser_state_t *);

/**
 * @brief Parses all instructions from the current tokens in the parser state
 * @param state Pointer to the parser state
 */

void parse_instructions(parser_state_t *);

/**
 * @brief Parses instructions from a given input FILE stream
 * @param state Pointer to the parser state
 * @param input FILE pointer to the input stream
 */

void parse(parser_state_t *, FILE *);

/**
 * @brief Parses a data processing instruction from tokens
 * @param instr Pointer to the instruction to fill
 * @param tokens List of tokens representing the instruction
 * @param current_token Pointer to the current token being parsed
 */

void parse_dp(instruction_t *, token_list_t, token_t *);

/**
 * @brief Parse a branch instruction from tokens
 * @param instr Pointer to the instruction to fill
 * @param tokens List of tokens representing the instruction
 * @param current_token Pointer to the current token being parsed
 */

void parse_branch(instruction_t *, token_list_t, token_t *);

/**
 * @brief Parses a load store instruction from tokens
 * @param instr Pointer to the instruction to fill
 * @param tokens List of tokens representing the instruction
 * @param current_token Pointer to the current token being parsed
 */

void parse_load_store(instruction_t *, token_list_t, token_t *);

/**
 * @brief Parses an operand from a list of tokens
 * @param tokens List of tokens
 * @param instr Pointer to the current instruction
 * @return Parsed operand_t structure
 */

operand_t parse_operand(token_list_t, instruction_t *);

/**
 * @brief Parses the addressing mode from tokens
 * @param tokens List of tokens
 * @param dt Data type used for the addressing mode
 * @return Parsed addressing_mode_t value
 */

addressing_mode_t parse_addressing_mode(token_list_t, dt_type);


#endif
