#include <stdio.h>
#include <stdlib.h>

int comp(const void* p1, const void* p2) {
	return *(int*)p2 - *(int*)p1;
}

int main() {
	int n, temp, row1[10001], row2[10001], count1 = 0, count2 = 0;
	scanf("%d", &n);
	for (int i = 1; i <= n; i++) {
		scanf("%d", &temp);
		if (temp % 2 == 1) {
			row1[count1] = temp;
			count1 += 1;
		}
		else {
			row2[count2] = temp;
			count2 += 1;
		}
	}
	qsort(row1, count1, sizeof(int), comp);
	qsort(row2, count2, sizeof(int), comp);
	for (int i = 0; i < count2 - 1; i++) {
		printf("%d ", row2[i]);
	}
	if (count2 != 0) {
		printf("%d", row2[count2 - 1]);
	}
	printf("\n");
	for (int i = 0; i < count1 - 1; i++) {
		printf("%d ", row1[i]);
	}
	if (count1 != 0) {
		printf("%d", row1[count1 - 1]);
	}
	return 0;
}
