
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            klib.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"


/*======================================================================*
                               itoa
 *======================================================================*/
PUBLIC char* _itoa(char * str, int num)/* 数字前面的 0 不被显示出来, 比如 0000B800 被显示成 B800 */
{
	char *	p = str;
	char	ch;
	int	i;
	t_bool	flag = FALSE;

	if(num == 0){
		*p++ = '0';
	}
	else{
		if (num < 0)
		{
			*p++ = '-';
			num = -num;
		}
		for(i=10;i>=0;i--){

			int iTemp = 1, j;
			if (i > 10 || i < 0) return 0;
			for ( j = 0 ; j < i ; ++j )
			{
				iTemp *= 10;
			}
			ch = num / iTemp % 10;
			if(flag || (ch > 0)){
				flag = TRUE;
				ch += '0';
				*p++ = ch;
			}
		}
	}
	*p = 0;

	return str;
}


/*======================================================================*
                               atoi
 *======================================================================*/
t_bool _atoi(const char * str, int * pRet)
{
	int iRet = 0;
	t_bool fNeg = FALSE;
	if (*str == '-')
	{
		fNeg = TRUE;
		++str;
	}
	else if (*str == '+')
	{
		fNeg = FALSE;
		++str;
	}
	while (*str)
	{
		if (*str == ' ' || *str == '.') break;
		if (*str < '0' || *str > '9')
		{
			return FALSE;
		}
		iRet = iRet * 10 + (*str - '0');
		++str;
	}
	*pRet = !fNeg ? iRet : -iRet;
	return TRUE;
}


/*======================================================================*
                               disp_int
 *======================================================================*/
PUBLIC void disp_int(int input)
{
	char output[16];
	_itoa(output, input);
	disp_str(output);
}

/*======================================================================*
                               delay
 *======================================================================*/
PUBLIC void delay(int time)
{
	int i, j, k;
	for(k=0;k<time;k++){
		/*for(i=0;i<10000;i++){	for Virtual PC	*/
		for(i=0;i<10;i++){/*	for Bochs	*/
			for(j=0;j<10000;j++){}
		}
	}
}
