//N阶勒让德多项式 
#include <stdio.h>
#define N 100 

double x;
double calP(int n) {
	if (n == 0) {
		return 1;
	}
	if (n == 1) {
		return x;
	}
	return ((2.0 * n - 1.0) * x * calP(n - 1) - (n - 1.0) * calP(n - 2)) / n;
}

int main() {
	int n;
	scanf("%lf", &x);
	scanf("%d", &n);
	printf("%.2lf\n", calP(n));
}
