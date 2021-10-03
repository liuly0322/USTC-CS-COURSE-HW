#include <stdio.h>
char name[100001][15];
int main() {
	int N, M, j = 0, d, p, nowposi = 0;
	scanf("%d%d", &N, &M);
	getchar();
	for (int i = 0; i < N; i++) {
		j = 0;
		while ((name[i][j] = getchar()) != '\n') j++;
		name[i][j] = '\0';
	}
	for (int i = 0; i < M; i++) {
		scanf("%d%d", &d, &p);
		if ((name[nowposi][0] == '0' && d == 0 ) || (name[nowposi][0] == '1' && d == 1)) {
			nowposi = (nowposi - p) % N;
			if (nowposi < 0) nowposi += N;
		}
		else {
			nowposi = (nowposi + p) % N;
		}
	}
	printf((name[nowposi]) + 2);
}