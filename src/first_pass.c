#include <string.h>
#include <stdio.h>
#include "../include/ast.h"
#include "../include/middleend.h"

int first_pass(SymbolTable *symbol_table, const char *am_file_name, FILE *am_file, DataImage *data_image)
{
    char line[81] = {0};
    int IC = 0;
    int DC = 0;
    int error_flag = 0;
    int line_number = 1;
    int i;
    ASTNode line_ast = {0};
    Symbol *symbol;

    while (fgets(line, sizeof(line), am_file))
    {
        line_ast = get_ast_node_from_line(line);

        if (strlen(line_ast.syntax_error) > 0)
        {
            fprintf(stderr, "Syntax error: %s:%d %s\n", am_file_name, line_number, line_ast.syntax_error);
            error_flag = 1;
            line_number++;
            continue;
        }

        symbol = symbol_look_up(symbol_table, line_ast.label_name);

        /*      XYZ: mov *r3, #5 (good)    or     XYZ: .data 1,2,3  (good)   or     XYZ: .string  "hello there!"  (bad)           */
        /*      add this tommorow (line_ast.type == AST_DIR && line_ast.ast.directive.dir_type == DIR_STRING)                */
        if (strlen(line_ast.label_name) > 0 && (line_ast.type == AST_INST || (line_ast.type == AST_DIR && line_ast.ast.directive.dir_type == DIR_DATA)))
        {
            if (symbol)
            {
                if (symbol->type == ENTRY)
                {
                    symbol->type = (line_ast.type == AST_INST) ? ENTRY_CODE : ENTRY_DATA;
                    symbol->address = (line_ast.type == AST_INST) ? IC : DC;
                }
                else
                {
                    fprintf(stderr, "%s:%d, redifinition of symbol: '%s'\n", am_file_name, line_number, symbol->name);
                    error_flag = 1;
                }
            }
            else
            {
                int address = (line_ast.type == AST_INST) ? IC : DC;
                int symbol_type = (line_ast.type == AST_INST) ? CODE : DATA;

                if (!add_symbol(symbol_table, line_ast.label_name, address, symbol_type))
                {
                    fprintf(stderr, "Failed to add symbol: '%s'\n", line_ast.label_name);
                    error_flag = 1;
                }

                if (line_ast.type == AST_INST)
                {
                    for (i = 0; i < 2; i++)
                    {
                        if (line_ast.ast.instruction.operand_type[i] == OPERAND_LABEL)
                        {
                            symbol = symbol_look_up(symbol_table, line_ast.ast.instruction.operands[i].label);
                            if (!symbol)
                            {
                                if (!add_symbol(symbol_table, line_ast.ast.instruction.operands[i].label, 0, ENTRY))
                                {
                                    fprintf(stderr, "Failed to add symbol: %s\n", line_ast.ast.instruction.operands[i].label);
                                    error_flag = 1;
                                }
                            }
                        }
                    }
                    /* FIX LOGIC TOMORROW */
                    IC += 1 + (line_ast.ast.instruction.operand_type[0] >= OPERAND_IMMEDIATE) + (line_ast.ast.instruction.operand_type[1] >= OPERAND_IMMEDIATE);
                }

                else
                {
                    for (i = 0; i < line_ast.ast.directive.dir_operand.data.data_count; i++)
                    {
                        add_data(data_image, line_ast.ast.directive.dir_operand.data.data[i]);
                    }
                    DC += line_ast.ast.directive.dir_operand.data.data_count;
                }
            }
        }

        /*      mov *r3, #5       */
        else if (line_ast.type == AST_INST)
        {
            for (i = 0; i < 2; i++)
            {
                if (line_ast.ast.instruction.operand_type[i] == OPERAND_LABEL)
                {
                    symbol = symbol_look_up(symbol_table, line_ast.ast.instruction.operands[i].label);
                    if (!symbol)
                    {
                        if (!add_symbol(symbol_table, line_ast.ast.instruction.operands[i].label, 0, ENTRY))
                        {
                            fprintf(stderr, "Failed to add symbol: %s\n", line_ast.ast.instruction.operands[i].label);
                            error_flag = 1;
                        }
                    }
                }
            }
            /* FIX LOGIC TOMORROW */
            IC += 1 + (line_ast.ast.instruction.operand_type[0] >= OPERAND_IMMEDIATE) + (line_ast.ast.instruction.operand_type[1] >= OPERAND_IMMEDIATE);
        }

        /*       .data 1,2,3       or    .string "hello world!"    or      .entry XYZ */
        else if (line_ast.type == AST_DIR)
        {
            if (line_ast.ast.directive.dir_type == DIR_DATA)
            {
                for (i = 0; i < line_ast.ast.directive.dir_operand.data.data_count; i++)
                {
                    add_data(data_image, line_ast.ast.directive.dir_operand.data.data[i]);
                }
                DC += line_ast.ast.directive.dir_operand.data.data_count;
            }

            else if (line_ast.ast.directive.dir_type == DIR_STRING)
            {
                /* FIX LOGIC TOMORROW */
                DC += strlen(line_ast.ast.directive.dir_operand.string);
            }

            else if (line_ast.ast.directive.dir_type == DIR_ENTRY)
            {
                if (!add_symbol(symbol_table, line_ast.label_name, 0, ENTRY))
                {
                    fprintf(stderr, "Failed to add symbol: %s\n", line_ast.label_name);
                    error_flag = 1;
                }
            }
        }
        line_number++;
    }

    for (i = 0; i < symbol_table->count; i++)
    {
        if (symbol_table->symbols[i].type == ENTRY)
        {
            fprintf(stderr, "Error: %s is an entry symbol but was not defined\n", symbol_table->symbols[i].name);
            error_flag = 1;
        }

        if (symbol_table->symbols[i].type == DATA || symbol_table->symbols[i].type == ENTRY_DATA)
        {
            symbol_table->symbols[i].address += IC;
        }
    }

    return error_flag;
}
