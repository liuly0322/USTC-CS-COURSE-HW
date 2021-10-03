#include <stdio.h>
#include <math.h>
int main(){
	int i;
	float x[3],y[3];
	for(i=0;i<=2;i++){
		scanf("%f %f",&x[i],&y[i]);
	}
	printf("%.2f",fabs(0.5*((x[1]-x[0])*(y[2]-y[0])-(y[1]-y[0])*(x[2]-x[0]))));
	return 0;
}
