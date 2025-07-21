#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

/**
 * @brief Checks if the instruction is a data-processing mnemonic
 */

#define IS_DP_MNEMONIC(instr) ( \
    strcmp((instr), "add")  == 0 || strcmp((instr), "adds") == 0 || \
    strcmp((instr), "sub")  == 0 || strcmp((instr), "subs") == 0 || \
    strcmp((instr), "cmp")  == 0 || strcmp((instr), "cmn")  == 0 || \
    strcmp((instr), "neg")  == 0 || strcmp((instr), "negs") == 0 || \
    strcmp((instr), "and")  == 0 || strcmp((instr), "ands") == 0 || \
    strcmp((instr), "bic")  == 0 || strcmp((instr), "bics") == 0 || \
    strcmp((instr), "eor")  == 0 || strcmp((instr), "eon")  == 0 || \
    strcmp((instr), "orr")  == 0 || strcmp((instr), "orn")  == 0 || \
    strcmp((instr), "tst")  == 0 || strcmp((instr), "mvn")  == 0 || \
    strcmp((instr), "mov")  == 0 || strcmp((instr), "movn") == 0 || \
    strcmp((instr), "movk") == 0 || strcmp((instr), "movz") == 0 || \
    strcmp((instr), "madd") == 0 || strcmp((instr), "msub") == 0 || \
    strcmp((instr), "mul")  == 0 || strcmp((instr), "mneg") == 0 \
)

/**
 * @brief Checks if the instruction is a branching mnemonic
 */

#define IS_BRANCHING_MNEMONIC(instr) ( \
    strcmp((instr), "b")  == 0 || strcmp((instr), "br") == 0 || \
    strcmp((instr), "b.eq")  == 0 || strcmp((instr), "b.ne") == 0 || \
    strcmp((instr), "b.ge")  == 0 || strcmp((instr), "b.lt")  == 0 || \
    strcmp((instr), "b.gt")  == 0 || strcmp((instr), "b.le") == 0 || \
    strcmp((instr), "b.al")  == 0 \
)

/**
 * @brief Checks if the instruction is a load/store mnemonic
 */

#define IS_LOAD_STORE_MNEMONIC(instr) ( \
    strcmp((instr), "str")  == 0 || strcmp((instr), "ldr") == 0 \
)

/**
 * @brief Checks if the instruction is any recognized mnemonic
 */

#define IS_MNEMONIC(instr) ( \
    IS_BRANCHING_MNEMONIC(instr) || IS_DP_MNEMONIC(instr) || IS_LOAD_STORE_MNEMONIC(instr) \
)

/**
 * @enum token_type_t
 * @brief Types of tokens that can be produced by the tokenizer
 */

typedef enum {
    TOKEN_LABEL,           // "loop:", "main:"
    TOKEN_MNEMONIC,        // "add", "ldr", "b"
    TOKEN_REGISTER,        // "x0", "w15", "sp"
    TOKEN_IMMEDIATE,       // "#42", "#0x1000"
    TOKEN_LITERAL,         // "label", "variable"
    TOKEN_SHIFT,           // "lsl", "lsr", "asr", or "ror"
    TOKEN_LBRACKET,        // "["
    TOKEN_RBRACKET,        // "]"
    TOKEN_EXCLAMATION,     // "!"
    TOKEN_DIRECTIVE,       // ".int"
    TOKEN_NEWLINE,         // "\n"
    TOKEN_EOF
} token_type_t;

/**
 * @struct token_t
 * @brief Represents a single token with its type and string value
 */

typedef struct {
    token_type_t type;
    char *value;
} token_t;

/**
 * @struct token_list
 * @brief Dynamic array structure holding a list of tokens and parsing state
 */

struct token_list {
    token_t *tokens;
    int count;
    int capacity;
    int current;  // Current token index for parsing
};

typedef struct token_list *token_list_t;

/**
 * @brief Tokenizes the input assembly file stream into a list of tokens
 * 
 * @param in FILE pointer to the assembly source input stream
 * @return token_list_t Dynamically allocated list of tokens
 */

token_list_t tokenise(FILE *in);

/**
 * @brief Tokenizes a single line of assembly source and adds tokens to the list
 * 
 * @param line Null terminated string of the line to tokenize
 * @param list Token list to add tokens to
 */

void token_line(const char *line, token_list_t list);

/**
 * @brief Creates a new, empty token list with initial capacity
 * 
 * @return token_list_t Newly allocated token list
 */

token_list_t token_list_create(void);

/**
 * @brief Frees all memory used by the token list, including token strings
 * 
 * @param list Token list to free
 */

void free_token_list(token_list_t list);

/**
 * @brief Adds a token to the token list, expanding capacity as needed
 * 
 * @param list Token list to add to
 * @param token Token to add
 */

void token_list_add(token_list_t list, token_t token);

/**
 * @brief Checks if the current token matches the specified type and advances if true
 * 
 * @param list Token list parser state
 * @param type Token type to match
 * @return true if matched and advanced, false otherwise
 */

bool match(token_list_t list, token_type_t type);

/**
 * @brief Advances the current token index and returns the previous current token
 * 
 * @param list Token list parser state
 * @return token_t* Pointer to the advanced token or NULL if at end
 */

token_t *advance(token_list_t list);

/**
 * @brief Returns the current token without advancing
 * 
 * @param list Token list parser state
 * @return token_t* Pointer to the current token, or a dummy EOF token if at end
 */

token_t *peek_next(token_list_t list);

/**
 * @brief Returns the next token without advancing
 * 
 * @param list Token list parser state
 * @return token_t* Pointer to the next token or NULL if no next token
 */

token_t *current_token(token_list_t list);

/**
 * @brief Expects the current token to be of a specific type, exits with error if not
 * Advances the token pointer if matched
 * 
 * @param list Token list parser state
 * @param type Expected token type
 * @param msg Error message to print if expectation fails
 * @return token_t* Pointer to the matched token
 */

token_t *expect(token_list_t list, token_type_t type, const char *msg);

/**
 * @brief Returns true if the string matches the pattern for a label token
 * 
 * @param str Input string
 * @return true if string ends with ':'
 */

bool is_label(const char *str);

/**
 * @brief Returns true if the string represents a directive token
 * 
 * @param str Input string
 * @return true if string starts with '.'
 */

bool is_directive(const char *str);

/**
 * @brief Returns true if the string matches the pattern of a register token
 * 
 * @param str Input string
 * @return true if the string is a valid register name
 */

bool is_register(const char *str);


/**
 * @brief Returns true if the string is a recognized shift operation
 * 
 * @param val Input string
 * @return true if val is one of "lsl", "lsr", "asr", or "ror"
 */

bool is_shift(const char *val);

/**
 * @brief Returns true if the string is an immediate value
 * 
 * @param str Input string
 * @return true if string starts with '#'
 */

bool is_immediate(const char *str);


/**
 * @brief Returns true if the string is a recognized mnemonic
 * 
 * @param str Input string
 * @return true if string is a valid mnemonic
 */

bool is_mnemonic(const char *str);

/**
 * @brief Returns a string representation of the token type enum
 * 
 * @param type Token type enum
 * @return const char* String name of the token type
 */

const char *token_type_to_string(token_type_t type);

#endif
