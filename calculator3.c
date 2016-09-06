#include <stdio.h>
void main()
{
 int a,b,c;
 char ch,ch1;
 printf("请输入表达式如 5+6= 然后按回车键:");
 scanf("%d%c%d%c",&a,&ch,&b,&ch1);
 switch(ch)
 {
  case '+':
     c=a+b;
     printf("%d+%d=%d\n",a,b,c);
     break;
  case '-':
     c=a-b;
     printf("%d-%d=%d\n",a,b,c);
     break;
  case '*':
     c=a*b;
     printf("%d*%d=%d\n",a,b,c);
     break;
     case '/':
     c=a/b;
     printf("%d/%d=%d\n",a,b,c);
     break;
  default:
     printf("输入表达式错误或该计算器不具备 %ch 功能\n",ch);
  
 }
}