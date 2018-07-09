void f1(int a)
{
}

int f2(int b) {
return 4;
}

int f3(int a, int b, int c) {
    return a + b + c;
}

void main()
{
    int [5] a;
    a[3]=5;
    f1(4);
    f1(a[3]);
    a[4] = f2(a[2]);
    f1(a[2] + 1);
    f3(1,2,3);
}