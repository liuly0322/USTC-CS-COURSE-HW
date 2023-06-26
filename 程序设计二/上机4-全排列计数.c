/*
 * 给出集合 [1,2,3,…,n]，其所有元素共有 n! 种排列。
 * 按大小顺序列出所有排列情况，并一一标记，当 n = 3 时，所有排列如下：
 * “123”“132”“213”“231”“312”“321”
 * 给定 n 和 k，返回第 k 个排列。
 */

#include <stdio.h>
int main() {
    int num[10] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880};
    unsigned int mark = 0;
    int n, k;
    scanf("%d%d", &n, &k);
    for (int i = 0; i < n; i++) {
        int times = (k - 1) / num[n - i - 1] + 1;
        k = (k - 1) % num[n - i - 1] + 1;
        int posi;
        for (posi = 0; times; posi++) {
            if ((1 << posi) & mark) {
                continue;
            }
            times--;
        }
        putchar(posi + '0');
        mark = mark | (1 << (posi - 1));
    }
}