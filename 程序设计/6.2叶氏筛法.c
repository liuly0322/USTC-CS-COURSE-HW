#include <stdio.h>
#include <math.h>
char num[50000001] = { 0 };
int main() {
	int N, sqrtN;
	scanf("%d", &N);
	sqrtN = sqrt(N);
	if (N >= 2) printf("2\n");
	for (int i = 3; i <= sqrtN + 2; i += 2) {
		if (num[i] == 0) {
			printf("%d\n", i);
			for (int j = 2 * i; j <= N; j += i) {
				num[j] = 1;
			}
		}
	}
	if (sqrtN % 2 == 1)sqrtN += 1;
	for (int i = sqrtN + 3; i <= N; i+=2) {
		if (num[i] == 0) {
			printf("%d\n", i);
		}
	}
	return 0;
}
