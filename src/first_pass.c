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

        /*      XYZ: mov *r3, #5 (good)    or     XYZ: .data 1,2,3  (good)   or     XYZ: .string  "hello there!"  (good) */
        if (is_valid_symbol_label(line_ast))
        {
            if (symbol)
            {
                if (symbol->type == ENTRY)
                {
                    symbol->type = (line_ast.type == AST_INST) ? ENTRY_CODE : ENTRY_DATA;
                    symbol->address = DC + IC + 100;

                    if (line_ast.type == AST_DIR)
                        process_directive(symbol_table, line_ast, data_image, &DC);
                    else if (line_ast.type == AST_INST)
                        process_instruction(symbol_table, line_ast, &IC, &error_flag);
                }
                else
                {
                    fprintf(stderr, "%s:%d, redifinition of symbol: '%s'\n", am_file_name, line_number, symbol->name);
                    error_flag = 1;
                }
            }
            else
            {
                int address = DC + IC + 100;
                int symbol_type = (line_ast.type == AST_INST) ? CODE : DATA;

                if (!add_symbol(symbol_table, line_ast.label_name, address, symbol_type))
                {
                    fprintf(stderr, "Failed to add symbol: '%s'\n", line_ast.label_name);
                    error_flag = 1;
                }

                line_ast.type == AST_INST ? process_instruction(symbol_table, line_ast, &IC, &error_flag) : 
                                            process_directive(symbol_table, line_ast, data_image, &DC);
            }
        }

        /*      mov *r3, #5       */
        else if (line_ast.type == AST_INST)
        {
            process_instruction(symbol_table, line_ast, &IC, &error_flag);
        }

        /*       .data 1,2,3       or    .string "hello world!"    or      .entry XYZ */
        else if (line_ast.type == AST_DIR)
        {
            process_directive(symbol_table, line_ast, data_image, &DC);
            
            if (line_ast.ast.directive.dir_type == DIR_ENTRY)
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


    check_entry_symbols(symbol_table, &error_flag);
    return error_flag;
}
