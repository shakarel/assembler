#include "middleend.h"
#include "ast.h"
#include "second_pass.h"

int second_pass(TranslationUnit *unit, const char *am_file_name, FILE *am_file)
{
    int i;
    char line[81] = {0};
    int error_flag = 0;
    ASTNode line_ast = {0};

    while (fgets(line, sizeof(line), am_file))
    {
        line_ast = get_ast_node_from_line(line);

        if (line_ast.type == AST_INST)
        {
            int encoded_word;
            encoded_word = encode_first_instruction_word(line_ast.ast.instruction);
            add_instruction(&unit->instruction_image, encoded_word);

            if (line_ast.ast.instruction.operand_count > 1 && two_operands_are_registers(&line_ast))
            {
                encoded_word = encode_registers_to_single_word(line_ast.ast.instruction);
                add_instruction(&unit->instruction_image, encoded_word);
            }

            else
            {
                for (i = 0; i < 2; i++)
                {
                    if (line_ast.ast.instruction.operand_type[i] == OPERAND_IMMEDIATE)
                    {
                        encoded_word = encode_immediate_operand(line_ast.ast.instruction, i);
                        add_instruction(&unit->instruction_image, encoded_word);
                    }
                    else if (line_ast.ast.instruction.operand_type[i] == OPERAND_LABEL)
                    {
                        encoded_word = encode_label_operand(line_ast.ast.instruction, i, &unit->symbol_table);
                        add_instruction(&unit->instruction_image, encoded_word);
                    }
                    else if ((line_ast.ast.instruction.operand_type[i] == OPERAND_IMMEDIATE_REGISTER) || (line_ast.ast.instruction.operand_type[i] == OPERAND_INDIRECT_REGISTER))
                    {
                        encoded_word = encode_register_operand(line_ast.ast.instruction, i);
                        add_instruction(&unit->instruction_image, encoded_word);
                    }
                }
            }
        }
    }

    return error_flag;
}
