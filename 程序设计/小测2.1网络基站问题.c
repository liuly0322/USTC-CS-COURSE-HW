#include <stdio.h>
//#include <windows.h>
int cover[1010][1010] = { { 0 } };
int main() {
	int m, n, x, y, r, count = 0;
	scanf("%d%d", &m, &n);
	while (1) {
		scanf("%d%d%d", &x, &y, &r);
		if (x == -1) {
			break;
		}
		//以x+2,y+2为中心
		for (int i = x + 2 - r; i <= x + 2 + r; i++) {
			for (int j = y + 2 - r; j <= y + 2 + r; j++) {
				cover[i][j] += 1;
			}
		}
	}
	for (int i = 3; i <= m + 2; i++) {
		for (int j = 3; j <= n + 2; j++) {
			if (cover[i][j] == 0) {
				count += 1;
			}
		}
	}
	printf("%d", count);
	//system("pause");
	return 0;
}

