#include <stdio.h>
int main(){
	int x[1001]={0};
	int i=2,j=2,k=2;
	for(i=2;i<34;i++){
		for(j=2*i;j<1000;j+=i){
			x[j]=1;
		}
	}
	for(k=2;k<1000;k++){
		if(x[k]==0){
			printf("%d ",k);
		}
	}
	return 0; 
} 
