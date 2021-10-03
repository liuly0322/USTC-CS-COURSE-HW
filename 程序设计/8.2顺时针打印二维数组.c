#include <stdio.h>
int num[101][101] = { {0} };
void print(int* a, int m, int n) {
	int* b = a;
	if (m <= 0 || n <= 0) {
		return;
	}
	//´òÓ¡
	if (m == 1) {
		for (int i = 1; i <= n; i++) {
			printf("%d ", *a);
			a += 1;
		}
		return;
	}
	if (n == 1) {
		for (int i = 1; i <= m; i++) {
			printf("%d ", *a);
			a += 101;
		}
		return;
	}
	for (int i = 1; i <= n; i++) {
		printf("%d ", *a);
		a += 1;
	}
	a -= 1;
	for (int i = 1; i < m; i++) {
		a += 101;
		printf("%d ", *a);
	}
	for (int i = 1; i < n; i++) {
		a -= 1;
		printf("%d ", *a);
	}
	for (int i = 1; i < m - 1; i++) {
		a -= 101;
		printf("%d ", *a);
	}
	//µİ¹é
	print(b + 102, m - 2, n - 2);
}
int main() {
	int M, N;
	scanf("%d%d", &M, &N);
	for (int i = 1; i <= M; i++) {
		for (int j = 1; j <= N; j++) {
			scanf("%d", &num[i][j]);
		}
	}
	print(&num[1][1], M, N);
	return 0;
}
