/* Generates a diamond pattern */

int main() {
  
  int n = 8;
  int space = 1;
  space = n - 1;
   
  int k = 1;
  int c;
  while (k <= n) {
    c = 1;
    while (c <= space) {
      print(" ");
      c = c + 1;
    }
    space = space - 1;
    c = 1;
    while (c <= space) {
      print("*");
      c = c + 1;
    }
    print("\n");
    k = k + 1;
  }

  space = 1;

  int k = 1;
  int c;
  while (k <= n) {
    c = 1;
    while (c <= space) {
      print(" ");
      c = c + 1;
    }
    space = space + 1;
    c = 1;
    while (c <= 2*(n-k)-1) {
      print("*");
      c = c + 1;
    }
    print("\n");
    k = k + 1;
  }
 
  return 0;
}
