/*
* һ��N x N������(grid) ������һ��յأ�ÿ�������������������ֵ�һ������ʾ��
* 0 ��ʾ��������ǿյģ���������Դ�������
* 1 ��ʾ������������һö��ң�����Լ�����Ȼ�󴩹�����
* -1 ��ʾ����������о�ʯ���������·��
* ������������������й��������£��񵽾����ܶ�Ľ�ң�
* ��λ�� (0, 0) ��������󵽴� (N-1, N-1) ��ֻ�����»������ߣ�����ֻ�ܴ�Խ��Ч�ĸ��ӣ���ֻ���Դ���ֵΪ0����1�ĸ��ӣ���
* ������ (N-1, N-1) ����Ҫ�����ߣ�ֱ�����ص� (0, 0) ��ֻ�����ϻ������ߣ�����ֻ�ܴ�Խ��Ч�ĸ��ӣ�
* ���㾭��һ��������������Ӱ���һ�����ʱ���㽫�����Ҳ���������ӻ��ɿյģ�ֵ��Ϊ0����
* ����� (0, 0) �� (N-1, N-1) ֮�䲻����һ���ɾ�����·��������Ϊ���޷�����κν�ҡ�
*/

#include <stdio.h>
#define MIN(x,y) (x)<(y)?(x):(y)
#define MAX(x,y) (x)>(y)?(x):(y)
#define MAX_MAP 250

int main() {

	int n = 0;	//size of the maze
	char c;	//for read
	static int dp[2 * MAX_MAP - 1][MAX_MAP][MAX_MAP];	//(sum,row_1,row_2)
	static char mark[MAX_MAP][MAX_MAP];	//if coin or blocked

	while ((c = getchar()) != '\n') {	//read
		if (c != ' ') {
			ungetc(c, stdin);
			scanf("%hhd", &mark[0][n++]);
		}
	}
	for (int i = 1; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			scanf("%hhd", &mark[i][j]);
		}
	}

	for (int sum = 2 * n - 2; sum >= 0; sum--) {
		int x_min = MAX(0, sum - n + 1), x_max = MIN(sum, n - 1);
		for (int row_1 = x_min; row_1 <= x_max; row_1++) {
			for (int row_2 = x_min; row_2 <= x_max; row_2++) {
				if (mark[row_1][sum - row_1] < 0 || mark[row_2][sum - row_2] < 0) {	//if blocked
					dp[sum][row_1][row_2] = -1;
					continue;
				}
				if (row_1 > row_2) {	//as symmetry
					dp[sum][row_1][row_2] = dp[sum][row_2][row_1];
					continue;
				}

				dp[sum][row_1][row_2] = -1;	//dp calculation
				if (row_1 != n - 1 && row_2 != n - 1) {
					dp[sum][row_1][row_2] = MAX(dp[sum][row_1][row_2], dp[sum + 1][row_1 + 1][row_2 + 1]);
				}
				if (row_1 != n - 1 && sum - row_2 != n - 1) {
					dp[sum][row_1][row_2] = MAX(dp[sum][row_1][row_2], dp[sum + 1][row_1 + 1][row_2]);
				}
				if (sum - row_1 != n - 1 && sum - row_2 != n - 1) {
					dp[sum][row_1][row_2] = MAX(dp[sum][row_1][row_2], dp[sum + 1][row_1][row_2]);
				}
				if (sum - row_1 != n - 1 && row_2 != n - 1) {
					dp[sum][row_1][row_2] = MAX(dp[sum][row_1][row_2], dp[sum + 1][row_1][row_2 + 1]);
				}
				if (sum == 2 * n - 2) {
					dp[sum][row_1][row_2] = 0;
				}

				if (dp[sum][row_1][row_2] == -1) {	//if coin
					continue;
				}
				if (row_1 == row_2) {
					dp[sum][row_1][row_2] += mark[row_1][sum - row_1];
				}
				else {
					dp[sum][row_1][row_2] += mark[row_1][sum - row_1] + mark[row_2][sum - row_2];
				}
			}
		}
	}

	printf("%d", MAX(dp[0][0][0], 0));
}