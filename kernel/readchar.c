
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                              readchar.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"


/*======================================================================*
                                 readacommand
 *======================================================================*/
void readacommand(char * pStr)
{
	char ch;
	accepton();
	echoon();
	while ( ( ch = getchar() ) != '\n' )
	{
		*pStr++ = ch;
	}
	*pStr = 0;
	echooff();
	acceptoff();
}


/*======================================================================*
                                 getchar
 *======================================================================*/
char getchar()
{
	char ch;
	accepton();
	echoon();
	while ( !( ch = s_getchar() ) );
	echooff();
	acceptoff();
	return ch;
}

