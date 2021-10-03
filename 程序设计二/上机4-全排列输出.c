/*
* ����һû���ظ����ֵ����У����������п��ܵ�ȫ���С�
* �����Ҫ��������Ĵ�ǰ�����˳�����С�����ο��������ݡ�
* �������ÿո�ָ�ļ������֣����ָ���n����1 <= n <= 9.
* ���������У�ÿ��һ�������մ�С����˳��
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