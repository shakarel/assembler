#include "middleend.h"
#include "ast.h"
#include "second_pass.h"

int second_pass(TranslationUnit *unit, const char *am_file_name, FILE *am_file) {
    char line[81] = {0};
    int error_flag = 0;
    int line_number = 1;
    ASTNode line_ast = {0};

    while (fgets(line, sizeof(line), am_file)) {
        line_ast = get_ast_node_from_line(line);

        if (line_ast.type == AST_INST) {
            int encoded_word = encode_first_instruction_word(line_ast.ast.instruction);
            add_instruction(&unit->instruction_image, encoded_word);
        }

        line_number++;
    }

    return error_flag;
}
