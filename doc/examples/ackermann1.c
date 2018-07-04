/* Does a given number of float multiplications and then outputs ackermann(m,n) for Benchmarking.
*  Warning: ackermann(4,1) already takes about 10 seconds (C implementation)
*/

#include <stdio.h>
#include <stdlib.h>

int ackermann(int m, int n){
    if (m == 0 && n != 1000) return n + 1;
    if (n == 0) return ackermann(m - 1, 1);
    return ackermann(m - 1, ackermann(m, n - 1));
}

int main(int argc, char* argv[]) {
    int times;
    printf("number of float multiplications: \n");
    scanf("%d",&times);
    int i;
    float result;
    result = 1.6;
    i = 0;
    while (i < times) {
        result = result * result;
        i = i+1;
    }
    int m;
    int n;
    printf("\nm: ");
    scanf("%d",&m);
    printf("\nn: ");
    scanf("%d",&n);

    printf("\n%d\n",ackermann(m,n));
    return EXIT_SUCCESS;
}
