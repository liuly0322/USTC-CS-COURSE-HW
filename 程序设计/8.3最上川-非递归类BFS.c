#include <stdio.h>
int mark[101][101] = { {0} };   //标记，0 黑色，1 灰色，2 白色
int height[101][101] = { {0} };
int N, M;
int main() {
	int round = 0;
	int flag = 1;               //flag 标记当前轮有没有进行操作
	scanf("%d%d", &N, &M);
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			scanf("%d", &height[i][j]);
		}
	}
	while (flag == 1) {
		flag = 0;
		//首先把黑色变成灰色
		for (int i = 1; i <= N; i++) {
			for (int j = 1; j <= M; j++) {
				if (mark[i][j] == 0) {
					mark[i][j]++;
				}
			}
		}
		//然后根据灰色确定新的黑色
		//在这一步中，若进行操作则令 flag=1
		for (int i = 1; i <= N; i++) {
			for (int j = 1; j <= M; j++) {
				if (mark[i][j] == 1) {
					//这一步中，如果相邻的是灰色，也会被变成黑色，但这并不会影响结果
					//想一想为什么？
					if (i > 1 && height[i][j] > height[i - 1][j]) {
						mark[i - 1][j] = 0;
						flag = 1;
					}
					if (j > 1 && height[i][j] > height[i][j - 1]) {
						mark[i][j - 1] = 0;
						flag = 1;
					}
					if (i < N && height[i][j] > height[i + 1][j]) {
						mark[i + 1][j] = 0;
						flag = 1;
					}
					if (j < M && height[i][j] > height[i][j + 1]) {
						mark[i][j + 1] = 0;
						flag = 1;
					}
				}
			}
		}
		//最后把灰色变成白色
		for (int i = 1; i <= N; i++) {
			for (int j = 1; j <= M; j++) {
				if (mark[i][j] == 1) {
					mark[i][j]++;
				}
			}
		}
		round++;
	}
	printf("%d", round);
}
