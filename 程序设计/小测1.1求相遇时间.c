#include <stdio.h>
int main(){
	int x,y,n;
	float ans;
	scanf("%d%d%d",&x,&y,&n);
	if(x+y<=0){
		printf("No answer");
		return 0;
	}
	ans=(n-0.5*x)/(1.0*x+1.0*y);
	if(ans<0){
		printf("0");
	}
	else{
		printf("%.2f",ans);
	}
	return 0;
}


