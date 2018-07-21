float calc_diameter(float radius)
{
	return radius + radius;
}

float calc_small_r(float radius)
{
	return radius / 2.0;
}

float calc_circumference(float radius, float pi)
{
    float tmp;
    tmp = 2.0 * radius;
	return tmp * pi;
}

float calc_area(float radius, float pi)
{
	return radius * radius * pi;
}

void main()
{
	print("Please enter a radius: ");
	print_nl();

	float radius;
	radius = read_float();

	float pi;
	pi = 3.1415;

	print("Circle Properties [r=");
	print_float(radius);
	print("]");
	print_nl();

	print("Diameter: ");
	print_float(calc_diameter(radius));
	print_nl();

	print("Circumference: ");
	print_float(calc_circumference(radius, pi));
	print_nl();

	print("Area: ");
	print_float(calc_area(radius, pi));
	print_nl();

		print("small_r: ");
    	print_float(calc_small_r(radius));
    	print_nl();
}
