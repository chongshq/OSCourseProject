#include<stdio.h>
void main()
{
float a,b;
char d;
printf("请输入两个数a,b:");
scanf("%f,%f"&a,&b);
printf("请输入符号d");
scanf("%c",d);
switch(d)
{
case'+'：printf("%f\n,a+b);break;
case'-'：printf("%f\n,a-b);break;
case'*'：printf("%f\n,a*b);break;
case'/'：printf("%f\n,a/b);break;
default:printf("input error\n");
}
}