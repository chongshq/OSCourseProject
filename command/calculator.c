#include "stdio.h"
#include "string.h"

int main(int argc, char * argv[])
{
	int i, num1 = 0, num2 = 0, result = 0;
	char buf[128];
	while(1)
	{
		int operator = 0;
		
		printf("please input operator: 1.+ 2.- 3.* 4./ 0.quit\n");
		i = read (1, buf, 16);
		operator = get_number(buf);
		//printf("%d\n",operator);
		if(operator == 0)
			break;
		else
		{
			buf[0] = 0;
			printf("please input first number:");
			i = read (1, buf, 16);
			num1 = get_number(buf);
			int j = strlen(buf);
			memset(buf,0,128);
			//while (buf[j++]!='\0')
				//buf[j-1] = '\0';
			printf("%d\n",num1);
			printf("please input second number:");
			i = read (1, buf, 16);
			num2 = get_number(buf);
			memset(buf,0,128);
			//for(int j = 0; j< strlen(buf);j++)
			//{	
				//buf[j] = 0;
			//}

			printf("%d\n",num2);
			switch(operator)
			{
				case 1:		
					result = num1 + num2;
					printf("%d + %d = %d\n",  num1, num2, result);
					break;

				case 2:		
					result = num1 - num2;
					printf("%d - %d = %d\n",  num1, num2, result);
					break;

				case 3:		
					result = num1 * num2;
					printf("%d * %d = %d\n",  num1, num2, result);
					break;

				case 4:		
					if(num2<= 0)
					{	
						printf("please input a number which greater than 0\n");
						break;
					}			
					result = num1 / num2;
					printf("%d / %d = %d\n",  num1, num2, result);
					break;
				
				default:
					printf("please input right operator!\n");
			}
		}
	}
	return 0;
}

int get_number(char * buf)
{
	int bit = 1, i = 0, result = 0;
	for (i = 0; i < strlen(buf) - 1; i++)
	{
		bit = bit * 10;
	}
	for (i = 0; i < strlen(buf); i++)
	{
		result = result + (buf[i] - '0') * bit;
		bit = bit/10;
	}	
	return result;
}
