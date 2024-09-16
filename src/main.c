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
        printf("Data: %d\n", data_image->data[i]);
    }
}

int main(int argc, char *argv[])
{
    FILE *am_file;
    SymbolTable symbol_table;
    DataImage data_image;
    
    init_symbol_table(&symbol_table);
    init_data_image(&data_image);

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


    first_pass(&symbol_table, argv[1], am_file, &data_image);

    fclose(am_file);

    print_symbol_table(&symbol_table);
    print_data_image(&data_image);

    return 0;
}
