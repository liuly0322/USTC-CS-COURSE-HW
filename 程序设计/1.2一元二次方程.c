#include <stdio.h>
#include <math.h>
int main(){
	int a,b,c;
	scanf("%d%d%d",&a,&b,&c);
	if(a==0){
		if(b==0){
			if(c==0){
				printf("x��ȡȫ��ʵ��");
			}
			else{
				printf("x�޽�");
			}
		}
		else{
			printf("%.2lf",-1.0*c/b);
		}
	}
	else{
		if(b*b-4*a*c<0){
			printf("x��ʵ����");
			return 0;
		}
		if(a>0)
		printf("%.2lf %.2lf",(-b-sqrt(b*b-4*a*c))/(2*a),(-b+sqrt(b*b-4*a*c))/(2*a));
		else
		printf("%.2lf %.2lf",(-b+sqrt(b*b-4*a*c))/(2*a),(-b-sqrt(b*b-4*a*c))/(2*a));
	}
	return 0;
}
