#include <string.h>
#include "ast.h"
#include "frontend.h"

InstructionType check_instruction_type(char *token)
{
    if (strcmp(token, "mov") == 0)
        return mov;
    else if (strcmp(token, "cmp") == 0)
        return cmp;
    else if (strcmp(token, "add") == 0)
        return add;
    else if (strcmp(token, "sub") == 0)
        return sub;
    else if (strcmp(token, "lea") == 0)
        return lea;
    else if (strcmp(token, "clr") == 0)
        return clr;
    else if (strcmp(token, "not") == 0)
        return not;
    else if (strcmp(token, "inc") == 0)
        return inc;
    else if (strcmp(token, "dec") == 0)
        return dec;
    else if (strcmp(token, "jmp") == 0)
        return jmp;
    else if (strcmp(token, "bne") == 0)
        return bne;
    else if (strcmp(token, "red") == 0)
        return red;
    else if (strcmp(token, "prn") == 0)
        return prn;
    else if (strcmp(token, "jsr") == 0)
        return jsr;
    else if (strcmp(token, "rts") == 0)
        return rts;
    else if (strcmp(token, "stop") == 0)
        return stop;
    else
        return invalid;
}

int get_required_operands(InstructionType inst_type)
{
    switch (inst_type)
    {
    case mov:
    case cmp:
    case add:
    case sub:
    case lea:
        return 2;

    case clr:
    case not:
    case inc:
    case jmp:
    case bne:
    case red:
    case prn:
    case jsr:
    case dec:
        return 1;

    case rts:
    case stop:
        return 0;

    default:
        return -1;
    }
}

int validate_instruction_operands_types(InstructionType inst_type, OperandType *operand_types, int operand_count)
{
    if (operand_count > 0 && operand_types[0] == OPERAND_IMMEDIATE)
    {
        if (inst_type == mov || inst_type == add || inst_type == sub ||
            inst_type == clr || inst_type == not || inst_type == inc ||
            inst_type == dec || inst_type == red)
            return 0;
    }

    if (inst_type == lea && operand_count > 1)
    {
        if (operand_types[0] == OPERAND_IMMEDIATE || operand_types[1] != OPERAND_LABEL)
            return 0;
    }

    if (operand_count > 0 && (operand_types[0] == OPERAND_IMMEDIATE || operand_types[0] == OPERAND_IMMEDIATE_REGISTER))
    {
        if (inst_type == jmp || inst_type == bne || inst_type == jsr)
            return 0;
    }

    return 1;
}
