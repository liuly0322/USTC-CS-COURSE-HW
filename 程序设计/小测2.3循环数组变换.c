#include <stdio.h>
int main() {
	int n, oplen, num[55];
	char op;
	scanf("%d%d", &n, &oplen);
	for (int i = 1; i <= n; i++) {
		scanf("%d", num + i);
	}
	getchar();
	for (int i = 1; i <= oplen; i++) {
		op = getchar();
		if (op == '1') {
			num[0] = num[n];
			for (int j = n; j >= 1; j--) {
				num[j] = (num[j] + num[j - 1]) % 100;
			}
		}
		else {
			num[n + 1] = num[1];
			for (int j = 1; j <= n; j++) {
				num[j] = (num[j] + num[j + 1]) % 100;
			}
		}
	}
	for (int i = 1; i <= n; i++) {
		printf("%d\n", num[i]);
	}
}

