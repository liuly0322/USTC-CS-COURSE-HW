#include <stdio.h>
int mark[101][101] = { {0} };   //��ǣ�0��ɫ��1��ɫ��2��ɫ
int height[101][101] = { {0} };
int N, M;
int main() {
	int round = 0;
	int flag = 1;               //flag��ǵ�ǰ����û�н��в���
	scanf("%d%d", &N, &M);
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= M; j++) {
			scanf("%d", &height[i][j]);
		}
	}
	while (flag == 1) {
		flag = 0;
		//���ȰѺ�ɫ��ɻ�ɫ
		for (int i = 1; i <= N; i++) {
			for (int j = 1; j <= M; j++) {
				if (mark[i][j] == 0) {
					mark[i][j]++;
				}
			}
		}
		//Ȼ����ݻ�ɫȷ���µĺ�ɫ
		//����һ���У������в�������flag=1
		for (int i = 1; i <= N; i++) {
			for (int j = 1; j <= M; j++) {
				if (mark[i][j] == 1) {
					//��һ���У�������ڵ��ǻ�ɫ��Ҳ�ᱻ��ɺ�ɫ�����Ⲣ����Ӱ����
					//��һ��Ϊʲô��
					if (i > 1 && height[i][j] > height[i - 1][j]) {
						mark[i - 1][j] = 0;
						flag = 1;
					}
					if (j > 1 && height[i][j] > height[i][j - 1]) {
						mark[i][j - 1] = 0;
						flag = 1;
					}
					if (i < N && height[i][j] > height[i + 1][j]) {
						mark[i + 1][j] = 0;
						flag = 1;
					}
					if (j < M && height[i][j] > height[i][j + 1]) {
						mark[i][j + 1] = 0;
						flag = 1;
					}
				}
			}
		}
		//���ѻ�ɫ��ɰ�ɫ
		for (int i = 1; i <= N; i++) {
			for (int j = 1; j <= M; j++) {
				if (mark[i][j] == 1) {
					mark[i][j]++;
				}
			}
		}
		round++;
	}
	printf("%d", round);
}
