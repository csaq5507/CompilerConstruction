int modulo(int a, int b)
{
    int tmp;
    tmp = a / b;
    tmp = b * tmp;
    return a - tmp;

}

int check_primes_(int n, int i)
{
    if (n == i) {
        return 1;
    } else if (0 == modulo(n, i)) {
        return 0;
    }
    i = i + 1;
    return check_primes_(n, i);
}

int check_primes(int n)
{
	return check_primes_(n, 2);
}

void main()
{
	print("Please enter a number: ");

	int n;
	n = read_int();
	print_nl();

	int result;
	result = check_primes(n);

	if (result != 0) {
	    print("Number was a prime!");
	} else {
	    print("Number was no prime!");
	}
	print_nl();
}
