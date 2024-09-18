#include "middleend.h"
#include "ast.h"

int second_pass(SymbolTable symbol_talbe, const char *am_file_name, FILE *am_file)
{
    char line[81] = {0};
    int error_flag = 0;
    int line_number = 1;
    ASTNode line_ast = {0};
    Symbol *symbol;
    int i;

    while (fgets(line, sizeof(line), am_file))
    {
        line_ast = get_ast_node_from_line(line);

        
    }
}