#include <stdio.h>
#include <stdlib.h>
#include "../include/preprocessor.h"

int main(const int argc, char *argv[]) {
    FILE *input;
    FILE *output;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input file> <output file>\n", argv[0]);
        return 1;
    }

    input = NULL;
    output = NULL;

    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Error opening input file");
        return 1;
    }

    output = fopen(argv[2], "w");
    if (output == NULL) {
        perror("Error opening output file");
        fclose(input);
        return 1;
    }

    preprocess(input, output);

    fclose(input);
    fclose(output);

    return 0;
}
