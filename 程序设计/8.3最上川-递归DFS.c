#include <stdio.h>
int length[101][101] = { {0} };
int height[101][101] = { {0} };
int N, M;
int calLength(int i, int j) {
	int max = 0;
	if (length[i][j]) {
		return length[i][j];
	}
	if (i > 1 && height[i][j] > height[i - 1][j]) {
		if (calLength(i - 1, j) > max) {
			max = calLength(i - 1, j);
		}
	}
	if (j > 1 && height[i][j] > height[i][j - 1]) {
		if (calLength(i, j - 1) > max) {
			max = calLength(i, j - 1);
		}
	}
	if (i < N && height[i][j] > height[i + 1][j]) {
		if (calLength(i + 1, j) > max) {
			max = calLength(i + 1, j);
		}
	}
	if (j < M && height[i][j] > height[i][j + 1]) {
		if (calLength(i, j + 1) > max) {
			max = calLength(i, j + 1);
		}
	}
	length[i][j] = max + 1;
	return length[i][j];
}
int main() {
	int max = 0;
	scanf("%d%d", &N, &M);
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			scanf("%d", &height[i][j]);
		}
	}
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			if (calLength(i, j) > max) {
				max = calLength(i, j);
			}
		}
	}
	printf("%d", max);
}
