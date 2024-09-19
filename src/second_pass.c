#include "middleend.h"
#include "ast.h"

int encode_operand_type(OperandType type, int shift) {
    switch (type) {
        case OPERAND_IMMEDIATE:
            return (1 << (shift + 0));
        case OPERAND_LABEL:
            return (1 << (shift + 1));
        case OPERAND_INDIRECT_REGISTER:
            return (1 << (shift + 2));
        case OPERAND_IMMEDIATE_REGISTER:
            return (1 << (shift + 3));
        default:
            return 0;
    }
}

int encode_instruction_word(Instruction inst) {
    int word = 0;

    word |= 0b100;
    word |= encode_operand_type(inst.operand_type[1], 3);
    word |= encode_operand_type(inst.operand_type[0], 7);
    word |= (inst.inst_type << 11);

    return word;
}

int second_pass(TranslationUnit *unit, const char *am_file_name, FILE *am_file) {
    char line[81] = {0};
    int error_flag = 0;
    int line_number = 1;
    ASTNode line_ast = {0};

    while (fgets(line, sizeof(line), am_file)) {
        line_ast = get_ast_node_from_line(line);

        if (line_ast.type == AST_INST) {
            int encoded_word = encode_instruction_word(line_ast.ast.instruction);
            add_instruction(&unit->instruction_image, encoded_word);
        }

        line_number++;
    }

    return error_flag;
}
