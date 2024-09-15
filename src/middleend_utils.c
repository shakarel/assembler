#include <string.h>
#include "../include/middleend.h"

void init_symbol_table(SymbolTable* table) {
    table->capacity = INITIAL_SYMBOL_CAPACITY;
    table->count = 0;
    table->symbols = (Symbol*)malloc(table->capacity * sizeof(Symbol));
    if (table->symbols == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void free_symbol_table(SymbolTable* table) {
    free(table->symbols);
}

int add_symbol(SymbolTable *table, const char *name, int address, SymbolType type) {
    Symbol symbol = {0};

    if (table->count == table->capacity) {
        table->capacity *= 2;
        table->symbols = (Symbol*)realloc(table->symbols, table->capacity * sizeof(Symbol));
        if (table->symbols == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }

    if (symbol_exists(table, name)) {
        return 0;
    }

    strncpy(symbol.name, name, MAX_SYMBOL_NAME_LEN);
    symbol.address = address;
    symbol.type = type;

    table->symbols[table->count++] = symbol;
    return 1;
}

int symbol_exists(SymbolTable* table, const char* name) {
    int i;
    for (i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

Symbol *symbol_look_up(SymbolTable* table, const char* name) {
    int i;
    for (i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return &table->symbols[i];
        }
    }
    return NULL;
}
