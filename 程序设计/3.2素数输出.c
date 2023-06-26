//改进思考：已被筛去的数不需要再筛一遍 
#include <stdio.h>
#include <stdbool.h>
int main(){
	bool x[1000001]={0};
	int i=2,j=2,n,m,count=0;
	long int sum=0;
	scanf("%d%d",&n,&m);
	for(i=3;i<1010;i+=2){
		for(j=2*i;j<1000000;j+=i){
			x[j]=1;
		}
	}
	if(n<=2){
		count+=1;sum+=2;n=2;
	}
	n=n/2*2;
	//保证 n 是二的倍数 
	for(i=n+1;i<=m;i+=2){
		if(x[i]==0){
			count+=1;sum+=i;
		}
	}
	printf("%d %ld",count,sum);
	return 0; 
} 
