#ifndef MACROS_H
#define MACROS_H

typedef struct Macro {
    char *name;
    char *code;
} Macro;

typedef struct MacroTable {
    Macro **macros;
    int size;
} MacroTable;


MacroTable *initialize_macro_table(int initial_capacity);
Macro *add_macro(MacroTable *table, const char *name, const char *code);
void free_macro_table(MacroTable *table);
char *get_macro_code(MacroTable *table, const char *name);

#endif
