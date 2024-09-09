#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "../include/ast.h"
#include "../include/frontend.h"
#include "../include/preprocessor.h"

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

static int is_label(char *str)
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
        int reg_number = operand[1] - '0';
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
        operand_type[operand_index] = OPERAND_LABEL;

        int len = strlen(operand);
        if (len > MAX_LABEL_LEN)
        {
            strcpy(ast->syntax_error, "Label too long");
        }
        else
        {
            strncpy(ast->ast.instruction.operands[operand_index].label, operand, MAX_LABEL_LEN);
            ast->ast.instruction.operands[operand_index].label[MAX_LABEL_LEN] = '\0';
        }
    }
}

seperated_string seperate_string_by_spaces(const char *str)
{
    seperated_string result;
    result.strings_count = 0;
    char *str_copy = strdup(str);

    if (str_copy == NULL)
    {
        result.strings_count = -1;
        return result;
    }

    char *current = str_copy;
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

        /* Handle non-whitespace tokens */
        char *start = current;
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
    ASTNode ast = {0};
    seperated_string tokens = seperate_string_by_spaces(line);

    if (tokens.strings_count < 1)
    {
        strcpy(ast.syntax_error, "Empty or invalid line");
        return ast;
    }

    int token_index = 0;

    /* Check if the first token is a label */
    if (is_label(tokens.strings[token_index]))
    {
        strncpy(ast.label_name, tokens.strings[token_index], strlen(tokens.strings[token_index]) - 1);
        ast.label_name[strlen(tokens.strings[token_index]) - 1] = '\0';
        token_index++;
    }

    /* Determine if the line is instruction or directive */
    if (token_index < tokens.strings_count)
    {
        char *first_token = tokens.strings[token_index];

        if (strcmp(first_token, ".extern") == 0)
        {
            ast.type = AST_DIR;
            ast.ast.directive.dir_type = DIR_EXTERN;
            token_index++;
        }
        else if (strcmp(first_token, ".entry") == 0)
        {
            ast.type = AST_DIR;
            ast.ast.directive.dir_type = DIR_ENTRY;
            token_index++;
        }
        else if (strcmp(first_token, ".string") == 0)
        {
            ast.type = AST_DIR;
            ast.ast.directive.dir_type = DIR_STRING;
            token_index++;
            if (token_index < tokens.strings_count)
            {
                int len = strlen(tokens.strings[token_index]);

                if (len > MAX_LABEL_LEN)
                {
                    strcpy(ast.syntax_error, "Label too long");
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
            }
        }
        else if (strcmp(first_token, ".data") == 0)
        {
            ast.type = AST_DIR;
            ast.ast.directive.dir_type = DIR_DATA;
            token_index++;
            int data_index = 0;

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
                    break;
                }

                token_index++;

                if (token_index < tokens.strings_count && strcmp(tokens.strings[token_index], ",") == 0)
                    token_index++; /* Skip the comma and move to the next number */
            }

            ast.ast.directive.dir_operand.data.data_count = data_index;
        }
        else
        {
            /* Assume it's an instruction */
            ast.type = AST_INST;
            token_index++;

            /* Parse the operands with comma handling */
            int operand_index = 0;
            bool last_token_was_comma = false;

            while (token_index < tokens.strings_count && operand_index < 2)
            {
                /* Check for commas */
                if (strcmp(tokens.strings[token_index], ",") == 0)
                {
                    if (last_token_was_comma)
                    {
                        strcpy(ast.syntax_error, "Multiple consecutive commas found between operands");
                        break;
                    }
                    last_token_was_comma = true;
                    token_index++;
                    continue;
                }

                /* Handle operands */
                parse_operand(tokens.strings[token_index], ast.ast.instruction.operand_type, &ast, operand_index);
                operand_index++;
                token_index++;
                last_token_was_comma = false;
            }

            /* Additional error if the loop completed but the last token was a comma */
            if (last_token_was_comma)
            {
                strcpy(ast.syntax_error, "Trailing comma found without operand");
            }
        }
    }
    else
    {
        ast.type = AST_NULL_LINE;
    }

    /* Clean up allocated strings in tokens */
    for (int i = 0; i < tokens.strings_count; i++)
    {
        free(tokens.strings[i]);
    }

    return ast;
}
