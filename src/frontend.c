#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/ast.h"
#include "../include/frontend.h"

/* good */
static int is_num(char *str, int min, int max, int *result)
{
    char *endptr;
    long val;

    val = strtol(str, &endptr, 10);
    if (*endptr != '\0' || str == endptr)
        return 0;
    if (val < min || val > max)
        return 0;
    *result = (int)val;
    return 1;
}

/* good */
static int is_label_def(char *str)
{
    int i;
    int len = strlen(str);
    if (len == 0 || len > MAX_LABEL_LEN)
        return 0;

    if (str[len - 1] != ':')
        return 0;

    for (i = 0; i < len - 1; i++)
        if (!isalnum(str[i]))
            return 0;

    return 1;
}

/* good */
static void parse_operand(char *operand, OperandType *operand_type, ASTNode *ast, int operand_index)
{
    if (operand[0] == '#')
    {
        /* Immediate operand */
        int value;
        if (is_num(operand + 1, -32768, 32767, &value))
        {
            operand_type[operand_index] = OPERAND_IMMEDIATE;
            ast->ast.instruction.operands[operand_index].immediate = value;
        }
        else
        {
            strcpy(ast->syntax_error, "Invalid immediate value");
        }
    }
    else if (operand[0] == '*')
    {
        /* Indirect register operand */
        int reg_number;
        if (is_num(operand + 2, 0, 7, &reg_number))
        {
            operand_type[operand_index] = OPERAND_INDIRECT_REGISTER;
            ast->ast.instruction.operands[operand_index].register_number = reg_number;
        }
        else
        {
            strcpy(ast->syntax_error, "Invalid register number");
        }
    }
    else if (operand[0] == 'r' && strlen(operand) == 2 && isdigit(operand[1]))
    {
        /* Immediate register operand */
        int reg_number;
        reg_number = operand[1] - '0';

        if (reg_number >= 0 && reg_number <= 7)
        {
            operand_type[operand_index] = OPERAND_IMMEDIATE_REGISTER;
            ast->ast.instruction.operands[operand_index].register_number = reg_number;
        }
        else
        {
            strcpy(ast->syntax_error, "Invalid register number");
        }
    }
    else
    {
        /* Assume it's a label */
        int len, i;
        operand_type[operand_index] = OPERAND_LABEL;

        len = strlen(operand);
        if (len > MAX_LABEL_LEN)
            strcpy(ast->syntax_error, "Label too long");
        else if (!isalpha(operand[0]))
            strcpy(ast->syntax_error, "Label must start with an alphabetic letter");
        else
        {
            for (i = 1; i < len; i++)
            {
                if (!isalnum(operand[i]))
                {
                    strcpy(ast->syntax_error, "Label contains illegal characters");
                    break;
                }
            }

            strncpy(ast->ast.instruction.operands[operand_index].label, operand, MAX_LABEL_LEN);
            ast->ast.instruction.operands[operand_index].label[MAX_LABEL_LEN] = '\0';
        }
    }
}

/* good */
seperated_string seperate_string_by_spaces(const char *str)
{
    char *str_copy;
    char *current;
    char *start;
    seperated_string result;
    result.strings_count = 0;

    str_copy = strdup(str);

    if (str_copy == NULL)
    {
        result.strings_count = -1;
        return result;
    }

    current = str_copy;
    while (*current != '\0' && result.strings_count < MAX_LINE_LEN)
    {
        /* Skip leading whitespace */
        while (isspace((unsigned char)*current))
        {
            current++;
        }

        /* Handle comma as a separate token */
        if (*current == ',')
        {
            result.strings[result.strings_count] = strdup(",");
            result.strings_count++;
            current++;
            continue;
        }

        /* Handle quoted strings as single tokens */
        if (*current == '"')
        {
            start = ++current;

            while (*current != '\0' && *current != '"')
                current++;

            if (start != current)
            {
                result.strings[result.strings_count] = strndup(start, current - start);
                result.strings_count++;
            }

            if (*current == '"')
                current++;

            continue;
        }

        /* Handle non-whitespace, non-comma tokens */
        start = current;
        while (*current != '\0' && !isspace((unsigned char)*current) && *current != ',')
            current++;

        if (start != current)
        {
            result.strings[result.strings_count] = strndup(start, current - start);
            result.strings_count++;
        }
    }

    free(str_copy);
    return result;
}

