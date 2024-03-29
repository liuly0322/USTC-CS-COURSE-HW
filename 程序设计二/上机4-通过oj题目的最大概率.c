/*
 * 小明只能挑取一些行整行抄取。但是提交的代码如果有连着两行完全一样，就会在代码查重环节被助教当场抓获并记零分。
 * 请你帮帮小明：在不被当场抓获的前提下，挑选哪些行复制粘贴，才能抄到最多字呢？
 * 输入是以空格分割的一系列整数，每个数字代表该行代码有多少字
 * 由于题目不难，代码不会超过 100
 * 行，但也不会少于一行。另外，由于他的室友严格遵守代码规范要求，每行的字数 n
 * 满足 0<=n<=80。
 * 输出为一个整数，代表在不会被助教当场抓获的情况下，小明最多能抄到多少字
 */

#include <stdio.h>
#define MAX(x, y) x > y ? x : y
int main() {
    int i, num[105] = {0};
    for (i = 3; scanf("%d", num + i) != EOF; i++) {
        num[i] += MAX(num[i - 3], num[i - 2]);
    }
    printf("%d", MAX(num[i - 2], num[i - 1]));
}