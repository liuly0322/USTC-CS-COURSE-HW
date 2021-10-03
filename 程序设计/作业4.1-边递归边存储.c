//N阶勒让德多项式 
#include <stdio.h>
#define N 100 

double x, P[N];
int flag[N] = { 0 };

double calP(int n) {
	if (n == 0) {
		return 1;
	}
	if (n == 1) {
		return x;
	}
	if (flag[n - 1] == 0) {
		P[n - 1] = calP(n - 1);
		flag[n - 1] = 1;
	}
	if (flag[n - 2] == 0) {
		P[n - 2] = calP(n - 2);
		flag[n - 2] = 1;
	}
	return ((2.0 * n - 1.0) * x * P[n - 1] - (n - 1.0) * P[n - 2]) / n;
}

int main() {
	int n;
	scanf("%lf", &x);
	scanf("%d", &n);
	printf("%.2lf\n", calP(n));
}
