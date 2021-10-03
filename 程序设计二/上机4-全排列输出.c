/*
* 给定一没有重复数字的序列，返回其所有可能的全排列。
* 输出需要按照输入的从前到后的顺序排列。具体参考样例数据。
* 输入是用空格分割的几个数字，数字个数n满足1 <= n <= 9.
* 输出你的排列，每行一个，按照从小到大顺序。
*/

#include <stdio.h>

void range(int* num, int depth, unsigned int now, int n, int* op) {
	if (depth == n) {
		for (int i = 0; i < n; i++) {
			putchar('0' + op[i]);
		}
		putchar('\n');
		return;
	}
	for (int i = 0; i < n; i++) {
		if (now & (1 << i)) {
			continue;
		}
		op[depth] = num[i];
		range(num, depth + 1, now | (1 << i), n, op);
	}
}

int main() {
	int num[10], op[10] = { 0 }, n;
	for (n = 0; scanf("%d", num + n) != EOF; n++);
	range(num, 0, 0, n, op);
}