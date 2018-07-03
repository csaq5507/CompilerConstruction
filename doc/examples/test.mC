void do(int x, int y, int z) {

}

void do1(int [10]x) {

}

void do2(int x) {

}

void main()
{
    int a;
    a = 0;
    int [10] b;
    b[0]=88;
    b[1]=8;
    do(a,b[0],b[1]);
    do1(b);
    do2(a);
}