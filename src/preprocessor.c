#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/macros.h"
#include "../include/preprocessor.h"

void preprocess(FILE *input, FILE *output) {
    char line[MAX_LINE_LENGTH];
    char macro_name[MAX_MACRO_NAME_LENGTH];
    char macro_code[MAX_CODE_LENGTH];
    char *expanded_code = NULL;
    int in_macro = 0;

    macro_code[0] = '\0'; /* Initialize the macro code to be empty */

    /* Read each line of the file */
    while (fgets(line, MAX_LINE_LENGTH, input)) {

        /* Determine if a macro is being declared */
        if (strncmp(line, MACRO_START, strlen(MACRO_START)) == 0) {
            sscanf(line, "macr %s", macro_name);
            in_macro = 1;
            macro_code[0] = '\0'; /* Reset the macro code for the upcoming macro */
        }

        /* Determine if ending a macro declaration */
        else if (strncmp(line, MACRO_END, strlen(MACRO_END)) == 0) {
            add_macro(macro_name, macro_code);
            in_macro = 0;
        }

        /* If inside macro definition - concatenate the line to the current macro code */
        else if (in_macro) {
            strcat(macro_code, line);
        }

        /* Any other line */
        else {
            /* Replace the \n in the word with \0 (null terminator) */
            line[strcspn(line, "\n")] = '\0';
            expanded_code = get_macro_code(line);

            if (expanded_code) {
                fputs(expanded_code, output);
            }
            else {
                fputs(line, output);
            }
        }
    }
}
