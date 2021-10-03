#include <stdio.h>
#include <math.h>

double a;

double cal(double x) {
	return((a/x+x)/2);
}

int main() {
	double x, y;
	scanf("%lf", &a);
	y = a;
	do {
		x = y;
		y = cal(x);
	} while (fabs(x - y) >= 1e-5);
	printf("%.4lf", x);
	return 0;
}
