/*
 * 一个 N x N 的网格 (grid) 代表了一块空地，每个格子由以下三种数字的一种来表示：
 * 0 表示这个格子是空的，所以你可以穿过它。
 * 1 表示这个格子里放了一枚金币，你可以捡起金币然后穿过它。
 * -1 表示这个格子里有巨石，挡着你的路。
 * 你的任务是在遵守下列规则的情况下，捡到尽可能多的金币：
 * 从位置 (0, 0) 出发，最后到达 (N-1, N-1)
 * ，只能向下或向右走，并且只能穿越有效的格子（即只可以穿过值为 0 或者 1
 * 的格子）；当到达 (N-1, N-1) 后，你要继续走，直到返回到 (0, 0)
 * ，只能向上或向左走，并且只能穿越有效的格子；
 * 当你经过一个格子且这个格子包含一个金币时，你将捡起金币并且这个格子会变成空的（值变为
 * 0）；如果在 (0, 0) 和 (N-1, N-1)
 * 之间不存在一条可经过的路径，则认为你无法获得任何金币。
 */

#include <stdio.h>
#define MIN(x, y) (x) < (y) ? (x) : (y)
#define MAX(x, y) (x) > (y) ? (x) : (y)
#define MAX_MAP 250

int main() {

    int n = 0;                                        // size of the maze
    char c;                                           // for read
    static int dp[2 * MAX_MAP - 1][MAX_MAP][MAX_MAP]; //(sum,row_1,row_2)
    static char mark[MAX_MAP][MAX_MAP];               // if coin or blocked

    while ((c = getchar()) != '\n') { // read
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
                if (mark[row_1][sum - row_1] < 0 ||
                    mark[row_2][sum - row_2] < 0) { // if blocked
                    dp[sum][row_1][row_2] = -1;
                    continue;
                }
                if (row_1 > row_2) { // as symmetry
                    dp[sum][row_1][row_2] = dp[sum][row_2][row_1];
                    continue;
                }

                dp[sum][row_1][row_2] = -1; // dp calculation
                if (row_1 != n - 1 && row_2 != n - 1) {
                    dp[sum][row_1][row_2] =
                        MAX(dp[sum][row_1][row_2],
                            dp[sum + 1][row_1 + 1][row_2 + 1]);
                }
                if (row_1 != n - 1 && sum - row_2 != n - 1) {
                    dp[sum][row_1][row_2] = MAX(dp[sum][row_1][row_2],
                                                dp[sum + 1][row_1 + 1][row_2]);
                }
                if (sum - row_1 != n - 1 && sum - row_2 != n - 1) {
                    dp[sum][row_1][row_2] =
                        MAX(dp[sum][row_1][row_2], dp[sum + 1][row_1][row_2]);
                }
                if (sum - row_1 != n - 1 && row_2 != n - 1) {
                    dp[sum][row_1][row_2] = MAX(dp[sum][row_1][row_2],
                                                dp[sum + 1][row_1][row_2 + 1]);
                }
                if (sum == 2 * n - 2) {
                    dp[sum][row_1][row_2] = 0;
                }

                if (dp[sum][row_1][row_2] == -1) { // if coin
                    continue;
                }
                if (row_1 == row_2) {
                    dp[sum][row_1][row_2] += mark[row_1][sum - row_1];
                } else {
                    dp[sum][row_1][row_2] +=
                        mark[row_1][sum - row_1] + mark[row_2][sum - row_2];
                }
            }
        }
    }

    printf("%d", MAX(dp[0][0][0], 0));
}