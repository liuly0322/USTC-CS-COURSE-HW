#include <stdio.h>
int money[169][169] = { {0} };
int main() {
	int d, n, xtemp, ytemp, max = 0, count = 0, temp = 0;
	short int flagY[169] = { 0 };
	scanf("%d", &d);
	scanf("%d", &n);
	for (int i = 1; i <= n; i++) {
		scanf("%d", &xtemp);
		scanf("%d", &ytemp);
		scanf("%d", &money[xtemp + d][ytemp + d]);
		flagY[ytemp + d] = 1;
	}
	for (int i = d; i <= 128 + d; i++) {
		if (flagY[i]) {
			for (int j = 0; j <= 2 * d - 1; j++) {
				temp += money[j][i];
			}
			for (int j = 0; j <= 128; j++) {
				temp = temp - money[j][i] + money[j + 2 * d][i];
				money[j][i] += temp;
			}
			temp = 0;
		}
	}
	for (int i = 0; i <= 128; i++) {
		for (int j = 0; j <= 2 * d - 1; j++) {
			temp += money[i][j];
		}
		for (int j = 0; j <= 128; j++) {
			temp = temp - money[i][j] + money[i][j + 2 * d];
			money[i][j] += temp;
		}
		temp = 0;
	}
	for (int i = 0; i <= 128; i++) {
		for (int j = 0; j <= 128; j++) {
			if (money[i][j] > max) {
				count = 1; max = money[i][j];
			}
			else if (money[i][j] == max) {
				count += 1;
			}
		}
	}
	printf("%d %d", count, max);
}
