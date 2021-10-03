#include <stdio.h>

void qSort(int* a, int length) {
	if (length <= 1) {
		return;
	}
	int ref = *a, i = 0, j = length - 1, temp;
	while (i != j) {
		while (j > i) {
			if (*(a+j) > ref) {
				break;
			}
			j--;
		}
		if (i == j) {
			break;
		}
		while (i < j) {
			if (*(a+i) < ref) {
				break;
			}
			i++;
		}
		if (i == j) {
			break;
		}
		temp = *(a+i); *(a + i) = *(a + j); *(a + j) = temp;
		j--;
	}
	temp = *a; *a = *(a + i); *(a + i) = temp;
	qSort(a, i);
	qSort(a + i + 1, length - i - 1);
	return;
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
	qSort(row1, count1);
	qSort(row2, count2);
	for (int i = 0; i < count2 - 1; i++) {
		printf("%d ", *(row2 + i));
	}
	if (count2 != 0) {
		printf("%d", *(row2 + count2 - 1));
	}
	printf("\n");
	for (int i = 0; i < count1 - 1; i++) {
		printf("%d ", *(row1 + i));
	}
	if (count1 != 0) {
		printf("%d", *(row1 + count1 - 1));
	}
	return 0;
}