void lucas_number (int num, int x, int y){
 int z;
 z=0;
 while(num >= x)
 {
   print_int(x);
   print(", ");
   z=x+y;
   x=y;
   y=z;
 }

}

void main()
{
    int x;
    int y;
    int num;
    print("Enter the limit of the Lucas number:");
    print_nl();
    x=2;
    y=1;
    num= read_int();
    print("The Lucas numbers are: ");
    lucas_number(num, x, y);
    print_nl();

}


