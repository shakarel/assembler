#ifndef MIDDLEEND_H
#define MIDDLEEND_H

#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_SYMBOL_CAPACITY 10
#define MAX_SYMBOL_NAME_LEN 32

typedef enum {
    ENTRY,
    CODE,
    DATA,
    ENTRY_CODE,
    ENTRY_DATA
} SymbolType;

typedef struct Symbol {
    char name[MAX_SYMBOL_NAME_LEN];
    int address;
    SymbolType type;
} Symbol;

typedef struct SymbolTable {
    Symbol* symbols;
    int count;
    int capacity;
} SymbolTable;

void init_symbol_table(SymbolTable *table);
void free_symbol_table(SymbolTable *table);
int add_symbol(SymbolTable *table, const char *name, int address, SymbolType type);
int symbol_exists(SymbolTable *table, const char *name);
Symbol *symbol_look_up(SymbolTable *table, const char *name);

#endif
