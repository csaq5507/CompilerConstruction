#include <stdio.h>

int div(int a , int b)
{
    printf("%d\n",b/a);
    return a / b;
}

int main(int argc, char* argv)
{
    int a = 5;
    int b;
    b = a * 6;
    int c = div(a,b);
    return 0;
}
