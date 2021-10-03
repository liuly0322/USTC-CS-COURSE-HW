//改进思考：边计算边比较 
#include <stdio.h>
int main(){
	long long x,t;
	int i,j,max=1,n,temp=1;
	char c[20]={0};
	scanf("%lld",&x);
	t=x;
	if(x==0){
		printf("0 1");
		return 0;
	}
	for(i=1;t>=10;i++){
		t=t/10;
	}
	//i是位数,t是最高位数字 
	c[i]=t;
	for(j=1;j<=i-1;j++){
		c[j]=x%10;
		x=x/10;
	}
	n=c[j];
	for(j=i-1;j>=1;j--){
		if(c[j]==c[j+1]){
			temp+=1;
		}
		else{
			if(temp>max){
				n=c[j+1];
				max=temp;
			}
			temp=1;
		}
	}
	if(temp>max){
		n=c[1];max=temp;
	}
	printf("%d %d",n,max);
	return 0;
}
