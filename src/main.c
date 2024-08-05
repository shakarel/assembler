#include <stdio.h>
#include "../include/preprocessor.h"

int main(int argc, char *argv[]) {
    char *input_file;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    input_file = argv[1];

    preprocessor(input_file);

    printf("Preprocessing completed. Output written to: %s\n", add_am_to_file_name(input_file));

    return 0;
}
