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
    if(a<b){
        print_int(a);
    }else if(b<a)
    {
        print_int(b);
    } else {
        print("none");
    }
    print_nl();
    print("Negation a: ");
    c = -a;
    print_int(c);
    print_nl();
    print("Factorial a: ");
    c = !a;
    print_int(c);
    print_nl();


    int [4] y;
    a = 3;
    while(a>=0)
    {
        print("y[");
        print_int(a);
        print("]=");
        y[a]=-read_int();
        print_nl();
        a=a-1;
    }
    print("y[2]=");
    print_int(y[2]);
}