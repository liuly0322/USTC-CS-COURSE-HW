#include <stdio.h>
#include <math.h>
double cal(double x) {
	return(2.0 * x * x * x - 4.0 * x * x + 3.0 * x - 6.0);
}

int main() {
	double a = -10, b = 10, m;
	while (fabs(a - b) >= 1e-7) {
		m = (a + b) / 2;
		if (cal(m) > 0) b = m;
		else if (cal(m) < 0) a = m;
		else { a = m; return 0; }
	}
	printf("%.6lf", a);
}
