#ifndef FRONTEND_H
#define FRONTEND_H

#include "preprocessor.h"

typedef struct seperated_string {
    char *strings[MAX_LINE_LEN];
    int strings_count;
} seperated_string;

seperated_string seperate_string_by_spaces(const char *str);
#endif
