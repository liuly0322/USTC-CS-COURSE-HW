//This program is to convert a number to a string. 
#include <stdio.h>
void toStr(int n) {
	if ((n / 10) != 0) {
		toStr(n / 10);
	}
	printf("%d", n % 10);
}

int main() {
	int n;
	scanf("%d", &n);
	if (n < 0) {
		printf("-");
		n = -n;
	}
	toStr(n);
	return 0;
}
