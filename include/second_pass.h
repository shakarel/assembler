#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "ast.h"
#include "middleend.h"

int encode_operand_type(OperandType type, int shift);
int encode_first_instruction_word(Instruction inst);
int encode_registers_to_single_word(Instruction inst);
int encode_immediate_operand(Instruction inst, int operand_index);
int encode_label_operand(Instruction inst, int operand_index, SymbolTable *symbol_table);
int encode_register_operand(Instruction inst, int operand_index);

int two_operands_are_registers(ASTNode *line_ast);

void encode_data_image(DataImage *data_image);

#endif
