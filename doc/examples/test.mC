int f(int a)
{
    print_int(a);
    print_nl();
    return a * 2;
}

void main() {
    int a;
    a = read_int();
    print("fac of ");
    print_int(a);
    print(" is: ");
    int b;
    b = f(a);
    print_int(b);
    print_nl();
    print_int(!a);
}