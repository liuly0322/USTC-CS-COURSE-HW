<div style="text-align:center;font-size:2em;font-weight:bold">数据结构 hw14</div>

<div style="text-align:center">刘良宇 PB20000180</div>

**10.30** 为了使得不交换时可以直接跳出，考虑 pivot 元素直接取数组中间元素即可。代码如下：

```cpp
void quick_sort(int* a, int length) {
    stack<int> left, right;
    left.push(0), right.push(length - 1);
    while (!left.empty()) {
        bool exchange = false;  // 交换标记
        int l = left.top(), r = right.top();
        left.pop(), right.pop();
        if (r - l <= 2) {  		// 判断长度
            if (r == l + 1 && a[l] > a[r]) {
                swap(a[l], a[r]);
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
        int i = l, j = r, pivot = a[(i + j) / 2];
        while (i <= j) {
            while (a[i] < pivot) i++;
            while (a[j] > pivot) j--;
            if (i > j) {
                break;
            }
            swap(a[i], a[j]), i++, j--;
            exchange = true;
        }
        if (!exchange)
            continue;
        if (j - l + 1 > r - i + 1) {		// 先压长的，再压短的
            left.push(l), right.push(j);
            left.push(i), right.push(r);
        } else {
            left.push(i), right.push(r);
            left.push(l), right.push(j);
        }
    }
}
```
