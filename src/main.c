#include <stdio.h>
#include "../include/preprocessor.h"
#include "../include/middleend.h"

void print_object_file(char *base_file_name, TranslationUnit *unit)
{
    char output_file_name[256];
    int i;
    FILE *file;

    sprintf(output_file_name, "%s.obj", base_file_name);

    file = fopen(output_file_name, "w");
    if (file == NULL)
    {
        perror("Error opening file");
        return;
    }

    fprintf(file, "%d %d\n", unit->IC, unit->DC);

    for (i = 0; i < unit->IC; i++)
    {
        int value = unit->instruction_image.instructions[i];
        fprintf(file, "%04d %05o\n", i + 100, value);
    }

    for (i = 0; i < unit->data_image.count; i++)
    {
        int address = unit->IC + 100 + i;
        int value = unit->data_image.data[i];
        fprintf(file, "%04d %05o\n", address, value);
    }

    fclose(file);
}

void print_entry_file(char *base_file_name, TranslationUnit *unit) {
    char output_file_name[256];
    int i;
    FILE *file;

    sprintf(output_file_name, "%s.ent", base_file_name);

    file = fopen(output_file_name, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    for (i = 0; i < unit->symbol_table.count; i++) {
        if (unit->symbol_table.symbols[i].type == ENTRY || unit->symbol_table.symbols[i].type == ENTRY_CODE || unit->symbol_table.symbols[i].type == ENTRY_DATA) {
            fprintf(file, "%s %d\n", unit->symbol_table.symbols[i].name, unit->symbol_table.symbols[i].address);
        }
    }

    fclose(file);
}

void print_extern_file(char *base_file_name, TranslationUnit *unit) {
    char output_file_name[256];
    int i;
    FILE *file;

    sprintf(output_file_name, "%s.ext", base_file_name);

    file = fopen(output_file_name, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    for (i = 0; i < unit->symbol_table.count; i++) {
        if (unit->symbol_table.symbols[i].type == EXTERN) {
            fprintf(file, "%s %d\n", unit->symbol_table.symbols[i].name, unit->symbol_table.symbols[i].address);
        }
    }

    fclose(file);
}


int main(int argc, char *argv[])
{
    int i;
    FILE *am_file;
    TranslationUnit unit;
    init_translation_unit(&unit);

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    for (i = 1; i < argc; i++)
    {
        preprocessor(argv[i]);

        am_file = fopen(add_am_to_file_name(argv[i]), "r");
        if (am_file)
        {
            if (!first_pass(&unit, add_am_to_file_name(argv[i]), am_file))
            {
                rewind(am_file);
                if (!second_pass(&unit, add_am_to_file_name(argv[i]), am_file))
                {
                    print_object_file(argv[i], &unit);
                    print_entry_file(argv[i], &unit);
                    print_extern_file(argv[i], &unit);
                }
            }
            fclose(am_file);
        }
    
    }

    free_translation_unit(&unit);

    return 0;
}
