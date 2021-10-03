#include <stdio.h>
int main(){
	unsigned int n,s;
	scanf("%d",&n);
	s=n&(~n+1);
	printf("%d",s);
	return 0;
}


