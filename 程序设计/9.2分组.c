#include <stdio.h>
#include <stdlib.h>
int a[10001] = { 0 };
int sort(const void* p1, const void* p2) {
	return *(int*)p1 - *(int*)p2;
}
int main() {
	int size, flag = 0;
	scanf("%d", &size);
	for (int i = 1; i <= size; i++) {
		scanf("%d", &a[i]);
	}
	qsort(a + 1, size, sizeof(int), sort);
	//已经按递增顺序排好
	int numofSmallest = 1;
	for (int i = 2; i <= size; i++) {
		if (a[i] == a[1]) {
			numofSmallest++;
		}
		else {
			break;
		}
	}
	for (int i = 2; i <= numofSmallest; i++) {
		if (numofSmallest % i == 0 && size % i == 0) {
			//有必要判断i作为x能否成立
			int flagX = 0;
			for (int start = 1; start < size; start += i) {
				for (int j = 1; j <= i - 1; j++) {
					if (a[start] != a[start + j]) {
						flagX = 1;
						break;
					}
				}
			}
			if (flagX == 0) {
				flag = 1;
				break;
			}
		}
	}
	printf("%d", flag);
}