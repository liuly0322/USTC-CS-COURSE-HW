//����׳˺����ĸ���
#include <stdio.h>
int main(){
	int n,sum=0;
	scanf("%d",&n);
	while(n!=0){
		n/=5;
		sum+=n;
	}
	printf("%d",sum);
	return 0;
}
