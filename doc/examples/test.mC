int f(int a)
{
    if(a == 1) return a;
    else return f(a-1) * a;
}

void main() {
    int a;
    a = read_int();
    print("fac of ");
    print_int(a);
    print(" is: ");
    print_int(f(a));
    print_nl();
    print_int(!a);
}