void main()
{
    int a;
    a = 0;
    int [10] b;
    b[5]=88;
    while(a<10)
    {
        b[a]=a;
        a = a+1;
    }
    print_int(b[5]);
    print_int(b[2]);
    print_int(b[91]);
}