//
// Created by ivan on 17.05.18.
//

#ifndef MINICOMPILER_UTILS_H
#define MINICOMPILER_UTILS_H


#include <stdio.h>

char * new_string(const char * pattern, ...);
char * copy_string(const char * src);
char * copy_string_realloc(char * dest, const char * src);

#endif //MINICOMPILER_UTILS_H
