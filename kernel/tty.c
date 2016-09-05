
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
#include "global.h"
#include "keyboard.h"
#include "proto.h"


#define TTY_FIRST	(tty_table)
#define TTY_END		(tty_table + NR_CONSOLES)


/* 本文件内函数声明 */
PRIVATE void	init_tty(TTY* p_tty);
PRIVATE void	tty_do_read(TTY* p_tty);
PRIVATE void	tty_do_write(TTY* p_tty);
PRIVATE void	tty_in_graphics();
PRIVATE void	in_graphics_process(t_32 key);
PRIVATE void	put_key(TTY* p_tty, t_32 key);

PRIVATE char tty_outbuf_getchar(TTY * p_tty);
PRIVATE void tty_tempbuf_putchar(TTY * p_tty, char ch);
PRIVATE void tty_tempbuf_to_outbuf(TTY * p_tty);
/*======================================================================*
                           task_tty
 *======================================================================*/
PUBLIC void task_tty()
{
	TTY*	p_tty;

	init_keyboard();
	init_mouse();

	for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
		init_tty(p_tty);
	}
	select_console(0);
	_VGAMode = 0x3;

	while (1) {
		if (_VGAMode < 0x4)
		{
			for (p_tty=TTY_FIRST;p_tty<TTY_END;p_tty++) {
				tty_do_read(p_tty);
				tty_do_write(p_tty);
			}
		}else{
			tty_in_graphics();
			//12h
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

	p_tty->outbuf_count = 0;
	p_tty->p_outbuf_head = p_tty->p_outbuf_tail = p_tty->out_buf;
	p_tty->tempLen = 0;
	p_tty->fEcho = FALSE;
	p_tty->fAccept = FALSE;

	init_screen(p_tty);
}
//
void	tty_in_graphics()
{
	keyboard_read(0);
}
//
void in_graphics_process(t_32 key)
{
	if (!(key & FLAG_EXT))
	{
		switch (key & 0xFF)
		{
		case 0x01:
			break;
		}
	}else{
		switch (key & 0x01FF)
		{
		case 0x0101:
			SetVGAMode(0x03);
			_MouseEnable = 0;
			break;
		}
	}
}
/*======================================================================*
                           in_process
 *======================================================================*/
PUBLIC void in_process(TTY* p_tty, t_32 key)
{
	if (p_tty == 0)
	{
		in_graphics_process(key);
		return;
	}
	char output[2] = {0 , 0};

	if (!(key & FLAG_EXT))
	{
		put_key(p_tty, key);
		tty_tempbuf_putchar(p_tty, key & 0xFF);
	}
	else
	{
		int raw_code = key & MASK_RAW;
		switch (raw_code)
		{
		case ENTER:
			put_key(p_tty, 0x0A);
			tty_tempbuf_putchar(p_tty, 0x0A);
			tty_tempbuf_to_outbuf(p_tty);
			break;
		case BACKSPACE:
			put_key(p_tty, '\b');
			if (p_tty->tempLen)
			{
				p_tty->tempLen--;
			}
			break;
		case UP:
			if ( ( key & FLAG_SHIFT_L ) || ( key & FLAG_SHIFT_R ) )
			{
				scroll_screen(p_tty->p_console, SCROLL_SCREEN_UP);
			}
			break;
		case DOWN:
			if ( ( key & FLAG_SHIFT_L ) || ( key & FLAG_SHIFT_R ) )
			{
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
			if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R))
			{
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
	if (p_tty->fAccept == FALSE) return;//是否可以接受字符
	if (p_tty->fEcho   == FALSE) return;//显示是否打开
	if (p_tty->inbuf_count < TTY_IN_BYTES) {
		*(p_tty->p_inbuf_head) = key;
		p_tty->p_inbuf_head++;
		if (p_tty->p_inbuf_head == p_tty->in_buf + TTY_IN_BYTES)
		{
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
PUBLIC int sys_write(PROCESS * p_proc, char * buf, int len)
{
	tty_write(&tty_table[p_proc->nr_tty], buf, len);
	return 0;
}


/*======================================================================*
                              sys_getchar
*======================================================================*/
PUBLIC char sys_getchar(PROCESS * p_proc)
{
	TTY * p_proc_tty = &tty_table[p_proc->nr_tty];
	
	return tty_outbuf_getchar(p_proc_tty);
}


/*======================================================================*
                              tty_outbuf_getchar
*======================================================================*/
PRIVATE char tty_outbuf_getchar(TTY * p_tty)
{
	if (p_tty->outbuf_count == 0) {return 0;}
	char chRet = *p_tty->p_outbuf_tail++;
	p_tty->outbuf_count--;
	if (p_tty->p_outbuf_tail == p_tty->out_buf + TTY_IN_BYTES)
	{
		p_tty->p_outbuf_tail = p_tty->out_buf;
	}
	return chRet;
}


/*======================================================================*
                              tty_tempbuf_putchar
*======================================================================*/
PRIVATE void tty_tempbuf_putchar(TTY * p_tty, char ch)
{
	if (p_tty->fAccept == FALSE) return;

	if (p_tty->tempLen == TTY_IN_BYTES) return;

	p_tty->tempBuffer[p_tty->tempLen] = ch;
	p_tty->tempLen++;
}


/*======================================================================*
                              tty_tempbuf_to_outbuf
*======================================================================*/
PRIVATE void tty_tempbuf_to_outbuf(TTY * p_tty)
{
	int i;
	for ( i = 0 ; i < p_tty->tempLen ; ++i )
	{
		*p_tty->p_outbuf_head++ = p_tty->tempBuffer[i];
		if (p_tty->p_outbuf_head == p_tty->out_buf + TTY_IN_BYTES)
		{
			p_tty->p_outbuf_head = p_tty->out_buf;
		}
		p_tty->outbuf_count++;
		if (p_tty->outbuf_count == TTY_IN_BYTES)
		{
			break;
		}
	}
	p_tty->tempLen = 0;
}


/*======================================================================*
                             sys_echoon
*======================================================================*/
PUBLIC void sys_echoon(PROCESS * p_proc)
{
	tty_table[p_proc->nr_tty].fEcho = TRUE;
}


/*======================================================================*
                             sys_echooff
*======================================================================*/
PUBLIC void sys_echooff(PROCESS * p_proc)
{
	tty_table[p_proc->nr_tty].fEcho = FALSE;
}


/*======================================================================*
                             sys_accepton
*======================================================================*/
PUBLIC void sys_accepton(PROCESS * p_proc)
{
	tty_table[p_proc->nr_tty].fAccept = TRUE;
}


/*======================================================================*
                            sys_acceptoff
*======================================================================*/
PUBLIC void sys_acceptoff(PROCESS * p_proc)
{
	tty_table[p_proc->nr_tty].fAccept = FALSE;
}


/*======================================================================*
                           sys_flush
*======================================================================*/
PUBLIC void sys_flush(PROCESS * p_proc)
{
	tty_table[p_proc->nr_tty].outbuf_count = 0;
	tty_table[p_proc->nr_tty].p_outbuf_head = tty_table[p_proc->nr_tty].out_buf;
	tty_table[p_proc->nr_tty].p_outbuf_tail = tty_table[p_proc->nr_tty].out_buf;
}

