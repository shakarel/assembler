#include <stdio.h>
#include <stdlib.h>
#include "../include/ast.h"
#include "../include/middleend.h"

void print_symbol_table(SymbolTable *symbol_table)
{
    int i;
    printf("\t\t\t\t\t\t\tSymbol Table:\n");
    for (i = 0; i < symbol_table->count; i++)
    {
        printf("Symbol: %s | Address: %d | Type: %d\n",
               symbol_table->symbols[i].name,
               symbol_table->symbols[i].address,
               symbol_table->symbols[i].type);
    }
}

void print_data_image(DataImage *data_image)
{
    int i;
    printf("\t\t\t\t\t\t\tData Image:\n");

    for (i = 0; i < data_image->count; i++)
    {
        int bit;
        unsigned int data = data_image->data[i];
        printf("Data Word %d: ", i);

        for (bit = 14; bit >= 0; bit--)
        {
            printf("%d", (data >> bit) & 1);
        }

        printf("\n");
    }
}

void print_instruction_image(InstructionImage *instruction_image)
{
    int i;
    printf("\t\t\t\t\t\t\tInstruction Image:\n");

    for (i = 0; i < instruction_image->count; i++)
    {
        int bit;
        unsigned int instruction = instruction_image->instructions[i];
        printf("Instruction Word %d: ", i);

        for (bit = 14; bit >= 0; bit--)
        {
            printf("%d", (instruction >> bit) & 1);
        }

        printf("\n");
    }
}

/* int main(int argc, char *argv[])
{
    FILE *am_file;
    TranslationUnit unit;

    init_translation_unit(&unit);

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <assembly file>\n", argv[0]);
        return 1;
    }

    am_file = fopen(argv[1], "r");
    if (!am_file)
    {
        fprintf(stderr, "Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    first_pass(&unit, argv[1], am_file);

    printf("IC: %d\n", unit.IC);
    printf("DC: %d\n", unit.DC);

    fclose(am_file);

    am_file = fopen(argv[1], "r");
    if (!am_file)
    {
        fprintf(stderr, "Error: Could not open file %s for second pass\n", argv[1]);
        return 1;
    }

    second_pass(&unit, argv[1], am_file);
    fclose(am_file);

    print_symbol_table(&unit.symbol_table);
    print_data_image(&unit.data_image);
    print_instruction_image(&unit.instruction_image);

    free_translation_unit(&unit);

    return 0;
}
*/
