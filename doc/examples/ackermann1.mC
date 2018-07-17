/* Does a given number of float multiplications and then outputs ackermann(m,n) for Benchmarking.
*  Warning: ackermann(4,1) already takes about 10 seconds (C implementation)
*/

int ackermann(int m, int n){
    print("m: ");
    print_int(m);
    print(", n: ");
    print_int(n);
    print_nl();
    int a;
    a=read_int();
        if ( (m == 0) && (n != 1000) ) return n + 1;
        if (n == 0) return ackermann(m - 1, 1);
        return ackermann(m - 1, ackermann(m, n - 1));
}

void main() {
    int m;
    int n;
    print("m: ");
    m = read_int();
    print_nl();
    print("n: ");
    n = read_int();
    print_nl();
    print_int(ackermann(m,n));
    print_nl();
}
