int fib(int n)
{
	if (n < 2) {
		return n;
	}

    int left;
    int right;
    left  = fib(n - 1);
    right = fib(n - 2);
	return left + right;
}

void main()
{
	print("Please enter a number: ");

	int n;
	n = read_int();
	print_nl();

	int result;
	result = fib(n);

	print("fib(");
	print_int(n);
	print(") = ");
	print_int(result);
	print_nl();
}
