
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                              console.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	回车键:	把光标移到第一列
	换行键:	把光标前进到下一行
*/


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


/* 本文件内函数声明 */
PRIVATE void	set_cursor(unsigned int position);
PRIVATE void	set_video_start_addr(t_32 addr);
PRIVATE void	conflush(CONSOLE* p_con);

PUBLIC char _mode3h[29] = {
	0x0C,0x00,0x0F,0x08,0x00,
	0x67,0x00,0x00,0x07,0x10,
	0x0E,0x5F,0x4F,0x50,0x82,
	0x55,0x81,0xBF,0x1F,0x00,
	0x4F,0x9C,0x8E,0x8F,0x28,
	0x1F,0x96,0xB9,0xA3};

PUBLIC char _mode12h[29] = {
	0x01,0x00,0x0F,0x00,0x00,
	0xE3,0x01,0x00,0x02,0x00,
	0x05,0x5F,0x4F,0x50,0x82,
	0x54,0x80,0x0B,0x3E,0x00,
	0x40,0xEA,0x8C,0xDF,0x28,
	0x00,0xE7,0x04,0xE3};
/*======================================================================*
                           init_screen
 *======================================================================*/
PUBLIC void init_screen(TTY* p_tty)
{
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;

	int v_mem_size = V_MEM_SIZE >> 1;	/* 显存总大小 (in WORD) */

	int con_v_mem_size			= v_mem_size / NR_CONSOLES;		/* 每个控制台占的显存大小		(in WORD) */
	p_tty->p_console->original_addr		= nr_tty * con_v_mem_size;		/* 当前控制台占的显存开始地址		(in WORD) */
	p_tty->p_console->v_mem_limit		= con_v_mem_size / SCREEN_WIDTH * SCREEN_WIDTH ;			/* 当前控制台占的显存大小		(in WORD) */
	p_tty->p_console->current_start_addr	= p_tty->p_console->original_addr;	/* 当前控制台显示到了显存的什么位置	(in WORD) */

	p_tty->p_console->cursor = p_tty->p_console->original_addr;	/* 默认光标位置在最开始处 */

	if (nr_tty == 0) {
		p_tty->p_console->cursor = disp_pos / 2;	/* 第一个控制台延用原来的光标位置 */
		disp_pos = 0;
	}
	else {
		out_char(p_tty->p_console, nr_tty + '0');
		out_char(p_tty->p_console, '#');
		out_char(p_tty->p_console, ' ');
	}

	set_cursor(p_tty->p_console->cursor);
}

PUBLIC void sys_dispmode(PROCESS* p_proc, char mode, int params)
{
 	set_video_start_addr(0xA0000);
 	printf("640*480*16 Mode Loading...%d\n", mode);
 	milli_delay(1000);
 	int i = 0;
 	SetVGAMode(mode);
	suduku();
}

PUBLIC void suduku()
{
	disable_irq(MOUSE_IRQ);
	int i;
	int j;
	for (j = 13;j < 15; j++)
		for (i = 13;i < 15 + 452;i++)
		{
			WritePix(0x12, i, j, 0x00);
			WritePix(0x12, j, i, 0x00);
			WritePix(0x12, i, j + 452, 0x00);
			WritePix(0x12, j + 452, i, 0x00);
		}
	for (j = 15 + 25;j < 15 + 25 * 18; j+=25)
		for (i = 13;i < 13 + 450 + 4;i++)
		{
			WritePix(0x12, i, j, 0x00);
			WritePix(0x12, j, i, 0x00);
		}
	for (j = 0; j < 18; j++)
	{
		for (i = 0; i < 18; i++)
		{
			WriteBigPix(0x12, 15 + i * 25 + 2, 15 + j * 25 + 2, 0b00001000, 25 - 3);
		}
	}
	initDL();
	for (i = 480; i < 144 + 480; i++)
	{
		WritePix(0x12, i, 15, 0x00);
		WritePix(0x12, i, 15 + 32, 0x00);
	}
	for (j = 15; j < 15 + 32; j++)
	{
		WritePix(0x12, 480, j, 0x00);
		WritePix(0x12, 480 + 144, j, 0x00);
	}
	WriteString(0x12, 480 + 52, 15 + 8, "RESET", 5, 1, 0x00);
	enable_irq(MOUSE_IRQ);
}

void sudukuchar(int x, int y, t_8 ch, int color, int bkcolor)
{
	int dx = 15 + 50 * x + 17;
	int dy = 15 + 50 * y + 9;
	WriteBigPix(0x12, dx - 8, dy, bkcolor, 32);
	if (ch != 0)
		WriteASCII(0x12, dx, dy, ch, 2, color);
}

