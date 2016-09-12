
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               tty.c
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
#include "keyboard.h"
#include "proto.h"


#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)
#define CMDLINE_BUF_SIZE 256


/* 本文件内函数声明 */
PRIVATE void	init_tty(TTY* p_tty);
PRIVATE void	tty_do_read(TTY* p_tty);
PRIVATE void	tty_do_write(TTY* p_tty);
PRIVATE void	put_key(TTY* p_tty, t_32 key);
PRIVATE void  	cmd_process(TTY* p_tty);
PRIVATE int		get_arg(char * start, void * buf);

PRIVATE char	cmdlinebuf[CMDLINE_BUF_SIZE];
PRIVATE int		cmdlinecount = 0;
//PRIVATE char 	cmd_create[] = "create";
#define cmd_create	"create"
#define cmd_open 	"open"
#define cmd_read	"read"
#define cmd_remove	"remove"
#define cmd_rename	"rename"
#define cmd_list	"list"
#define cmd_write	"write"
#define cmd_close	"close"
#define cmd_help	"help"

/*======================================================================*
                            get_arg
 *======================================================================*/
PRIVATE int get_arg(char * start, void * buf)
{
	int i = 0;
	while ((*(start+i) != '\0') && (*(start+i) != ' ') && i < CMDLINE_BUF_SIZE)
		i++;
		
	if (i == CMDLINE_BUF_SIZE) {
		((char*)buf)[0] = '\0';
		return 0;
	}
	
	memcpy((char*)buf,start,i);
	*((char*)buf+i) = '\0';
	return i;
}


/*======================================================================*
                         cmd_process
 *======================================================================*/
PRIVATE void cmd_process(TTY* p_tty)
{
	int  pointer = 0;
	
	char cmd[CMDLINE_BUF_SIZE];
	char arg1[CMDLINE_BUF_SIZE];
	char arg2[CMDLINE_BUF_SIZE];
	
	pointer += get_arg(cmdlinebuf,cmd);
	pointer++;
	pointer += get_arg(cmdlinebuf+pointer,arg1);
	pointer++;
	pointer += get_arg(cmdlinebuf+pointer,arg2);

	printf("\n");
	
	if (strcmp(cmd,cmd_create,6) == 0)	{
		// CREATE
		gfd = sys_fcreate(arg1, 2);
	} else if (strcmp(cmd,cmd_open,4) == 0) {
		// OPEN
		gfd = sys_fopen(arg1);
	} else if (strcmp(cmd,cmd_close,5) == 0) {
		// CLOSE
		sys_fclose(gfd);
	} else if (strcmp(cmd,cmd_remove,6) == 0) {
		// DELETE
		sys_fdelete(arg1);
	} else if (strcmp(cmd,cmd_rename,6) == 0) {
		// RENAME
		sys_frename(arg1, arg2);
	} else if (strcmp(cmd,cmd_list,4) == 0) {
		// LIST
		sys_flist();
	} else if (strcmp(cmd,cmd_write,5) == 0) {
		// WIRTE
		sys_fwrite(gfd, cmdlinebuf + 6, strlen(cmdlinebuf) - 6);
	} else if (strcmp(cmd,cmd_read,4) == 0){
		// READ
		char buf[5];
		sys_fskbegin(gfd);
		while(sys_fread(gfd, buf, 5))
		{
			printf(buf);
		}
		printf("\n");
	} else if (strcmp(cmd,cmd_help,4) == 0){
		printf("-----------HELP----------\n");
		printf("list             : to list all files.\n");
		printf("create filename  : to create a new file.\n");
		printf("remove filename  : to remove a file.\n");
		printf("open filename    : to open a file.\n");
		printf("close            : to close the opened file.\n");
		printf("read             : to show the content of the file.\n");
		printf("write content    : to write content into the file.\n");
		printf("help             : to show help.\n");
		printf("-----------END----------\n");
	} else {
	
	}
	//printf("#> ");
	memset(cmdlinebuf,'\0',CMDLINE_BUF_SIZE);
	cmdlinecount = 0;
}




