/* Getting the maximum of two integers */

float max(int num1, int num2) {

   int result;

   if (num1 > num2) {
      result = num1;
   }
   else {
      result = num2;
   }

   return result;
}

int main () {

   int a;
   int b;
   a = 100;
   b = 200;
   int ret;

   ret = max(a, b);

   print( "Max value is :");
   print_int(ret);
   print("\n");
   return 0;
}