#include <stdio.h>
char token;
void match( char expectedToken ) /*对当前的标志进行匹配*/
{
if( token == expectedToken ) token = getchar(); /*匹配成功，获取下一个标志*/
else
{      
printf("cannot match\n");
exit(1); /*匹配不成功，退出程序*/
}
}
int low( void )/*用于计算表达式中级别最低的运算*/
{
int result = mid(); /*计算比加减运算优先级别高的部分*/
while(( token == '+' ) || ( token == '-' ))
if ( token == '+')
{
match('+');     /*进行加法运算*/
result += mid();
break;
}
else if ( token == '-')
{
match('-');    /*进行减法运算*/
result -= mid();
break;
}
return result;
}
int low( void )/*用于计算表达式中级别最低的运算*/
{
int result = mid(); /*计算比加减运算优先级别高的部分*/
while(( token == '+' ) || ( token == '-' ))
if ( token == '+')
{
match('+');     /*进行加法运算*/
result += mid();
break;
}
else if ( token == '-')
{
match('-');    /*进行减法运算*/
result -= mid();
break;
}
return result;
}
int high( void )/*用于计算表达式中级别最高的运算，即带（）的运算*/
{
int result;
if( token == '(' ) /*带有括号的运算*/
{
match( '(' );
result = low();/*递归计算表达式*/
match(')');
}
else if ( token>= '0'&&token<='9' ) /*实际的数字*/
{
ungetc( token, stdin ); /*将读入的字符退还给输入流，为读取整个数*/
scanf( "%d", &result ); /*读出数字*/
token = getchar();  /*读出当前的标志*/
}
else
{
printf("The input has unexpected char\n"); /*不是括号也不是数字*/
exit(1);
}
return result;
}
int main()
{  
int result;  /*运算的结果*/                  
printf("*****************************************\n");
printf("**Welcome to use this simple calculator**\n");
printf("**Please input a multinomial like      **\n");
printf("**                    6-3*(5-1)/2+14/7 **\n");
printf("*****************************************\n");
token = getchar(); /*载入第一个符号*/

result = low(); /*进行计算*/
if( token == '\n' ) /* 是否一行结束 */
printf( "The answer is : %d\n", result );
else
{
printf( "Unexpected char!");
exit(1); /* 出现了例外的字符 */
}
scanf("%d",result);
return 0;
}