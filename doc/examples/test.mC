int add(int a,int b)
{
	int c;
	c = a / b;
	return c;
}

void main()
{
	int a;
	a=3;
	if(a<4)
		add(4,5);
	else
		add(1,2);
}
