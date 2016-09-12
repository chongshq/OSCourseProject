
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
#include "file.h"
#include "global.h"
#include "proto.h"


/*======================================================================*
                               is_alphanumeric
 *======================================================================*/
PUBLIC t_bool is_alphanumeric(char ch)
{
	return ((ch >= ' ') && (ch <= '~'));
}

#define MD_TotalBytes		0x01000000 //16M
#define	MD_Base				0x01000000 //16M
#define	MD_Limit			0x02000000 //32M
#define	MD_BytesPerBlock	0x00080000 //512K


PUBLIC void * readMD(void * dest, int blocknum, int offset, int size)
{
	int address = blocknum * MD_BytesPerBlock + MD_Base + offset;
	memcpy((char*)dest,(char*)address,size);
	return dest;
}

PUBLIC void * writeMD(int blocknum, void * src, int offset, int size)
{
	int address = blocknum * MD_BytesPerBlock + MD_Base + offset;
	memcpy((char*)address,(char*)src,size);
	return (void*)address;
}

PUBLIC int strcmp(char * a, char * b, int size)
{
	int lena = strlen(a);
	int lenb = strlen(b);
	if (lena != lenb)
		return lena - lenb;
	
	return memcmp((void*)a,(void*)b,lena);
}


/*======================================================================*
                               itoa
 *======================================================================*/
PUBLIC char * itoa(char * str, int num)/* ����ǰ��� 0 ������ʾ����, ���� 0000B800 ����ʾ�� B800 */
{
	char *	p = str;
	char	ch;
	int	i;
	t_bool	flag = FALSE;

	*p++ = '0';
	*p++ = 'x';

	if(num == 0){
		*p++ = '0';
	}
	else{	
		for(i=28;i>=0;i-=4){
			ch = (num >> i) & 0xF;
			if(flag || (ch > 0)){
				flag = TRUE;
				ch += '0';
				if(ch > '9'){
					ch += 7;
				}
				*p++ = ch;
			}
		}
	}

	*p = 0;

	return str;
}


/*======================================================================*
                               disp_int
 *======================================================================*/
PUBLIC void disp_int(int input)
{
	char output[16];
	itoa(output, input);
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
