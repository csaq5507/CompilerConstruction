void main()
{
    int c;
    int a;
    print("a: ");
    a = read_int();
    int b;
    print("b: ");
    b = read_int();

    print("Min: ");
    if(a<b)
        print_int(a);
    else if(b<a)
        print_int(b);
    else
        print("none");
    print_nl();
    print("Factorial a: ");
    c = -a;
    print_int(c);
    print_nl();
}