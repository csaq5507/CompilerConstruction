#include <stdio.h>

void print_int(long x)
{
    printf("%ld", x);
}
void main()
{
    int a;
    a = 0;
    int  b[10];
    int c = 5;
    b[c]=88;
    while(a<10)
    {
        b[a]=a;
        a = a+1;
    }
    print_int(b[5]);
}