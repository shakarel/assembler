#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/preprocessor.h"

int is_valid_macro_name(char *name) {
    int i;

    const char *invalid_names[] = {
        "mov", "cmp", "add", "sub", "lea", "clr",
        "not", "inc", "dec", "jmp", "bne", "red",
        "prn", "jsr", "rts", "stop"
    };

    const int num_invalid_names = sizeof(invalid_names) / sizeof(invalid_names[0]);
    for (i = 0; i < num_invalid_names; i++) {
        if (strcmp(name, invalid_names[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

int is_macro_definition(char *line) {
    int len;
    int i;
    const char *name_start;
    const char *name_end;
    int name_length;
    char *macro_name;

    len = strlen(line);
    i = 0;

    /* Skip leading whitespace */
    while (i < len && isspace(line[i]))
        i++;

    /* Check for the "macr" keyword */
    if (strncmp(&line[i], MACRO_DEF_KEYWORD, 4) != 0 || (i + 4 < len && !isspace(line[i + 4]))) {
        return 0;
    }
    i += 4;

    /* Skip whitespace after "macr" */
    while (i < len && isspace(line[i]))
        i++;

    /* Check if there is no macro name */
    if (i >= len) {
        fprintf(stderr, "Error: No Macro name has been specified.");
        exit(1);
    }

    /* Extract the macro name */
    name_start = &line[i];
    name_end = name_start;

    /* Find the end of the macro name */
    while (name_end < &line[len] && !isspace(*name_end))
        name_end++;

    /* Strip trailing spaces from the macro name */
    while (name_end > name_start && isspace(*(name_end - 1)))
        name_end--;

    /* Create a null-terminated macro name string */
    name_length = name_end - name_start;
    macro_name = (char *)malloc(name_length + 1);
    if (macro_name == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.");
        exit(1);
    }
    strncpy(macro_name, name_start, name_length);
    macro_name[name_length] = '\0';

    /* Check if the macro name is valid (not an Assembly operation) */
    if (!is_valid_macro_name(macro_name)) {
        fprintf(stderr, "Error: Macro name can't be an Assembly operation.");
        free(macro_name);
        exit(1);
    }

    free(macro_name);
    return 1;
}

int is_macro_end_definition(char *line) {
    int i;
    int len;

    i = 0;
    len = strlen(line);

    /* Skip leading white spaces */
    while (i < len && isspace(line[i]))
        i++;

    if (strncmp(END_MACRO_DEF_KEYWORD, &line[i], strlen(END_MACRO_DEF_KEYWORD)) != 0 ||
       (i + strlen(END_MACRO_DEF_KEYWORD) < len && !isspace(line[i + strlen(END_MACRO_DEF_KEYWORD)])))
        return 0;

    return 1;
}

int is_macro_call(MacroTable *table, char *line, int chars_to_read) {
    int i;
    for (i = 0; i < table->size; i++)
        if (strncmp(line, table->macros[i]->name, chars_to_read) == 0)
            return 1;

    return 0;
}

int is_null_line(char *line) {
    return line[0] == '\0';
}

LineType determine_line_type(MacroTable *table, char *line, int chars_to_read) {
    /* New Macro definition */
    if (is_macro_definition(line))
        return MACRO_DEFINITION;

    /* Ending a Macro definition */
    if (is_macro_end_definition(line))
        return MACRO_END_DEFINITION;

    /* Macro call */
    if (is_macro_call(table, line, chars_to_read))
        return MACRO_CALL;

    /* Null line */
    if (is_null_line(line))
        return NULL_LINE;

    /* Any other line */
    return ANY_OTHER_LINE;
}

char *add_as_to_file_name(const char *file_name) {
    char *input_file = (char *)malloc(strlen(file_name) + 4);
    if (input_file == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }
    strcpy(input_file, file_name);
    strcat(input_file, ".as");
    return input_file;
}

char *add_am_to_file_name(const char *file_name) {
    char *input_file = (char *)malloc(strlen(file_name) + 4);
    if (input_file == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }
    strcpy(input_file, file_name);
    strcat(input_file, ".am");
    return input_file;
}
