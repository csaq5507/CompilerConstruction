#include <stdio.h>

void div(int a , int b)
{
    printf("%d\n",b/a);
    return ;
}

int main(int argc, char* argv)
{
    int a = 5;
    int b =3;
    float c = 4.4;
    b = a * b;
    c = c / 6;
    while(a>0)
        a--;
    div(a,b);
    return 0;
}
