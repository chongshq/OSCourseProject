
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
	t_32	in_buf[TTY_IN_BYTES];	/* TTY ���뻺���� */
	t_32*	p_inbuf_head;		/* ָ�򻺳�������һ������λ�� */
	t_32*	p_inbuf_tail;		/* ָ���������Ӧ����ļ�ֵ */
	int		inbuf_count;		/* ���������Ѿ�����˶��� */

	t_32	out_buf[TTY_IN_BYTES];
	t_32*	p_outbuf_head;
	t_32*	p_outbuf_tail;
	int		outbuf_count;	
	t_32	tempBuffer[TTY_IN_BYTES];
	int		tempLen;

	t_bool	fEcho;	//�Ƿ���ʾ�ַ�
	t_bool	fAccept;//�ɷ�����ַ�
	struct s_console *	p_console;
}TTY;


#endif /* _TINIX_TTY_H_ */
