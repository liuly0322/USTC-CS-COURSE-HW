#include <stdio.h>
#include <stdlib.h>
int num[31], target, size, count = 0;

int comp(const void* p1, const void* p2) {
	return *(int*)p1 - *(int*)p2;
}

void cal(int n, int sum) {
	if (sum > target) return;
	int theLastSame = n, sameNumber = 1;
	for (int i = n + 1; i <= size; i++) {
		if (num[i] != num[n]) {
			break;
		}
		theLastSame += 1;
		sameNumber += 1;
	}
	if (theLastSame == size) {
		for (int i = 0; i <= sameNumber; i++) {
			if (sum + num[n] * i == target) count++;
		}
		return;
	}
	for (int i = 0; i <= sameNumber; i++) {
		cal(theLastSame + 1, sum + num[n] * i);
	}
}

int main() {
	scanf("%d", &size);
	for (int i = 1; i <= size; i++) {
		scanf("%d", &num[i]);
	}
	scanf("%d", &target);
	qsort(num + 1, size, sizeof(int), comp);
	cal(1, 0);
	printf("%d", count);
}
