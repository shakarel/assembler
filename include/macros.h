#ifndef MACROS_H
#define MACROS_H

#define MAX_MACROS 100
#define MAX_CODE_LENGTH 1024
#define MAX_LINE_LENGTH 80
#define MAX_MACRO_NAME_LENGTH 20
#define MACRO_START "macr"
#define MACRO_END "endmacr"

typedef struct Macro {
    char name[MAX_MACRO_NAME_LENGTH];
    char code[MAX_CODE_LENGTH];
} Macro;

extern Macro macro_table[MAX_MACROS];
extern int macro_count;

void add_macro(const char *name, const char *code);
char* get_macro_code(const char *name);

#endif
