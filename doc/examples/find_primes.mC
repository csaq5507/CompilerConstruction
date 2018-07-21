bool check_primes_(int y, int i)
{
    print("y: ");
    print_int(y);
    print_nl();
    print("i: ");
    print_int(i);
    print_nl();
    if (y == i) {
        return true;
    } else if (0 == modulo(y, i)) {
        return false;
    }
    i = i + 1;
    return check_primes_(y, i);
}

bool check_primes(int x) {
	return check_primes_(x, 2);
}

int modulo(int a, int b)
{
    int tmp;
    tmp = a / b;
    tmp = b * tmp;
    return a - tmp;

}

void main()
{
	print("Please enter a number: ");

	int n;
	n = read_int();
	print_nl();

	bool result;
	result = check_primes(n);

	if (result) {
	    print("Number was a prime!");
	} else {
	    print("Number was no prime!");
	}
	print_nl();
}
