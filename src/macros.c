#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/macros.h"

Macro macro_table[MAX_MACROS];
int macro_count = 0;
int i;

void add_macro(const char* name, const char* code) {
    if (macro_count >= MAX_MACROS) {
        fprintf(stderr, "Error: Macro table is full.\n");
        exit(1);
    }

    strcpy(macro_table[macro_count].name, name);
    strcpy(macro_table[macro_count].code, code);

    macro_count++;
}

char* get_macro_code(const char* name) {
    for (i = 0; i < macro_count; i++) {
        if (strcmp(name, macro_table[i].name) == 0) {
            return macro_table[i].code;
        }
    }

    return NULL;
}
