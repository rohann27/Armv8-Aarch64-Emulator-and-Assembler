#include "tokens.h"

#define MAX_LINE_LEN 256

token_list_t tokenise(FILE *in) {
    char line[MAX_LINE_LEN];
    token_list_t list = token_list_create();

    while(fgets(line, MAX_LINE_LEN, in)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        bool is_blank = true;
        for (size_t i = 0; i < strlen(line); i++) {
            if (!isspace(line[i])) {
            is_blank = false;
            break;
            }
        }

        if (!is_blank) {
            token_line(line, list);
            token_t newline = { .type = TOKEN_NEWLINE, .value = strdup("\n") };
            token_list_add(list, newline);
        }

    }

    token_t eof = { .type = TOKEN_EOF, .value = "\0" };
    token_list_add(list, eof);

    return list;
}

void token_line(const char *line, token_list_t list) {
    const char *ptr = line;

    while (*ptr != '\0') {

        bool saw_newline = false;
        while (isspace(*ptr)) {
            if (*ptr == '\n') {
                saw_newline = true;
            }
            ptr++;
        }
        if (saw_newline) {
            token_t newline = { .type = TOKEN_NEWLINE, .value = strdup("\\n") };
            token_list_add(list, newline);
        }

        if (*ptr == '\0') break;

        if (*ptr == '[') {
            token_t tok = {TOKEN_LBRACKET, strdup("[")};
            token_list_add(list, tok);
            ptr++;
            continue;
        }
        else if (*ptr == ']') {
            token_t tok = {TOKEN_RBRACKET, strdup("]")};
            token_list_add(list, tok);
            ptr++;
            continue;
        }
        else if (*ptr == '!') {
            token_t tok = {TOKEN_EXCLAMATION, strdup("!")};
            token_list_add(list, tok);
            ptr++;
            continue;
        }
        else if (*ptr == ',') {
            ptr++;
            continue;
        }

        const char *start = ptr;
        while (*ptr && !isspace(*ptr) && *ptr != ',' && *ptr != '[' && *ptr != ']' && *ptr != '!') {
            ptr++;
        }
        
        int len = ptr - start;
        char *val = strndup(start,len);

        token_t token;

        if (val[0] == '.') {
            token.type = TOKEN_DIRECTIVE;
        } else if (val[0] == '#') {
            token.type = TOKEN_IMMEDIATE;
            memmove(val, val + 1, strlen(val)); // remove the '#'
        } else if (is_register(val)) {
            token.type = TOKEN_REGISTER;
        } else if (is_shift(val)) {
            token.type = TOKEN_SHIFT;
        } else if (is_mnemonic(val)) {
            token.type = TOKEN_MNEMONIC;
        } else if (is_label(val)) {
            token.type = TOKEN_LABEL;
            val[strlen(val) - 1] = '\0'; // remove trailing ':'
        } else {
            token.type = TOKEN_LITERAL;
        }

        token.value = val;
        token_list_add(list, token);
    }

    // token_t newline = { .type = TOKEN_NEWLINE, .value = strdup("\\n") };
    // token_list_add(list, newline);
}

bool is_label(const char *str) {
    size_t len = strlen(str);
    return len > 0 && str[len - 1] == ':';
}

bool is_directive(const char *str) {
    return str[0] == '.';
}


bool is_register(const char *str) {
    if (!str) return false;
    // Check special registers
    if (strcmp(str, "sp") == 0 || strcmp(str, "pc") == 0 || strcmp(str, "PC") == 0)
        return true;

    if ((str[0] == 'x' || str[0] == 'w')) {
        if (strcmp(str + 1, "zr") == 0) {
            return true; // xzr or wzr
        }

        // Check if the rest is a number between 0 and 30
        char *end;
        long reg_num = strtol(str + 1, &end, 10);
        if (*end != '\0') return false; // not fully numeric
        if (reg_num >= 0 && reg_num <= 30) {
            return true;
        }
    }

    return false;
}

bool is_immediate(const char *str) {
    return str[0] == '#';
}

bool is_mnemonic(const char *str) {
    return IS_MNEMONIC(str);
}

bool is_shift(const char *val) {
    return strcmp(val, "lsl") == 0 ||
           strcmp(val, "lsr") == 0 ||
           strcmp(val, "asr") == 0 ||
           strcmp(val, "ror") == 0;
}

token_list_t token_list_create() {
    token_list_t list = malloc(sizeof(struct token_list));
    assert(list != NULL);
    list->count = 0;
    list->capacity = 256;
    list->current = 0;
    list->tokens = malloc(list->capacity * sizeof(token_t));
    assert(list != NULL);
    return list;
}

void token_list_add(token_list_t list, token_t token) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->tokens = realloc(list->tokens, list->capacity * sizeof(token_t));
        assert(list != NULL);
    }
    list->tokens[list->count++] = token;
}

void free_token_list(token_list_t list) {
    // for (int i = 0; i < list->count; i++) {
    //     free(list->tokens[i].value);
    // }
    free(list->tokens);
    free(list);
}

// Peeks at the current token; if it matches the type, advances and returns true.
// Otherwise, leaves the cursor unchanged and returns false.
bool match(token_list_t list, token_type_t type) {
    if (list->current >= list->count) return false;
    if (list->tokens[list->current].type == type) {
        advance(list);
        return true;
    }
    return false;
}

token_t *advance(token_list_t list) {
    if (list->current < list->count) return &list->tokens[list->current++];
    return NULL;
}

token_t *current_token(token_list_t list) {
    token_t token_obj = { .type = TOKEN_EOF, .value = NULL };
    return (list->current < list->count) ? &list->tokens[list->current] : &token_obj;
}

// Returns the next token without advancing the current token
token_t *peek_next(token_list_t list) {
    if (list->current + 1 < list->count) {
        return &list->tokens[list->current + 1];
    }
    return NULL; // No next token
}

token_t *expect(token_list_t list, token_type_t type, const char *msg) {
    token_t *tok = current_token(list);
    if (!tok || tok->type != type) {
        fprintf(stderr, "Error (found '%s', %s)\n",
                msg, tok ? tok->value : "EOF");
        exit(EXIT_FAILURE);
    }
    advance(list);
    return tok;
}


const char *token_type_to_string(token_type_t type) {
    switch (type) {
        case TOKEN_LABEL:         return "TOKEN_LABEL";
        case TOKEN_MNEMONIC:      return "TOKEN_MNEMONIC";
        case TOKEN_REGISTER:      return "TOKEN_REGISTER";
        case TOKEN_IMMEDIATE:     return "TOKEN_IMMEDIATE";
        case TOKEN_LITERAL:       return "TOKEN_LITERAL";
        case TOKEN_SHIFT:         return "TOKEN_SHIFT";
        case TOKEN_LBRACKET:      return "TOKEN_LBRACKET";
        case TOKEN_RBRACKET:      return "TOKEN_RBRACKET";
        case TOKEN_EXCLAMATION:   return "TOKEN_EXCLAMATION";
        case TOKEN_DIRECTIVE:     return "TOKEN_DIRECTIVE";
        case TOKEN_NEWLINE:       return "TOKEN_NEWLINE";
        case TOKEN_EOF:           return "TOKEN_EOF";
        default:                  return "UNKNOWN_TOKEN_TYPE";
    }
}
