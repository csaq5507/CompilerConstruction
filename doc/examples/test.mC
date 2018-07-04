void do(int a)
{
print("a: ");
print_int(a);
print_nl();
}

void main()
{
    int [5] a;
    a[3]=5;
    do(4);
    do(a[3]);
}