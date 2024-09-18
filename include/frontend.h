#ifndef FRONTEND_H
#define FRONTEND_H

#include "ast.h"

#define MAX_LINE_LEN 80

typedef struct seperated_string {
    char *strings[MAX_LINE_LEN];
    int strings_count;
} seperated_string;

seperated_string seperate_string_by_spaces(const char *str);
int validate_instruction_operands_types(InstructionType inst_type, OperandType *operand_types, int operand_count);

#endif
