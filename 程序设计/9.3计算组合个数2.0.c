#include <stdio.h>
int count = 0;
void cal(int sum, int target, int size, int nowSize, int smallestNum) {
	if (sum > target) {
		return;
	}
	if (nowSize == size + 1) {
		if (sum == target) {
			count++;
		}
		return;
	}
	for (int i = smallestNum; i <= 9; i++) {
		cal(sum + i, target, size, nowSize + 1, i + 1);
	}
}
int main() {
	int k, n;
	scanf("%d%d", &k, &n);
	cal(0, n, k, 1, 1);
	printf("%d", count);
}