ASTNode get_ast_node_from_line(const char *line)
{
    int token_index;
    int i;
    ASTNode ast = {0};
    seperated_string tokens = seperate_string_by_spaces(line);

    if (tokens.strings_count < 1)
    {
        strcpy(ast.syntax_error, "Empty or invalid line");
        return ast;
    }

    token_index = 0;

    for (i = 0; i < 2; i++)
    {
        ast.ast.instruction.operand_type[i] = OPERAND_NONE;
    }

    /* Check if the first token is a label */
    if (is_label_def(tokens.strings[token_index]))
    {
        strncpy(ast.label_name, tokens.strings[token_index], strlen(tokens.strings[token_index]) - 1);
        ast.label_name[strlen(tokens.strings[token_index]) - 1] = '\0';
        token_index++;
    }

    /* Determine if the line is instruction or directive */
    if (token_index < tokens.strings_count)
    {
        char *first_token;
        first_token = tokens.strings[token_index];

        if (strcmp(first_token, ";") == 0)
        {
            ast.type = AST_COMMENT;
        }

        else if (strcmp(first_token, ".extern") == 0)
        {
            ast.type = AST_DIR;
            ast.ast.directive.dir_type = DIR_EXTERN;
            token_index++;
            ast.label_name[0] = '\0';
            strcpy(ast.label_name, tokens.strings[token_index]);
            ast.label_name[strlen(tokens.strings[token_index])] = '\0';
        }
        else if (strcmp(first_token, ".entry") == 0)
        {
            ast.type = AST_DIR;
            ast.ast.directive.dir_type = DIR_ENTRY;
            token_index++;
            ast.label_name[0] = '\0';
            strcpy(ast.label_name, tokens.strings[token_index]);
            ast.label_name[strlen(tokens.strings[token_index])] = '\0';
        }
        else if (strcmp(first_token, ".string") == 0)
        {
            ast.type = AST_DIR;
            ast.ast.directive.dir_type = DIR_STRING;
            token_index++;
            if (token_index < tokens.strings_count)
            {
                int len;
                len = strlen(tokens.strings[token_index]);

                if (len > MAX_LABEL_LEN)
                {
                    strcpy(ast.syntax_error, "Label too long");
                    return ast;
                }
                else
                {
                    strncpy(ast.ast.directive.dir_operand.string, tokens.strings[token_index], MAX_LINE_LEN);
                    ast.ast.directive.dir_operand.string[MAX_LINE_LEN] = '\0';
                }
            }
            else
            {
                strcpy(ast.syntax_error, "Missing string for .string directive");
                return ast;
            }
        }
        else if (strcmp(first_token, ".data") == 0)
        {
            int data_index;
            ast.type = AST_DIR;
            ast.ast.directive.dir_type = DIR_DATA;
            token_index++;
            data_index = 0;

            while (token_index < tokens.strings_count && data_index < DIRECTIVE_DATA_CAPACITY)
            {
                int value;
                if (is_num(tokens.strings[token_index], -32768, 32767, &value))
                {
                    ast.ast.directive.dir_operand.data.data[data_index] = value;
                    data_index++;
                }
                else
                {
                    strcpy(ast.syntax_error, "Invalid data value");
                    return ast;
                }

                token_index++;

                if (token_index < tokens.strings_count && strcmp(tokens.strings[token_index], ",") == 0)
                {
                    token_index++;

                    if (token_index == tokens.strings_count)
                    {
                        strcpy(ast.syntax_error, "Trailing comma without value");
                        return ast;
                    }
                }
            }

            ast.ast.directive.dir_operand.data.data_count = data_index;
        }
        else
        {
            /* Assume it's an instruction */
            int operand_index;
            int last_token_was_comma;
            int required_operands;

            ast.type = AST_INST;

            if (token_index < tokens.strings_count)
                ast.ast.instruction.inst_type = check_instruction_type(tokens.strings[token_index]);

            if (ast.ast.instruction.inst_type == invalid)
            {
                strcpy(ast.syntax_error, "Didn't find instruction");
                return ast;
            }

            token_index++;

            operand_index = 0;
            last_token_was_comma = 0;
            required_operands = get_required_operands(ast.ast.instruction.inst_type);

            if (token_index < tokens.strings_count && strcmp(tokens.strings[token_index], ",") == 0)
            {
                strcpy(ast.syntax_error, "Comma before first operand");
                return ast;
            }

            ast.ast.instruction.operand_count = 0;

            while (token_index < tokens.strings_count)
            {
                /* Check for commas */
                if (strcmp(tokens.strings[token_index], ",") == 0)
                {
                    if (last_token_was_comma == 1)
                    {
                        strcpy(ast.syntax_error, "Multiple consecutive commas found between operands");
                        return ast;
                    }
                    last_token_was_comma = 1;
                    token_index++;
                    continue;
                }

                /* Handle operands */
                parse_operand(tokens.strings[token_index], ast.ast.instruction.operand_type, &ast, operand_index);
                ast.ast.instruction.operand_count++;
                operand_index++;
                token_index++;
                last_token_was_comma = 0;

                /* Check if the last operand is followed by a comma */
                if (token_index == tokens.strings_count - 1 && strcmp(tokens.strings[token_index], ",") == 0)
                {
                    strcpy(ast.syntax_error, "Comma after the last operand");
                    return ast;
                }
            }

            /* Ensure operands are correctly placed */
            if (operand_index == 1)
            {
                /* If instruction requires 2 operands, move the first operand to destination (operand[1]) */
                if (required_operands == 1)
                {
                    ast.ast.instruction.operand_type[1] = ast.ast.instruction.operand_type[0];
                    ast.ast.instruction.operands[1] = ast.ast.instruction.operands[0];
                    ast.ast.instruction.operand_type[0] = OPERAND_NONE;
                }
            }

            /* Check if we have the correct number of operands */
            if (operand_index < required_operands)
            {
                strcpy(ast.syntax_error, "Not enough operands");
                return ast;
            }
            else if (operand_index > required_operands)
            {
                strcpy(ast.syntax_error, "Too many operands");
                return ast;
            }

            /* Validate operand types */
            if (validate_instruction_operands_types(ast.ast.instruction.inst_type, ast.ast.instruction.operand_type, operand_index) == 0)
            {
                strcpy(ast.syntax_error, "Invalid operand types");
                return ast;
            }
        }
    }
    else
    {
        ast.type = AST_NULL_LINE;
    }

    /* Clean up allocated strings in tokens */
    for (i = 0; i < tokens.strings_count; i++)
    {
        free(tokens.strings[i]);
    }

    return ast;
}
