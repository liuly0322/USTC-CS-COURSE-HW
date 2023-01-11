
#include "runtime/io.h"
 
const int buf_size = 4096;

void QuickSort(float *arr, int low, int high)
{
    if (low < high)
    {
        int i = low;
        int j = high;
        float k = arr[low];
        while (i < j)
        {
            while(i < j && arr[j] >= k)     // 从右向左找第一个小于k的数
            {
                j--;
            }
 
            if(i < j)
            {
                arr[i++] = arr[j];
            }
 
            while(i < j && arr[i] < k)      // 从左向右找第一个大于等于k的数
            {
                i++;
            }
 
            if(i < j)
            {
                arr[j--] = arr[i];
            }
        }
 
        arr[i] = k;
 
        // 递归调用
        QuickSort(arr, low, i - 1);     // 排序k左边
        QuickSort(arr, i + 1, high);    // 排序k右边
    }
}
 
// 主函数
int main()
{
    float array[buf_size] = {0};
    // 输入样例数目<=buf_size
    int array_num = getfarray(array);
    int start = current_time();
    QuickSort(array, 0, array_num-1);  // 快速排序
    int end = current_time();
    putfarray(array_num, array);
    putint(end - start);
    putch(' ');
    putch('m');
    putch('s');
    putch('\n');
    return 0;
}