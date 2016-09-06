#include<stdio.h>
#include<conio.h>
#include<math.h>
/*****************************************/
/*  将数字字符转化成浮点型实数进行计算   */
/*                                       */
/*****************************************/
double readnum(char f[],int*i)
{
  double x=0.0;
  int k=0;
  while(f[*i]>='0'&&f[*i]<='9')
  {
    x=x*10+(f[*i]-'0');
    (*i)++;
  }
  if(f[*i]=='.')
  {
    (*i)++;
    while(f[*i]>='0'&&f[*i]<='9')
    {
      x=x*10+(f[*i]-'0');
      (*i)++;
      k++;
    }
  }
  while(k-->0)
  {
    x=x/10.0;
    }
  return (x);
}
/*******************************/
/*  计算后缀表达式的值         */
/*******************************/
double evalpost(char f[])
{
  double obst[10];
  int top=0;
  int i=0;
  double x1,x2;
  while(f[i]!='=')
  {
    if(f[i]>='0'&&f[i]<='9')
    { obst[top]=readnum(f,&i);top++;}
    else if(f[i]==' ')
      i++;
    else if(f[i]=='+')
    {
      x1=obst[--top];
      x2=obst[--top];
      obst[top]=x1+x2;
      i++;
      top++;
    }
    else if(f[i]=='-')
    {
      x1=obst[--top];
      x2=obst[--top];
      obst[top]=x2-x1;
      i++;
      top++;
    }
    else if(f[i]=='*')
    {
      x1=obst[--top];
      x2=obst[--top];
      obst[top]=x1*x2;
      i++;
      top++;
    }
    else if(f[i]=='/')
    {
      x1=obst[--top];
      x2=obst[--top];
      obst[top]=x2/x1;
      i++;
      top++;
    }
      }
  return obst[0];
}
/***********************************/
/*       判断字符是否为操作字符    */
/***********************************/
int is_operation(char op)
{
  switch(op)
  {
  case'^':
  case'K':
  case'+':
  case'-':
  case'*':
  case'/': return 1;
  default: return 0;
  }
}
/*****************************/
/*    判断字符的优先级       */
/*****************************/
int priority(char op)
{
  switch(op)
  {
  case'=': return -1;
  case'(': return 0;
  case'+':
  case'-': return 1;
  case'*':
  case'/': return 2;
  default: return -1;
  }
}
/******************************/
/*  中缀表达式转化成后缀表达式*/
/******************************/
void postfix(char e[],char f[])
{
  int i=0,j=0,k=0;
  char opst[100];
  int top=0;
  opst[0]='=';top++;
  while(e[i]!='=')
  {
    if((e[i]>='0'&&e[i]<='9')||e[i]=='.')
      f[j++]=e[i];
    else if(e[i]=='(')
    { opst[top]=e[i];top++;}
    else if(e[i]==')')
    {
      k=top-1;
      while(opst[k]!='(') {f[j++]=opst[--top];k=top-1;}
      top--;
    }
    else if(is_operation(e[i]))
    {
      f[j++]=' ';
      while(priority(opst[top-1])>=priority(e[i]))
      f[j++]=opst[--top];
      opst[top]=e[i];
      top++;
    }
    i++;
  }
  while(top) f[j++]=opst[--top];f[j]='\0';
}
void print_1()
  {  printf("|_______________________________________ |                            \n");
       printf("||                                     | |                            \n");
       printf("||        欢迎使用多功能计算器         | |         本计算器能够进行   \n");
       printf("||_____________________________________| |      ＋，－，×，÷，\n");
       printf("||                    图案仅供参考     | |        （）             \n");
       printf("||_____________________________________| |              \n");
       printf("|                                        |                         \n");
       printf("|___  ___  ___  ___  ___  ___  ___  ___  |                            \n");
       printf("|________  ________  ________  ________  |                            \n");
       printf("||  ⑨  |  |  ⑧  |  |  ⑦  |  |  ×  |  |                            \n");
       printf("||______|  |______|  |______|  |______|  |                            \n");
       printf("|________  ________  ________  ________  |                            \n");
       printf("||  ⑥  |  |  ⑤  |  |  ④  |  |  －  |  |                            \n");
       printf("||______|  |______|  |______|  |______|  |                            \n");
       printf("| _______  ________  ________  ________  |                            \n");
       printf("||  ③  |  |  ②  |  |  ①  |  |  ＋  |  |                            \n");
       printf("||______|  |______|  |______|  |______|  |                            \n");
       printf("|________  ________  ________  ________  |                            \n");
       printf("||  〇  |  |  ＝  |  |  AC  |  |  ÷  |  |                            \n");
       printf("||______|  |______|  |______|  |______|  |                            \n");

   getch();
   system("cls");
}
void printf_2()
{system("cls");
 printf("\n\n\n\n\n\n\n\n\t\t\t ##############################\n");
    printf("\t\t\t #                            #\n");
    printf("\t\t\t #----------谢谢使用----------#\n");
   printf("\t\t\t #                            #\n");
   printf("\t\t\t ##############################\n");
   printf("\t\t\t                      --XXXXXX制作\n                                 ");
}
/****************/
/*   转化   */
/****************/
void zhuanhuan(char g[],char e[])
{
  int k,i,j=0;
  for(i=0;g[i]!='=';i++)
  {
    k=i+1;
    if(g[i]=='('&&g[k]=='-')
    {
      e[j++]=g[i];
      e[j++]='0';

    }
    else e[j++]=g[i];
  }

  e[j]='=';
}
int main()
{
    int wei;
         char e[100],f[100],g[100];
     int sign;int flag;
    print_1();
     do
     {
       printf("输入所要经计算的表达式（如：a*b/(c-d)=）：\n");
       scanf("%s",g);
       zhuanhuan(g,e);
       postfix(e,f);
       printf("输出保留几位小数：\n");
       scanf("%d",&wei);
       printf("%.*lf\n",wei,evalpost(f));
       while(1)
       { flag=3 ;
       printf("继续计算/退出？1/0?");
           sign=getch();
       printf("%c\n",sign);
       switch(sign)
       {

        case '1':flag=1;getch();break;

      case '0':flag=0;getch();break;
      default: printf("非法输入，请重新输入：\n");

       }
       if(flag==1||flag==0)break;
       }
     }while(flag==1);
    printf_2();

return 0;
}
