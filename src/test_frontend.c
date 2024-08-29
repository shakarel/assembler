#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/frontend.h"
#include "../include/ast.h"

/* Helper function to print the contents of an ASTNode */
void print_ast_node(ASTNode *ast) {
    if (strlen(ast->syntax_error) > 0) {
        printf("Syntax Error: %s\n", ast->syntax_error);
        return;
    }

    if (strlen(ast->label_name) > 0) {
        printf("Label: %s\n", ast->label_name);
    }

    if (ast->type == AST_DIR) {
        printf("Directive Type: ");
        switch (ast->ast.directive.dir_type) {
            case DIR_EXTERN:
                printf(".extern\n");
                break;
            case DIR_ENTRY:
                printf(".entry\n");
                break;
            case DIR_STRING:
                printf(".string \"%s\"\n", ast->ast.directive.dir_operand.string);
                break;
            case DIR_DATA:
                printf(".data ");
                for (int i = 0; i < ast->ast.directive.dir_operand.data.data_count; i++) {
                    printf("%d ", ast->ast.directive.dir_operand.data.data[i]);
                }
                printf("\n");
                break;
        }
    } else if (ast->type == AST_INST) {
        printf("Instruction\n");
        for (int i = 0; i < 2; i++) {
            if (ast->ast.instruction.operand_type[i] == OPERAND_IMMEDIATE) {
                printf("Operand %d: Immediate #%d\n", i + 1, ast->ast.instruction.operands[i].immediate);
            } else if (ast->ast.instruction.operand_type[i] == OPERAND_LABEL) {
                printf("Operand %d: Label %s\n", i + 1, ast->ast.instruction.operands[i].label);
            } else if (ast->ast.instruction.operand_type[i] == OPERAND_INDIRECT_REGISTER) {
                printf("Operand %d: Indirect Register *r%d\n", i + 1, ast->ast.instruction.operands[i].register_number);
            } else if (ast->ast.instruction.operand_type[i] == OPERAND_IMMEDIATE_REGISTER) {
                printf("Operand %d: Immediate Register r%d\n", i + 1, ast->ast.instruction.operands[i].register_number);
            }
        }
    } else {
        printf("Empty or Null Line\n");
    }
}

/* Test function to test various inputs */
void run_tests() {
    const char *tests[] = {"MOV ,r1    ,    r5"};
    ASTNode ast = get_ast_node_from_line(tests[0]);
    print_ast_node(&ast);
    printf("\n");
    
}

int main() {
    run_tests();
    return 0;
}