void sudukucell(int x, int y, int color, int bkcolor)
{
	int dx = 15 + 50 * x + 4;
	int dy = 15 + 50 * y + 4;
	WriteBigPix(0x12, dx, dy, bkcolor, 32);
}
/*======================================================================*
								读取寄存器的值
=======================================================================*/
void SetVGAReg(char* buff, int mode)
{
	int i = 0;
	for (i = 0; i < 5; i++)
	{
		in_byte(0x3DA);
		out_byte(_portlist[i], _indexlist[i]);
		out_byte(_portlist[i], buff[i]);
	}
	out_byte(_portlist[5], buff[5]);
	for (i = 6; i < 29; i++)
	{
		out_byte(_portlist[i], _indexlist[i]);
		out_byte(_portlist[i] + 1, buff[i]);
	}
	switch (mode)
	{
	case 0x3:
		break;
		t_8* fb = (t_8*)(0x07E00);
		t_8* fd = (t_8*)0xA0000;
		memcpy(fd, fb, 0x2000);	
	case 0x12:
		break;
	}
}

/*======================================================================*
								设置显示模式
=======================================================================*/
PUBLIC void SetVGAMode(int mode)
{
	enable_irq(MOUSE_IRQ);

	printf("SetVGAMode:%d\nCurrrent VGAMode:%d\n", mode, _VGAMode);
	if (_VGAMode < 0x4)
	{
		_TextCursorPos = tty_table[0].p_console->cursor;
		printf("Copy A000:0000...\n");
 		t_8* vmsrc = (t_8*)(0xA0000);
		t_8* vmdes = (t_8*)(0x10000);
		memcpy(vmdes, vmsrc, 0xC0000-0xA0000);
		milli_delay(200);
	}
 	switch (mode)
 	{
 	case 0x03:
		{
			t_8* vmdes = (t_8*)(0xA0000);
			t_8* vmsrc = (t_8*)(0x10000); 
			out_byte(0x3C4, 2);
			out_byte(0x3C5, 0b00000100);
			out_byte(0x3CE, 5);
			out_byte(0x3CF, 0);//写模式0吧……
			out_byte(0x3CE, 6);
			out_byte(0x3CF, 4);
			out_byte(0x3CE, 8);
			out_byte(0x3CF, 0xFF);
			vmsrc = (t_8*)0x7E00;
			memcpy(vmdes, vmsrc, 0x8000);
			SetVGAReg(_mode3h, 0x3);
			vmsrc = (t_8*)0x10000;
			memcpy(vmdes, vmsrc, 0xC0000-0xA0000);
			conflush(tty_table[0].p_console);
			tty_table[0].p_console->cursor = _TextCursorPos;
 			break;
		}
 	case 0x12:
		select_console(0);
 		SetVGAReg(_mode12h, 0x12);
		init_high_screen();
 		break;
 	}
	_VGAMode = mode;
}

/*======================================================================*
								读写像素
=======================================================================*/
PUBLIC void WritePix(int mode, int x, int y, int color)
{
	char dmd = mode & 0xFF;
	switch (dmd)
	{
	case 0x12:
		{
			asm_writepix(x, y, color);
		}
		break;
	}
}
PUBLIC void WriteImage(int mode, int x, int y, t_8* buff, int w, int h)
{
	switch (mode & 0xFF)
	{
	case 0x12:
		{
			int i = 0;
			int j = 0;
			for (i = 0; i < h; i++)
			{
				for (j = 0; j < w; j++)
				{
					int pos = i*w+j;
					if (buff[pos] & 0xF0 == 1)
					{
						WritePix(mode, x, y, buff[pos] & 0x0F);
					}
				}
			}
		}
		break;
	}
}
PUBLIC void WriteBigPix(int mode, int x, int y, int color, int pix)
{
	switch (mode & 0xFF)
	{
	case 0x12:
		{
			int i = 0;
			int j = 0;
			for (i = 0; i < pix; i++)
				for (j = 0; j < pix; j++)
					WritePix(mode, x+i, y+j, color & 0x0f);
		}
		break;
	}
}
PUBLIC void WriteASCII(int mode, int x, int y, t_8 ch, int mul, int color)
{
	switch (mode & 0xFF)
	{
	case 0x12:
		{
			t_8* fmbase = (t_8*)(0x7E00);
			fmbase += 32 * ch;
			int fx = 0;
			int fy = 0;
			int muli = 0;
			int mulj = 0;
			for (fy = 0; fy < 16; fy++)
			{
				for (fx = 0; fx < 8; fx++)
				{
					t_8 t = 0x80;
					t = t>>fx;
					t_8 fb = *(fmbase + fy);
					if (fb & t)
					{
						WriteBigPix(mode, x+fx*mul, y+fy*mul, color, mul);
					}
				}
			}
		}
		break;
	}
}
PUBLIC void WriteString(int mode, int x, int y, char* buff, int size, int mul, int color)
{
	switch (mode & 0xFF)
	{
	case 0x12:
		{
			int i = 0;
			int cx = x;
			for (i = 0; i < size; i++)
			{
				WriteASCII(mode, cx, y, buff[i], mul, color);
				cx += 8 * mul;
				cx += 1 * mul;
			}
		}
		break;
	}
}

PUBLIC void Clean(int mode, t_8 color)
{
	t_8* vmbase = (t_8*)(0xA0000);
	t_8 plane;
	int i = 0;
	switch(mode)
	{
	case 0x12:
		out_byte(0x3C4, 2);
		out_byte(0x3C5, 0b00001111);
		out_byte(0x3CE, 5);
		out_byte(0x3CF, 2);//写模式2吧……
		out_byte(0x3CE, 8);
		out_byte(0x3CF, 0xFF);
		for (i = 0; i < 80*480; i++)
		{
			*(vmbase ++) = color;
		}
		break;
	}
}
PUBLIC void init_high_screen()
{
	Clean(0x12, 0x0F);
}

