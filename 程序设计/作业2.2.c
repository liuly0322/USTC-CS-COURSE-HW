#include <stdio.h>
#include <math.h>
int main(){
	double const d=300000.0,p=6000.0,r=0.01;
	double m;
	m=log(p/(p-d*r))/log(1+r);
	if ((int)(100*m)%10<=4) {
		printf("%.1lf",m);
	}
	else {
		printf("%.1lf",((int)(10*m)+1)/10.0);
	}
	return 0;
}
