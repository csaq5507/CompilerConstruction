#include <stdio.h>

void  __attribute__((cdecl)) print(const char *msg);
void  __attribute__((cdecl)) print_nl(void);
void  __attribute__((cdecl)) print_int(long x);
void  __attribute__((cdecl)) print_float(float x);

void print(const char *msg)
{
    printf("%s", msg);
}

void print_nl(void)
{
    printf("\n");
}

void print_int(long x)
{
    printf("%ld", x);
}

void print_float(float x)
{
    printf("%f", x);
}