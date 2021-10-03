#include <stdio.h>

int cal(int n) {
	if (n == 2) return 4;
	return(cal(n - 1) * 2 + 2);
}

int main() {
	printf("%d", cal(10));
}