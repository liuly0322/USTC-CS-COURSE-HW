#include <stdio.h>
//#include <windows.h>
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
	//接下来分别排序
	int maxInfo, max;
	for (int i = 0; i < count1 - 1; i++) {
		maxInfo = i;
		max = row1[i];
		for (int j = i + 1; j <= count1 - 1; j++) {
			if (row1[j] > max) {
				maxInfo = j;
				max = row1[j];
			}
		}
		temp = row1[i];
		row1[i] = row1[maxInfo];
		row1[maxInfo] = temp;
	}
	for (int i = 0; i < count2 - 1; i++) {
		maxInfo = i;
		max = row2[i];
		for (int j = i + 1; j <= count2 - 1; j++) {
			if (row2[j] > max) {
				maxInfo = j;
				max = row2[j];
			}
		}
		temp = row2[i];
		row2[i] = row2[maxInfo];
		row2[maxInfo] = temp;
	}
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
	//system("pause");
	return 0;
}

