#include <string.h>
#include "../include/middleend.h"

void init_symbol_table(SymbolTable *table)
{
    table->capacity = INITIAL_SYMBOL_CAPACITY;
    table->count = 0;
    table->symbols = (Symbol *)malloc(table->capacity * sizeof(Symbol));
    if (table->symbols == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void free_symbol_table(SymbolTable *table)
{
    free(table->symbols);
}

int add_symbol(SymbolTable *table, const char *name, int address, SymbolType type)
{
    Symbol symbol = {0};

    if (table->count == table->capacity)
    {
        table->capacity *= 2;
        table->symbols = (Symbol *)realloc(table->symbols, table->capacity * sizeof(Symbol));
        if (table->symbols == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }

    if (symbol_exists(table, name))
    {
        return 0;
    }

    strncpy(symbol.name, name, MAX_SYMBOL_NAME_LEN);
    symbol.address = address;
    symbol.type = type;

    table->symbols[table->count++] = symbol;
    return 1;
}

int symbol_exists(SymbolTable *table, const char *name)
{
    int i;
    for (i = 0; i < table->count; i++)
    {
        if (strcmp(table->symbols[i].name, name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

Symbol *symbol_look_up(SymbolTable *table, const char *name)
{
    int i;
    for (i = 0; i < table->count; i++)
    {
        if (strcmp(table->symbols[i].name, name) == 0)
        {
            return &table->symbols[i];
        }
    }
    return NULL;
}

void init_data_image(DataImage *image)
{
    image->capacity = INITIAL_SYMBOL_CAPACITY;
    image->count = 0;
    image->data = (int *)malloc(image->capacity * sizeof(int));
    if (image->data == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void free_data_image(DataImage *image)
{
    free(image->data);
}

void add_data(DataImage *image, int data)
{
    if (image->count == image->capacity)
    {
        image->capacity *= 2;
        image->data = (int *)realloc(image->data, image->capacity * sizeof(int));
        if (image->data == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
    image->data[image->count++] = data;
}

void init_instruction_image(InstructionImage *image)
{
    image->capacity = INITIAL_SYMBOL_CAPACITY;
    image->count = 0;
    image->instructions = (int *)malloc(image->capacity * sizeof(int));
    if (image->instructions == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void free_instruction_image(InstructionImage *image)
{
    free(image->instructions);
}

void add_instruction(InstructionImage *image, int instruction)
{
    if (image->count == image->capacity)
    {
        image->capacity *= 2;
        image->instructions = (int *)realloc(image->instructions, image->capacity * sizeof(int));
        if (image->instructions == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
    image->instructions[image->count++] = instruction;
}

void init_extern_usage_table(ExternUsageTable *table)
{
    table->capacity = INITIAL_SYMBOL_CAPACITY;
    table->count = 0;
    table->usages = (ExternSymbolUsage *)malloc(table->capacity * sizeof(ExternSymbolUsage));
    if (table->usages == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
}

void free_extern_usage_table(ExternUsageTable *table)
{
    free(table->usages);
}

void add_extern_usage(ExternUsageTable *table, const char *name, int address, SymbolType type)
{
    if (table->count == table->capacity)
    {
        table->capacity *= 2;
        table->usages = realloc(table->usages, table->capacity * sizeof(ExternSymbolUsage));
    }
    strncpy(table->usages[table->count].name, name, MAX_SYMBOL_NAME_LEN);
    table->usages[table->count].address = address;
    table->usages[table->count].type = type;
    table->count++;
}

void init_translation_unit(TranslationUnit *unit)
{
    init_symbol_table(&unit->symbol_table);
    init_data_image(&unit->data_image);
    init_instruction_image(&unit->instruction_image);
    init_extern_usage_table(&unit->extern_usage_table);
}

void free_translation_unit(TranslationUnit *unit)
{
    free_symbol_table(&unit->symbol_table);
    free_data_image(&unit->data_image);
    free_instruction_image(&unit->instruction_image);
    free_extern_usage_table(&unit->extern_usage_table);
}

void process_directive(SymbolTable *symbol_table, ASTNode line_ast, DataImage *data_image, int *DC)
{
    int i;

    if (line_ast.ast.directive.dir_type == DIR_DATA)
    {
        for (i = 0; i < line_ast.ast.directive.dir_operand.data.data_count; i++)
        {
            add_data(data_image, line_ast.ast.directive.dir_operand.data.data[i]);
        }
        *DC += line_ast.ast.directive.dir_operand.data.data_count;
    }
    else if (line_ast.ast.directive.dir_type == DIR_STRING)
    {
        for (i = 0; i < strlen(line_ast.ast.directive.dir_operand.string); i++)
        {
            add_data(data_image, line_ast.ast.directive.dir_operand.string[i]);
        }
        *DC += strlen(line_ast.ast.directive.dir_operand.string) + 1;
        add_data(data_image, '\0');
    }
}

void handle_symbol(SymbolTable *symbol_table, const char *label, int IC, int *error_flag, ExternUsageTable *extern_usage_table)
{
    Symbol *symbol = symbol_look_up(symbol_table, label);

    if (symbol && symbol->type == TO_BE_DEFINED)
    {
        add_extern_usage(extern_usage_table, symbol->name, IC + 100, TO_BE_DEFINED);
    }
    else if (symbol && symbol->type == EXTERN)
    {
        add_extern_usage(extern_usage_table, symbol->name, IC + 100, EXTERN);
    }
    else if (!symbol)
    {
        if (!add_symbol(symbol_table, label, 0, TO_BE_DEFINED))
        {
            fprintf(stderr, "Failed to add symbol: %s\n", label);
            *error_flag = 1;
        }
        else
        {
            add_extern_usage(extern_usage_table, label, IC + 100, TO_BE_DEFINED);
        }
    }
}

void process_instruction(SymbolTable *symbol_table, ASTNode line_ast, int *IC, int *error_flag, ExternUsageTable *extern_usage_table)
{
    int i;

    *IC += 1;

    if (line_ast.ast.instruction.operand_count == 2 &&
        (line_ast.ast.instruction.operand_type[0] == OPERAND_INDIRECT_REGISTER || line_ast.ast.instruction.operand_type[0] == OPERAND_IMMEDIATE_REGISTER) &&
        (line_ast.ast.instruction.operand_type[1] == OPERAND_INDIRECT_REGISTER || line_ast.ast.instruction.operand_type[1] == OPERAND_IMMEDIATE_REGISTER))
    {
        *IC += 1;
        return;
    }

    if (line_ast.ast.instruction.operand_count == 1 && line_ast.ast.instruction.operand_type[1] == OPERAND_LABEL)
    {
        handle_symbol(symbol_table, line_ast.ast.instruction.operands[1].label, *IC, error_flag, extern_usage_table);
        *IC += 1;
        return;
    }

    for (i = 0; i < line_ast.ast.instruction.operand_count; i++)
    {
        if (line_ast.ast.instruction.operand_type[i] == OPERAND_LABEL)
        {
            handle_symbol(symbol_table, line_ast.ast.instruction.operands[i].label, *IC, error_flag, extern_usage_table);
        }
        *IC += 1;
    }
}

void check_entry_symbols(SymbolTable *symbol_table, int *error_flag)
{
    int i;
    for (i = 0; i < symbol_table->count; i++)
    {
        if (symbol_table->symbols[i].type == ENTRY)
        {
            fprintf(stderr, "Error: %s is an entry symbol but was not defined\n", symbol_table->symbols[i].name);
            *error_flag = 1;
        }
    }
}

void check_to_be_defined_symbols(SymbolTable *symbol_table, int *error_flag)
{
    int i;
    for (i = 0; i < symbol_table->count; i++)
    {
        if (symbol_table->symbols[i].type == TO_BE_DEFINED)
        {
            fprintf(stderr, "Error: %s is used but was not defined\n", symbol_table->symbols[i].name);
            *error_flag = 1;
        }
    }
}

int is_valid_symbol_label(ASTNode ast_line)
{
    return (strlen(ast_line.label_name) > 0 &&
            (ast_line.type == AST_INST ||
             (ast_line.type == AST_DIR && ast_line.ast.directive.dir_type == DIR_DATA) ||
             (ast_line.type == AST_DIR && ast_line.ast.directive.dir_type == DIR_STRING)));
}

void fix_extern_usage_table(ExternUsageTable *extern_usage_table, char *name)
{
    int i;
    if (!extern_usage_table || !name)
        return;

    for (i = 0; i < extern_usage_table->count; i++)
    {
        ExternSymbolUsage *usage = &extern_usage_table->usages[i];

        if (strcmp(usage->name, name) == 0)
        {
            usage->type = EXTERN;
        }
    }
}
