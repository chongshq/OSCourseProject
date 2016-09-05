
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                tty.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef	_TINIX_TTY_H_
#define	_TINIX_TTY_H_


#define TTY_IN_BYTES	256	/* tty input queue size */

struct s_tty;
struct s_console;

/* TTY */
typedef struct s_tty
{
	t_32	in_buf[TTY_IN_BYTES];	/* TTY 输入缓冲区 */
	t_32*	p_inbuf_head;		/* 指向缓冲区中下一个空闲位置 */
	t_32*	p_inbuf_tail;		/* 指向键盘任务应处理的键值 */
	int		inbuf_count;		/* 缓冲区中已经填充了多少 */

	t_32	out_buf[TTY_IN_BYTES];
	t_32*	p_outbuf_head;
	t_32*	p_outbuf_tail;
	int		outbuf_count;	
	t_32	tempBuffer[TTY_IN_BYTES];
	int		tempLen;

	t_bool	fEcho;	//是否显示字符
	t_bool	fAccept;//可否接受字符
	struct s_console *	p_console;
}TTY;


#endif /* _TINIX_TTY_H_ */