PUBLIC void out_char_b8000(char ch)
{
	t_8* b_mem = (t_8*)(0xb8000);
	*b_mem += ch;
	*b_mem += DEFAULT_CHAR_COLOR;
}
/*======================================================================*
                           out_char
 *======================================================================*/
PUBLIC void out_char(CONSOLE* p_con, char ch)
{
	t_8* p_vmem = (t_8*)(V_MEM_BASE + p_con->cursor * 2);

	switch(ch) {
	case '\n':
		*(p_vmem+1)=' ';
		*(p_vmem+1)=DEFAULT_CHAR_COLOR;
		if (p_con->cursor < p_con->original_addr + p_con->v_mem_limit - SCREEN_WIDTH) {
			p_con->cursor = p_con->original_addr + SCREEN_WIDTH * ((p_con->cursor - p_con->original_addr) / SCREEN_WIDTH + 1);
		}
		break;
	case '\b':
		*(p_vmem+1)=' ';
		*(p_vmem+1)=DEFAULT_CHAR_COLOR;
		if (p_con->cursor > p_con->original_addr) {
			p_con->cursor--;
			*(p_vmem-2) = ' ';
			*(p_vmem-1) = 0xF7;
		}
		break;
	default:
		if (p_con->cursor < p_con->original_addr + p_con->v_mem_limit - 1) {
			*p_vmem++ = ch;
			*p_vmem++ = DEFAULT_CHAR_COLOR;
			p_con->cursor++;
			*(p_vmem+1)=' ';
			*(p_vmem+1)=0xF7;
		}
		break;
	}

	while (p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE) {
		if(scroll_screen(p_con, SCROLL_SCREEN_DOWN)==0)
		{
			clear();
			p_con->current_start_addr = p_con->original_addr;
			p_con->cursor = p_con->original_addr;
			break;
		}
	}

	conflush(p_con);
}


/*======================================================================*
                           is_current_console
 *======================================================================*/
PUBLIC t_bool is_current_console(CONSOLE* p_con)
{
	return (p_con == &console_table[nr_current_console]);
}


/*======================================================================*
                            set_cursor
 *======================================================================*/
PRIVATE void set_cursor(unsigned int position)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CRTC_DATA_IDX_CURSOR_H);
	out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CRTC_DATA_IDX_CURSOR_L);
	out_byte(CRTC_DATA_REG, position & 0xFF);
	enable_int();

}


/*======================================================================*
                          set_video_start_addr
 *======================================================================*/
PRIVATE void set_video_start_addr(t_32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CRTC_DATA_IDX_START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CRTC_DATA_IDX_START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}


/*======================================================================*
                           select_console
 *======================================================================*/
PUBLIC void select_console(int nr_console)	/* 0 ~ (NR_CONSOLES - 1) */
{
	if ((nr_console < 0) || (nr_console >= NR_CONSOLES)) {	/* invalid console number */
		return;
	}

	nr_current_console = nr_console;

	conflush(&console_table[nr_console]);
}


/*======================================================================*
                           scroll_screen
 *----------------------------------------------------------------------*
 滚屏.
 *----------------------------------------------------------------------*
 direction:
	SCROLL_SCREEN_UP	: 向上滚屏
	SCROLL_SCREEN_DOWN	: 向下滚屏
	其它				: 不做处理
 *======================================================================*/
PUBLIC int scroll_screen(CONSOLE* p_con, int direction)
{
	int Ret = 1;
	if (direction == SCROLL_SCREEN_UP) {
		if (p_con->current_start_addr > p_con->original_addr) {
			p_con->current_start_addr -= SCREEN_WIDTH;
			}
		else Ret = 0;
	}
	else if (direction == SCROLL_SCREEN_DOWN) {
		if (p_con->current_start_addr + SCREEN_SIZE < p_con->original_addr + p_con->v_mem_limit) {
			p_con->current_start_addr += SCREEN_WIDTH;
		}
		else Ret = 0;
	}
	conflush(p_con);
	return Ret;
}


/*======================================================================*
                           conflush
*======================================================================*/
PRIVATE void conflush(CONSOLE* p_con)
{
	if (is_current_console(p_con)) {
		set_cursor(p_con->cursor);
		set_video_start_addr(p_con->current_start_addr);
	}
}


/*======================================================================*
                          sys_clear
*======================================================================*/
PUBLIC int sys_clear(PROCESS * p_proc)
{
	int i;
	TTY * p_tty = &tty_table[p_proc->nr_tty];

	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;
	p_tty->p_console->cursor = p_tty->p_console->original_addr;

	for ( i = 0; i < p_tty->p_console->v_mem_limit ; i++ )
	{
		out_char(p_tty->p_console, ' ');
	}

	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;
	p_tty->p_console->cursor			 = p_tty->p_console->original_addr;
}

