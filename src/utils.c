//
// Created by ivan on 17.05.18.
//
#include "mCc/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <zconf.h>

#define ARRAY_SIZE 2048

char * new_string(const char * pattern, ...)
{
    char help[ARRAY_SIZE] = {0};
    va_list args;
    va_start(args, pattern);
    sprintf(help, pattern, args);
    va_end(args);
    char *dest = malloc(sizeof(char) * strlen(help));
    strcpy(dest,help);
    return dest;
}

char * copy_string(const char * src)
{
    char *dest = malloc(sizeof(char) * strlen(src));
    if(dest==NULL)
    {
        printf("Error malloc");
        assert(NULL);
    }
    if(strlen(src)==0)
        printf("lol");
    strcpy(dest,src);
    return dest;
}

char * copy_string_realloc(char * dest, const char * src)
{
    dest = realloc(dest,sizeof(char) * strlen(src));
    if(dest==NULL)
    {
        printf("Error malloc");
        assert(NULL);
    }
    if(strlen(src)==0)
        printf("lol");
    strcpy(dest,src);
    return dest;
}