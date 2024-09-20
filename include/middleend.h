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
    ENTRY_DATA,
    EXTERN,
    TO_BE_DEFINED
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

typedef struct InstructionImage {
    int *instructions;
    int count;
    int capacity;
} InstructionImage;

typedef struct ExternSymbolUsage {
    char name[MAX_SYMBOL_NAME_LEN];
    int address;
    SymbolType type;
} ExternSymbolUsage;

typedef struct ExternUsageTable {
    ExternSymbolUsage* usages;
    int count;
    int capacity;
} ExternUsageTable;

typedef struct {
    int IC;
    int DC;
    SymbolTable symbol_table;
    DataImage data_image;
    InstructionImage instruction_image;
    ExternUsageTable extern_usage_table;
} TranslationUnit;

void init_symbol_table(SymbolTable *table);
void free_symbol_table(SymbolTable *table);
int add_symbol(SymbolTable *table, const char *name, int address, SymbolType type);
int symbol_exists(SymbolTable *table, const char *name);
Symbol *symbol_look_up(SymbolTable *table, const char *name);

void init_data_image(DataImage *image);
void free_data_image(DataImage *image);
void add_data(DataImage *image, int data);

void init_instruction_image(InstructionImage *image);
void free_instruction_image(InstructionImage *image);
void add_instruction(InstructionImage *image, int instruction);

void init_extern_usage_table(ExternUsageTable *table);
void free_extern_usage_table(ExternUsageTable *table);
void add_extern_usage(ExternUsageTable *table, const char *name, int address, SymbolType type);

void init_translation_unit(TranslationUnit *unit);
void free_translation_unit(TranslationUnit *unit);

int first_pass(TranslationUnit *unit, const char *am_file_name, FILE *am_file);

void process_directive(SymbolTable *symbol_table, ASTNode line_ast, DataImage *data_image, int *DC);
void process_instruction(SymbolTable *symbol_table, ASTNode line_ast, int *IC, int *error_flag, ExternUsageTable *extern_usage_table);

void check_entry_symbols(SymbolTable *symbol_table, int *error_flag);
void check_to_be_defined_symbols(SymbolTable *symbol_table, int *error_flag);
int is_valid_symbol_label(ASTNode ast_line);
void fix_extern_usage_table(ExternUsageTable *extern_usage_table, char *name);


void print_symbol_table(SymbolTable *symbol_table);
void print_data_image(DataImage *data_image);
void print_instruction_image(InstructionImage *instruction_image);

int second_pass(TranslationUnit *unit, const char *am_file_name, FILE *am_file);
#endif
