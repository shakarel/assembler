#include "../include/second_pass.h"

int encode_operand_type(OperandType type, int shift)
{
    switch (type)
    {
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

int encode_first_instruction_word(Instruction inst)
{
    int word = 0;

    word |= 0x4;
    word |= encode_operand_type(inst.operand_type[1], 3);
    word |= encode_operand_type(inst.operand_type[0], 7);
    word |= (inst.inst_type << 11);

    return word;
}

int encode_registers_to_single_word(Instruction inst)
{
    int word = 0;

    word |= 0x4;
    word |= ((inst.operands[1].register_number & 0x7) << 3);
    word |= ((inst.operands[0].register_number & 0x7) << 6);

    return word;
}

int encode_immediate_operand(Instruction inst, int operand_index)
{
    int word = 0;
    int immediate_value = inst.operands[operand_index].immediate;

    word |= 0x4;

    word |= (immediate_value & 0xFFF) << 3;
    return word;
}

int encode_label_operand(Instruction inst, int operand_index, SymbolTable *symbol_table)
{
    int word = 0;
    const char *label_name = inst.operands[operand_index].label;

    Symbol *symbol = symbol_look_up(symbol_table, label_name);

    if (symbol->type == EXTERN)
        word |= 1;
    else
        word |= 2;

    word |= (symbol->address & 0xFFF) << 3;

    return word;
}

int encode_register_operand(Instruction inst, int operand_index)
{
    int word = 0;
    int reg_number;

    word |= 0x4;

    if (operand_index == 0)
    {
        reg_number = inst.operands[operand_index].register_number;
        word |= (reg_number & 0x7) << 6;
    }
    else if (operand_index == 1)
    {
        reg_number = inst.operands[operand_index].register_number;
        word |= (reg_number & 0x7) << 3;
    }

    return word;
}

int two_operands_are_registers(ASTNode *line_ast)
{
    return line_ast->ast.instruction.operand_type[0] == OPERAND_IMMEDIATE_REGISTER &&
           line_ast->ast.instruction.operand_type[1] == OPERAND_IMMEDIATE_REGISTER;
}