/*======================================================================*
                           task_tty
 *======================================================================*/
PUBLIC void task_tty()
{
	TTY*	p_tty;

	init_keyboard();

	for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
		init_tty(p_tty);
	}

	select_console(0);

	while (1) {
		for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
			tty_do_read(p_tty);
			tty_do_write(p_tty);
		}
	}
}


/*======================================================================*
                           init_tty
 *======================================================================*/
PRIVATE void init_tty(TTY* p_tty)
{
	p_tty->inbuf_count = 0;
	p_tty->p_inbuf_head = p_tty->p_inbuf_tail = p_tty->in_buf;

	init_screen(p_tty);
	
	memset(cmdlinebuf,'\0',CMDLINE_BUF_SIZE);
}


/*======================================================================*
                           in_process
 *======================================================================*/
PUBLIC void in_process(TTY* p_tty, t_32 key)
{
	if (!(key & FLAG_EXT)) {
		put_key(p_tty, key);
	}
	else {
		int raw_code = key & MASK_RAW;
		switch(raw_code) {
		case ENTER:
			put_key(p_tty, '\n');
			break;
		case BACKSPACE:
			put_key(p_tty, '\b');
			break;
		case UP:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {	/* Shift + Up */
				scroll_screen(p_tty->p_console, SCROLL_SCREEN_UP);
			}
			break;
		case DOWN:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)) {	/* Shift + Down */
				scroll_screen(p_tty->p_console, SCROLL_SCREEN_DOWN);
			}
			break;
		case F1:
		case F2:
		case F3:
		case F4:
		case F5:
		case F6:
		case F7:
		case F8:
		case F9:
		case F10:
		case F11:
		case F12:
			if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R)) {	/* Alt + F1~F12 */
				select_console(raw_code - F1);
			}
			break;
		default:
			break;
		}
	}
}


/*======================================================================*
                              put_key
*======================================================================*/
PRIVATE void put_key(TTY* p_tty, t_32 key)
{
	if (p_tty->inbuf_count < TTY_IN_BYTES) {
		*(p_tty->p_inbuf_head) = key;
		
		//copy to cmdlinebuf
		char ch = *(p_tty->p_inbuf_head);
		if (ch == '\n')
			cmd_process(p_tty);
		else if (ch == '\b')
			cmdlinebuf[--cmdlinecount] = '\0';
		else	
			cmdlinebuf[cmdlinecount++] = ch;
		
		p_tty->p_inbuf_head++;
		if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_head = p_tty->in_buf;
		}
		p_tty->inbuf_count++;
	}
}


/*======================================================================*
                              tty_do_read
*======================================================================*/
PRIVATE void tty_do_read(TTY* p_tty)
{
	if (is_current_console(p_tty->p_console)) {
		keyboard_read(p_tty);
	}
}


/*======================================================================*
                              tty_do_write
*======================================================================*/
PRIVATE void tty_do_write(TTY* p_tty)
{
	if (p_tty->inbuf_count) {
		char ch = *(p_tty->p_inbuf_tail);
		p_tty->p_inbuf_tail++;
		if (p_tty->p_inbuf_tail == p_tty->in_buf + TTY_IN_BYTES) {
			p_tty->p_inbuf_tail = p_tty->in_buf;
		}
		p_tty->inbuf_count--;

		//disp_int(ch);
		out_char(p_tty->p_console, ch);
	}
}


/*======================================================================*
                              tty_write
*======================================================================*/
PUBLIC void tty_write(TTY* p_tty, char* buf, int len)
{
	char* p = buf;
	int i = len;

	while (i) {
		out_char(p_tty->p_console, *p++);
		i--;
	}
}


/*======================================================================*
                              sys_write
*======================================================================*/
PUBLIC int sys_write(char* buf, int len, PROCESS* p_proc)
{
	tty_write(&tty_table[p_proc->nr_tty], buf, len);
	return 0;
}


