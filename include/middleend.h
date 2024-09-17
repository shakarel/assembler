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

typedef struct DataImage {
    int *data;
    int count;
    int capacity;
} DataImage;

void init_symbol_table(SymbolTable *table);
void free_symbol_table(SymbolTable *table);
int add_symbol(SymbolTable *table, const char *name, int address, SymbolType type);
int symbol_exists(SymbolTable *table, const char *name);
Symbol *symbol_look_up(SymbolTable *table, const char *name);

void init_data_image(DataImage *image);
void free_data_image(DataImage *image);
void add_data(DataImage *image, int data);

int first_pass(SymbolTable *symbol_table, const char *am_file_name, FILE *am_file, DataImage *data_image);

#endif