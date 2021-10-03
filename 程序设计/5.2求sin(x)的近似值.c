#include <stdio.h>
#include <math.h>

int main() {
	double x, ans = 0, now;
	int count = 1;
	scanf("%lf", &x);
	now = x;
	while (fabs(now) >= 1e-5) {
		ans += now;
		now = now * x * x / (count + 1.0) / (count + 2.0) * (-1.0);
		count += 2;
	}
	printf("The approximate value of sin(x) is %.5lf", ans);
}
