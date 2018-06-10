int f() {
    int a;
    int b;
    int c;
    a = 10;
    b = 32;
    c = a + b;
    return c;
}

void main()
{
    int [10]a;
    int [10]c;
    int b;
    b=0;
    if (b == 0) {
        a[b] = 0;
        a[1] = c[2];
        b=a[5];
    } else if (b == 1) {
       a[b] = 1;
       a[1] = c[3];
       b=a[2];
    } else {
        a[b] = 7;
        a[4] = c[3];
        b=a[8];
    }

    while (b < 10) {
        b = b + 1;
    }
    b = 5;
    a[1] = 2;
}