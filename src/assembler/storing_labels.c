#include "storing_labels.h"

#define MAX_LINE_LEN 256

int is_instruction(const char *line) {
    if (line[0] == '\0') return 0;
    if (isalpha(line[0]) && strchr(line, ':') == NULL) return 1;
    return 0;
}

int is_int_directive(const char *line) {
    return strncmp(line, ".int", 4) == 0;
}

void fill_symbol_table(FILE *in, symbol_table table) {

    char line[MAX_LINE_LEN];
    int address = 0;

    rewind(in);
    
    while (fgets(line, sizeof(line), in) != NULL) {
        
        char *p = line;
        while(isspace(*p)) p++;

        if (*p == '\0' || *p == '#' || *p == ';') continue;

        size_t len = strlen(p);

        while(len > 0 && isspace(p[len - 1])) {
            p[len - 1] = '\0';
            len--;
        }

        if(len > 0 && p[len - 1] == ':') {

            p[len - 1] = '\0';
            symbol_table_append(table, p, address);

        } 
        else if (is_instruction(p) || is_int_directive(p)) {

            address+= 4;

        }
    }

    rewind(in);
}
