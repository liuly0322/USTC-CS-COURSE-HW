#include <stdio.h>
int num[1020][1020];
int main() {
	int n, win_size, stride, padding;
	scanf("%d%d%d%d", &n, &win_size, &stride, &padding);
	for (int i = padding; i <= padding + n - 1; i++) {
		for (int j = padding; j <= padding + n - 1; j++) {
			scanf("%d", &num[i][j]);
		}
	}
	//看末尾：win_size-1 到 stride+win_size-1 到 2*stride+win_size-1
	//到末尾 超过 2*padding+n-1 为止 k*stride+win_size>2*padding+n
	//所以最后一个 k 恰好是 (n+2*padding-win_size)/stride
	int k = (n + 2 * padding - win_size) / stride;
	for (int i = 0; i <= k; i++) {
		for (int j = 0; j <= k; j++) {
			int x_max = i * stride + win_size - 1, y_max = j * stride + win_size - 1;
			int x_min = i * stride, y_min = j * stride;
			int max = 0;
			for (int k = x_min; k <= x_max; k++) {
				for (int l = y_min; l <= y_max; l++) {
					if (num[k][l] > max) max = num[k][l];
				}
			}
			if (j != k) printf("%d ", max);
			else printf("%d", max);
		}
		printf("\n");
	}
}
