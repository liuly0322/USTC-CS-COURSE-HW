// 10.30 一种非递归的快速排序实现

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <stack>
using namespace std;
int a[100000], n;

void quick_sort(int* a, int length) {
    stack<int> left, right;
    left.push(0), right.push(length - 1);
    while (!left.empty()) {
        bool exchange = false;  // 交换标记
        int l = left.top(), r = right.top();
        left.pop(), right.pop();
        if (r - l <= 2) {  // 判断长度
            if (r == l + 1) {
                if (a[l] > a[r]) {
                    swap(a[l], a[r]);
                }
            }
            if (r == l + 2) {
                if (a[l] > a[l + 1])
                    swap(a[l], a[l + 1]);
                if (a[l] > a[r])
                    swap(a[l], a[r]);
                if (a[l + 1] > a[r])
                    swap(a[l + 1], a[r]);
            }
            continue;
        }
        int i = l, j = r;
        int pivot = a[(i + j) / 2];
        while (i <= j) {
            while (a[i] < pivot) {
                i++;
            }
            while (a[j] > pivot) {
                j--;
            }
            if (i > j) {
                break;
            }
            swap(a[i], a[j]), i++, j--;
            exchange = true;
        }
        if (!exchange)
            continue;
        // 压栈，先压长的，再压短的
        if (j - l + 1 > r - i + 1) {
            left.push(l), right.push(j);
            left.push(i), right.push(r);
        } else {
            left.push(i), right.push(r);
            left.push(l), right.push(j);
        }
    }
}

int main() {
    cin >> n;
    for (int i = 0; i < n; i++) {
        scanf("%d", a + i);
    }
    quick_sort(a, n);
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
}