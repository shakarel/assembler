#include "../include/second_pass.h"

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

int encode_first_instruction_word(Instruction inst) {
    int word = 0;

    word |= 0x4;
    word |= encode_operand_type(inst.operand_type[1], 3);
    word |= encode_operand_type(inst.operand_type[0], 7);
    word |= (inst.inst_type << 11);

    return word;
}
