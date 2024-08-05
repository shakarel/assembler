#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "macros.h"
#define MAX_LINE_LEN 80
#define MACRO_DEF_KEYWORD "macr"
#define END_MACRO_DEF_KEYWORD "endmacr"

typedef enum LineType {
    MACRO_DEFINITION,
    MACRO_END_DEFINITION,
    MACRO_CALL,
    ANY_OTHER_LINE
} LineType;

int is_macro_definition(char *line);
int is_macro_end_definition(char *line);
int is_macro_call(MacroTable *table, char *line, int chars_to_read);
LineType determine_line_type(MacroTable *table, char *line, int chars_to_read);
char *add_as_to_file_name(const char *file_name);
char *add_am_to_file_name(const char *file_name);
void preprocessor(char *input);

#endif
