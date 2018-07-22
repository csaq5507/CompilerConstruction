
float abs(float a)
{
    print_float(a);
    if (a > 0.0)
    {
        return a;
    }
    else
    {
        return -a;
    }
}

bool isDiffSmallerEpsilon(float a, float b, float eps)
{
    print_float(a);
    print(" - ");
    print_float(b);
    print(": ");
    return (abs(a - b) < eps);
}

float betterGuess(float x, float guess)
{
    return ((guess + x/guess) / 2.0);
}

float sqrt(float x, float guess)
{
    if (isDiffSmallerEpsilon(x/guess, guess, 0.0001))
    {

        print("< ");
        print_float(0.0001);
        print("true");
        print_nl();
        return guess;
    }
    else
    {
            print("< ");
            print_float(0.0001);
        print("false");
        print_nl();
        int d;
        d=read_int();
        return sqrt(x, betterGuess(x, guess));
    }
}

void main()
{
    print("Please enter two floating point numbers: ");
    float n;
    n = read_float();
    float guess;
    guess = read_float();
    print_nl();
    print("The result is: ");
    print_float(sqrt(n,guess));
    print_nl();
}
