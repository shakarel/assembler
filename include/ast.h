#ifndef AST_H
#define AST_H

#define MAX_LABEL_LEN 31
#define DIRECTIVE_DATA_CAPACITY 80

/* Enum for sentence types */
typedef enum {
    AST_INST,
    AST_DIR,
    AST_NULL_LINE
} SentenceType;

/* Enum for directive types */
typedef enum {
    DIR_EXTERN,
    DIR_ENTRY,
    DIR_STRING,
    DIR_DATA
} DirectiveType;

/* Enum for instruction types */
typedef enum {
    MOV, CMP, ADD, SUB, LEA, CLR, NOT, INC, DEC, JMP, BNE, RED, PRN, JSR, RTS, STOP
} InstructionType;

/* Enum for operand types */
typedef enum {
    OPERAND_IMMEDIATE,          /* mov #-1 */
    OPERAND_LABEL,              /* x: .data 23 or dec x */
    OPERAND_INDIRECT_REGISTER,  /* inc *r1 */
    OPERAND_IMMEDIATE_REGISTER  /* clr r1 */
} OperandType;

/* Struct for directive operands */
typedef struct {
    DirectiveType dir_type;
    union {
        char *label_name;  /* extern or entry */
        char *string;      /* string */
        struct {           /* data */
            int data[DIRECTIVE_DATA_CAPACITY];
            int data_count;
        } data;
    } dir_operand;
} Directive;

/* Struct for instruction operands */
typedef struct {
    InstructionType inst_type;
    OperandType operand_type[2];
    union {
        int immediate;
        char *label;
        int register_number;  /* for both immediate and indirect register  */
    } operands[2];
} Instruction;

/* Main AST Node struct */
typedef struct {
    char syntax_error[200];  /* space for error messages */
    char label_name[MAX_LABEL_LEN + 1];  /* label name */
    SentenceType type;  /* type of the sentence */
    union {
        Directive directive;  /* directive type sentence */
        Instruction instruction;  /* instruction type sentence */
    } ast;
} ASTNode;

ASTNode get_ast_node_from_line(const char *line);

#endif
