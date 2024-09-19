#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "ast.h"
#include "middleend.h"

int encode_operand_type(OperandType type, int shift);
int encode_first_instruction_word(Instruction inst);

#endif
