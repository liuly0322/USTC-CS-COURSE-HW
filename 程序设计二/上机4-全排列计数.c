/*
* �������� [1,2,3,��,n]��������Ԫ�ع��� n! �����С�
* ����С˳���г����������������һһ��ǣ��� n = 3 ʱ, �����������£�
* ��123����132����213����231����312����321��
* ���� n �� k�����ص� k �����С�
*/

#include <stdio.h>
int main() {
	int num[10] = { 1,1,2,6,24,120,720,5040,40320,362880 };
	unsigned int mark = 0;
	int n, k;
	scanf("%d%d", &n, &k);
	for (int i = 0; i < n; i++) {
		int times = (k - 1) / num[n - i - 1] + 1;
		k = (k - 1) % num[n - i - 1] + 1;
		int posi;
		for (posi = 0; times; posi++) {
			if ((1 << posi) & mark) {
				continue;
			}
			times--;
		}
		putchar(posi + '0');
		mark = mark | (1 << (posi - 1));
	}
}