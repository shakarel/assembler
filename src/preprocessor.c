#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../include/preprocessor.h"
#include "../include/macros.h"


void preprocessor(char *input) {
    FILE *input_file;
    FILE *output_file;
    MacroTable *table;

    char line[MAX_LINE_LEN];
    int in_macro;
    char macro_name[MAX_LINE_LEN];
    char macro_code[MAX_LINE_LEN * 10];

    macro_name[0] = '\0';
    macro_code[0] = '\0';
    in_macro = 0;

    input_file = fopen(add_as_to_file_name(input), "r");
    if (input_file == NULL) {
        fprintf(stderr, "Error: Failed to open input.as file.\n");
        exit(1);
    }

    output_file = fopen(add_am_to_file_name(input), "w");
    if (output_file == NULL) {
        fprintf(stderr, "Error: Failed to open output file.\n");
        fclose(input_file);
        exit(1);
    }

    table = initialize_macro_table(0);
    if (!table) {
        fclose(input_file);
        fclose(output_file);
        exit(1);
    }

    while (fgets(line, MAX_LINE_LEN, input_file)) {
        char *start;
        char *end;

        start = line;
        while (isspace((unsigned char)*start)) start++;
        end  = line + strlen(line) - 1;
        while (end > start && isspace((unsigned char)*end)) end--;
        end[1] = '\0';

        switch (determine_line_type(table, start, MAX_LINE_LEN)) {
            case NULL_LINE:
                continue;

            case MACRO_DEFINITION:
                sscanf(start, "macr %s", macro_name);
                in_macro = 1;
                macro_code[0] = '\0';
                break;

            case MACRO_END_DEFINITION:
                add_macro(table, macro_name, macro_code);
                in_macro = 0;
                break;

            case MACRO_CALL:
                {
                    char *expanded_code = get_macro_code(table, start);
                    if (expanded_code) {
                        fputs(expanded_code, output_file);
                    } else {
                        fputs(start, output_file);
                        fputs("\n", output_file);
                    }
                }
                break;

            default:
                if (in_macro) {
                    strcat(macro_code, start);
                    strcat(macro_code, "\n");
                } else {
                    fputs(start, output_file);
                    fputs("\n", output_file);
                }
                break;
        }
    }

    fclose(input_file);
    fclose(output_file);
    free_macro_table(table);
}
