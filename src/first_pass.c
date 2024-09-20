#include <string.h>
#include <stdio.h>
#include "../include/ast.h"
#include "../include/middleend.h"

int first_pass(TranslationUnit *unit, const char *am_file_name, FILE *am_file)
{
    char line[81] = {0};
    int error_flag = 0;
    int line_number = 1;
    ASTNode line_ast = {0};
    Symbol *symbol;
    unit->IC = 0;
    unit->DC = 0;

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

        symbol = symbol_look_up(&unit->symbol_table, line_ast.label_name);

        /*      XYZ: mov *r3, #5 (good)    or     XYZ: .data 1,2,3  (good)   or     XYZ: .string  "hello there!"  (good) */
        if (is_valid_symbol_label(line_ast))
        {
            if (symbol)
            {   
                if (symbol->type == EXTERN)
                {
                    fprintf(stderr, "%s:%d, symbol '%s' is defined as extern\n", am_file_name, line_number, symbol->name);
                    error_flag = 1;
                }
                
                else if (symbol->type == ENTRY)
                {
                    symbol->type = (line_ast.type == AST_INST) ? ENTRY_CODE : ENTRY_DATA;
                    symbol->address = unit->DC + unit->IC + 100;

                    if (line_ast.type == AST_DIR)
                        process_directive(&unit->symbol_table, line_ast, &unit->data_image, &unit->DC);
                    else if (line_ast.type == AST_INST)
                        process_instruction(&unit->symbol_table, line_ast, &unit->IC, &error_flag);
                }

                else if (symbol->type == TO_BE_DEFINED) {
                    symbol->type = (line_ast.type == AST_INST) ? CODE : DATA;
                    symbol->address = unit->DC + unit->IC + 100;

                    if (line_ast.type == AST_DIR)
                        process_directive(&unit->symbol_table, line_ast, &unit->data_image, &unit->DC);
                    else if (line_ast.type == AST_INST)
                        process_instruction(&unit->symbol_table, line_ast, &unit->IC, &error_flag);
                }
                
                else
                {
                    fprintf(stderr, "%s:%d, redefinition of symbol: '%s'\n", am_file_name, line_number, symbol->name);
                    error_flag = 1;
                }
            }
            else
            {
                int address = unit->DC + unit->IC + 100;
                int symbol_type = (line_ast.type == AST_INST) ? CODE : DATA;

                if (!add_symbol(&unit->symbol_table, line_ast.label_name, address, symbol_type))
                {
                    fprintf(stderr, "Failed to add symbol: '%s'\n", line_ast.label_name);
                    error_flag = 1;
                }

                line_ast.type == AST_INST
                    ? process_instruction(&unit->symbol_table, line_ast, &unit->IC, &error_flag)
                    : process_directive(&unit->symbol_table, line_ast, &unit->data_image, &unit->DC);
            }
        }
        /*      mov *r3, #5       */
        else if (line_ast.type == AST_INST)
        {
            process_instruction(&unit->symbol_table, line_ast, &unit->IC, &error_flag);
        }
        /*       .data 1,2,3       or    .string "hello world!"    or      .entry XYZ */
        else if (line_ast.type == AST_DIR)
        {
            process_directive(&unit->symbol_table, line_ast, &unit->data_image, &unit->DC);

            if (line_ast.ast.directive.dir_type == DIR_ENTRY)
            {
                symbol = symbol_look_up(&unit->symbol_table, line_ast.label_name);
                if (symbol && symbol->type == TO_BE_DEFINED)
                    symbol->type = ENTRY;

                else if (symbol && (symbol->type == CODE || symbol->type == DATA))
                    symbol->type = symbol->type == CODE ? ENTRY_CODE : ENTRY_DATA;

                else if (!add_symbol(&unit->symbol_table, line_ast.label_name, 0, ENTRY))
                {
                    fprintf(stderr, "Failed to add symbol: %s\n", line_ast.label_name);
                    error_flag = 1;
                }
            }
            else if (line_ast.ast.directive.dir_type == DIR_EXTERN)
            {
                symbol = symbol_look_up(&unit->symbol_table, line_ast.label_name);
                if (symbol && symbol->type == TO_BE_DEFINED)
                    symbol->type = EXTERN;

                
                else if (!add_symbol(&unit->symbol_table, line_ast.label_name, 0, EXTERN))
                {
                    fprintf(stderr, "Failed to add symbol: %s\n", line_ast.label_name);
                    error_flag = 1;
                }
            }
        }

        line_number++;
    }

    check_entry_symbols(&unit->symbol_table, &error_flag);
    check_to_be_defined_symbols(&unit->symbol_table, &error_flag);
    return error_flag;
}
