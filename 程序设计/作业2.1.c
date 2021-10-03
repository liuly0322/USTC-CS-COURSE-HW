#include <stdio.h>
#include <math.h>
#define money 1000.0
int main(){
	const double r1=0.015,r2=0.021,r3=0.0275,r5=0.03,r=0.0035;
	printf("%.4lf\n",money*(1+5*r5));
	printf("%.4lf\n",money*(1+2*r2)*(1+3*r3));
	printf("%.4lf\n",money*(1+2*r2)*(1+3*r3));
	printf("%.4lf\n",money*pow(1+r1,5));
	printf("%.4lf",money*pow(1+r/4.0,20));
	return 0;
}
