//����M��N�Ļ������������߽磩
#include <stdio.h>
int main(){
	int isCorrect(int x);
	int M,N,sum=0,i;
	scanf("%d%d",&M,&N);
	for (i=M;i<=N;i++) {
		if (isCorrect(i)==1) {
			sum+=i;
		}
	}
	printf("%d",sum);
	return 0;
} 

int isCorrect(int x) {
	int a[10];
	int i,t;
	for (i=0;x!=0;i++) {
		a[i]=x%10;
		x=x/10;
	}
	//i��λ������Ӧa[0]��a[i-1] 
	for (t=0;t<=i/2;t++) {
		if (a[t]!=a[i-1-t]) {
			return 0;
		}
	}
	return 1;
}
