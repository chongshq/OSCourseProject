#include <stdio.h>
 #include <stdlib.h>
 
 int main(void)
 {
 int choice;//选择
 double a,result;
 
 while(1)
 {
 result=0.0;
 printf("(1.加法 2.减法 3.乘法 4.除法 5.退出)\n输入你的选择:");
 scanf("%d",&choice);
 
 if(1 == choice)
 {
 printf("输入一个实数a:");
 while(1 == scanf("%lf",&a))
 result+=a;
 }
 
 else if(2 == choice)
 {
 int flag=1;
 printf("输入一个实数a:");
 while(1 == scanf("%lf",&a))
 {
 if(flag == 1)
 result+=a;
 else
 result-=a;
 flag=0;
 }
 }
 
 else if(3 == choice)
 {
 result=1.0;
 printf("输入一个实数a:");
 while(1 == scanf("%lf",&a))
 result*=a;
 }
 
 else if(4 == choice)
 {
 int flag=1;
 result=1.0;
 
 printf("输入一个实数a:");
 while(1 == scanf("%lf",&a))
 {
 if(flag == 1)
 result/=a;
 else
 result*=a;
 }
 }
 
 else if(5 == choice)
 {
 printf("最终结果:%.2f\n",result);
 break;
 }
 
 else
 printf("输入错误!\n");
 
 printf("最终结果:%.2f\n",result);
 system("pause");
 system("cls");
 getchar();
 }
 return 0;
 }
