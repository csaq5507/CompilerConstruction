/* Some simple function operations */

int addition(int num1, int num2) {
     int sum;
     sum = num1+num2;
     return sum;
}

int subtraction(int num1, int num2) {
     int sum;
     sum = num1-num2;
     return sum;
}

int main() {
     int var1 = 12;
     int var2 = 55;

     int sum = addition(var1, var2);
     print("Addition = ");
     print_int(sum);
     print("\n");
     
     int sub = subtraction(var1, var2);
     print("Subtraction = ");
     print_int(sub);
     print("\n");
     
     return 0;
}
