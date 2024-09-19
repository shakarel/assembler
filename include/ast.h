#ifndef AST_H
#define AST_H

#define MAX_LABEL_LEN 31
#define DIRECTIVE_DATA_CAPACITY 80
#define MAX_LINE_LEN 80

/* Enum for sentence types */
typedef enum
{
    AST_INST,
    AST_DIR,
    AST_NULL_LINE
} SentenceType;

/* Enum for directive types */
typedef enum
{
    DIR_EXTERN,
    DIR_ENTRY,
    DIR_STRING,
    DIR_DATA
} DirectiveType;

/* Enum for instruction types */
typedef enum
{
    mov,
    cmp,
    add,
    sub,
    lea,
    clr,
    not,
    inc,
    dec,
    jmp,
    bne,
    red,
    prn,
    jsr,
    rts,
    stop,
    invalid
} InstructionType;

/* Enum for operand types */
typedef enum
{
    OPERAND_IMMEDIATE,         /* mov #-1 */
    OPERAND_LABEL,             /* x: .data 23 or dec x */
    OPERAND_INDIRECT_REGISTER, /* inc *r1 */
    OPERAND_IMMEDIATE_REGISTER, /* clr r1 */
    OPERAND_NONE
} OperandType;

/* Struct for directive operands */
typedef struct
{
    DirectiveType dir_type;
    union
    {
        char label_name[MAX_LABEL_LEN + 1];
        char string[MAX_LINE_LEN + 1];
        struct
        {
            int data[DIRECTIVE_DATA_CAPACITY];
            int data_count;
        } data;
    } dir_operand;
} Directive;

/* Struct for instruction operands */
typedef struct
{
    int operand_count;
    InstructionType inst_type;
    OperandType operand_type[2];
    union
    {
        int immediate;
        char label[MAX_LABEL_LEN + 1];
        int register_number; /* for both immediate and indirect register  */
    } operands[2];
} Instruction;

/* Main AST Node struct */
typedef struct
{
    char syntax_error[200];
    char label_name[MAX_LABEL_LEN + 1];
    SentenceType type;
    union
    {
        Directive directive;
        Instruction instruction;
    } ast;
} ASTNode;

ASTNode get_ast_node_from_line(const char *line);
InstructionType check_instruction_type(char *token);
int get_required_operands(InstructionType inst_type);

#endif
