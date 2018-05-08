int test1() {
    return 10;
}

void test2(int a, int b, int c) {
}

void main() {
    int a;
    a = 10;
    test2(10, a, 5);
    test1();

    if (a == 5) {
        print("A");
    } else {
        test2(10,5,60);
    }

    a = 0;
    while (a <= 5) {
        a = a + 1;
    }
    a = 10;

    int[5] b;
    a = b[2];
    b[1] = a;
}
