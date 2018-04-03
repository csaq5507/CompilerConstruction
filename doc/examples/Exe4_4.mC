

int main()
{
	print("Please enter the year");
	int y;
	y = read_int();
	print_y();
  print("Please enter the year of your birthday");
  int x;
  x = read_int();
  print_x();
  s=y-x;
  m=(y-x)|*12;
  d=(y-x)*365;
  h=(y-x)*8760;
  min=(y-x)*525600;
  sec=(y-x)*31536000;
  print("This is ");
  int result= s ,m ,d ,h ,min ,sec;
	print_int(result);
  
}
