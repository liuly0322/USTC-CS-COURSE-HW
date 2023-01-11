#include<stdio.h>
#include"lib.h"
/* Input & output functions */
int getint(){int t; scanf("%d",&t); return t; }
float getfloat(){float t; scanf("%f",&t); return t; }
int getch(){char c; scanf("%c",&c); return (int)c; }
int getarray(int a[]){
  int n;
  scanf("%d",&n);
  for(int i=0;i<n;i++)scanf("%d",&a[i]);
  return n;
}
int getfarray(float a[]){
  int n;
  scanf("%d",&n);
  for(int i=0;i<n;i++)scanf("%f",&a[i]);
  return n;
}
void putint(int a){ printf("%d",a); }
void putfloat(float a){ printf("%f",a); }
void putch(int a){ printf("%c",a); }
void putarray(int n, int a[]){
  printf("%d:",n);
  for(int i=0;i<n;i++)printf(" %d",a[i]);
  printf("\n");
}
void putfarray(int n, float a[]){
  printf("%d:",n);
  for(int i=0;i<n;i++)printf(" %f",a[i]);
  printf("\n");
}
