#include <stdio.h>
#include <math.h>
int main(){
	long int M,N,Sum=0,i,t,sum;
	int j,k;
	char c[15];
	scanf("%ld%ld",&M,&N);
	for(i=M;i<=N;i++){
		sum=0;
		j=1;
		for(t=i;t!=0;t=t/10){
			c[j]=t%10;j+=1;
		}
		j-=1;
		//�ӵ͵���������c[1]��c[j]��λ����jλ 
		for(k=1;k<=j;k++){
			sum+=pow(c[k],j);
		}
		if(sum==i){
			Sum+=i;
		}
	}
	printf("%ld",Sum);
	return 0;
}
