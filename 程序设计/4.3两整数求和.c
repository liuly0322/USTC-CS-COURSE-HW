//异或操作相当于不进位的加法 
#include <stdio.h>
int main(){
	int x,y,sum;
	scanf("%d%d",&x,&y);
	sum=x;
	while(y!=0){
		sum=x^y;
		y=(x&y)<<1;
		x=sum;
    }
    printf("%d",sum);
    return 0;
} 
