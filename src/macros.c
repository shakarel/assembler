#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/macros.h"

MacroTable *initialize_macro_table(const int initial_capacity) {
    MacroTable *macro_table;
    macro_table = (MacroTable *)malloc(sizeof(MacroTable) * initial_capacity);
    if (!macro_table) {
        fprintf(stderr, "Error: Failed to allocate memory for MacroTable.\n");
        return NULL;
    }
    macro_table->macros = (Macro **)malloc(initial_capacity * sizeof(Macro));
    if (!macro_table->macros) {
        fprintf(stderr, "Error: Failed to allocate memory for Macro pointers.\n");
        free(macro_table);
        return NULL;
    }
    macro_table->size = 0;
    return macro_table;
}

Macro *add_macro(MacroTable *table, const char *name, const char *code) {
    Macro **new_macros;
    Macro *new_macro;

    new_macros = (Macro **)realloc(table->macros, (table->size + 1) * sizeof(Macro *));
    if (!new_macros) {
        fprintf(stderr, "Error: Failed to reallocate memory for Macro pointers.\n");
        return NULL;
    }
    table->macros = new_macros;

    new_macro = (Macro *)malloc(sizeof(Macro));
    if (!new_macro) {
        fprintf(stderr, "Error: Failed to allocate memory for Macro.\n");
        return NULL;
    }

    new_macro->name = (char *)malloc(strlen(name) + 1);
    if (!new_macro->name) {
        fprintf(stderr, "Error: Failed to allocate memory for Macro name.\n");
        free(new_macro);
        return NULL;
    }
    strcpy(new_macro->name, name);

    new_macro->code = (char *)malloc(strlen(code) + 1);
    if (!new_macro->code) {
        fprintf(stderr, "Error: Failed to allocate memory for Macro code.\n");
        free(new_macro->name);
        free(new_macro);
        return NULL;
    }
    strcpy(new_macro->code, code);

    table->macros[table->size] = new_macro;
    table->size++;
    return new_macro;
}

void free_macro_table(MacroTable *table) {
    int i;
    if (!table) {
        printf("No table to free.\n");
        return;
    }
    for (i = 0; i < table->size; ++i) {
        if (table->macros[i]) {
            free(table->macros[i]->name);
            free(table->macros[i]->code);
            free(table->macros[i]);
        }
    }
    free(table->macros);
    free(table);
}

char *get_macro_code(MacroTable *table, const char *name) {
    int i;
    for (i = 0; i < table->size; ++i) {
        if (strcmp(table->macros[i]->name, name) == 0) {
            return table->macros[i]->code;
        }
    }
    return NULL;
}
