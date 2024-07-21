#ifndef MACROS_H
#define MACROS_H

#define MAX_MACROS 100
#define MAX_CODE_LENGTH 1024
#define MAX_LINE_LENGTH 256

typedef struct Macro {
    char name[MAX_LINE_LENGTH];
    char code[MAX_CODE_LENGTH];
} Macro;

extern Macro macro_table[MAX_MACROS];
extern int macro_count;

void add_macro(char *name, char *code);
char* get_macro_code(char *name);

#endif
