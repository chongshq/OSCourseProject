
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            keyboard.c
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
#include "keymap.h"
#include "proto.h"

PRIVATE	KB_INPUT	kb_in;
PRIVATE	t_bool		code_with_E0	= FALSE;
PRIVATE	t_bool		shift_l;		/* l shift state	*/
PRIVATE	t_bool		shift_r;		/* r shift state	*/
PRIVATE	t_bool		alt_l;			/* l alt state		*/
PRIVATE	t_bool		alt_r;			/* r left state		*/
PRIVATE	t_bool		ctrl_l;			/* l ctrl state		*/
PRIVATE	t_bool		ctrl_r;			/* l ctrl state		*/
PRIVATE	t_bool		caps_lock;		/* Caps Lock		*/
PRIVATE	t_bool		num_lock;		/* Num Lock		*/
PRIVATE	t_bool		scroll_lock;		/* Scroll Lock		*/
PRIVATE	int		column		= 0;	/* keyrow[column] ���� keymap ��ĳһ��ֵ */

/* ���ļ��ں������� */
PRIVATE t_8	get_byte_from_kb_buf();
PRIVATE void	set_leds();
PRIVATE void	kb_wait();
PRIVATE void	kb_ack();
PRIVATE t_8 SetMouse(t_8 byteparam);
PRIVATE void Dispbin(t_8* startaddr, t_8 byteparam);

/*======================================================================*
                            keyboard_handler
 *======================================================================*/
PUBLIC void keyboard_handler(int irq)
{
	t_8 scan_code = in_byte(KB_DATA);

	if (kb_in.count < KB_IN_BYTES) {
		*(kb_in.p_head) = scan_code;
		kb_in.p_head++;
		if (kb_in.p_head == kb_in.buf + KB_IN_BYTES) {
			kb_in.p_head = kb_in.buf;
		}
		kb_in.count++;
	}
}


/*======================================================================*
                           init_keyboard
 *======================================================================*/
PUBLIC void init_keyboard()
{
	_MouseEnable = 0;

	kb_in.count = 0;
	kb_in.p_head = kb_in.p_tail = kb_in.buf;

	caps_lock	= 0;
	num_lock	= 0;
	scroll_lock	= 0;

	set_leds();

	put_irq_handler(KEYBOARD_IRQ, keyboard_handler);	/* �趨�����жϴ������ */
	enable_irq(KEYBOARD_IRQ);				/* �������ж� */
}

PUBLIC void init_mouse()
{
	_MouseEnable = 0;
	_MouseX = 320;
	_MouseY = 240;

	put_irq_handler(MOUSE_IRQ, mouse_handler);
	enable_irq(MOUSE_IRQ);
	kb_wait();

	out_byte(0x64, 0xA8);
	t_8 re = in_byte(0x60);
	Dispbin((t_8*)0xB8000, re);

	re = SetMouse(0xFF);
	Dispbin((t_8*)(0xB8000), re);
	re = SetMouse(0xF4);
	Dispbin((t_8*)(0xB801E), re);

	out_byte(0x64,0x60);
	out_byte(0x60,0x47);

	kb_ack();

}

t_8 SetMouse(t_8 byteparam)
{
	out_byte(0x64, 0xD4);
	out_byte(0x60, byteparam);
	return in_byte(0x60);
}

void Dispbin(t_8* startaddr, t_8 byteparam)
{
 	t_8 i = 0b10000000;
 	t_8* testm = startaddr;
 	for (;testm < startaddr + 0xf;)
 	{
 		if ((byteparam & i) != 0)
 		{
 			*testm++='1';
 			*testm++=DEFAULT_CHAR_COLOR;
 		}else
 		{
 			*testm++='0';
 			*testm++=DEFAULT_CHAR_COLOR;
 		}
 		i>>=1;
 	}
}
PUBLIC void mouse_handler(int irq)
{
	t_8 data = in_byte(0x60);
	static int count = 0;
	static int xs = 0;
	static int ys = 0;
	static t_8 ld = 0;
	static t_8 rd = 0;
	switch (_VGAMode)
	{
	case 0x12:
		switch (++count)
		{
		case 1:
			ld = data & 0x1;
			rd = data & 0x2;
			xs = data & 0x10 ? 0xFFFFFF00 : 0;
			ys = data & 0x20 ? 0xFFFFFF00 : 0;
			break;
		case 2:
			_MouseX += (xs | data);
			if (_MouseX >= 640) _MouseX = 640;
			if (_MouseX < 0) _MouseX = 0;
			break;
		case 3:
			_MouseY -= (ys | data);
			if (_MouseY > 480) _MouseY = 480;
			if (_MouseY < 0) _MouseY = 0;
			DrawCursor(1);
			if (ld)
			{
				DrawCursor(0);
				Redraw(_MouseX, _MouseY, 1);
			}
			else if (rd)
			{
				DrawCursor(0);
				Redraw(_MouseX, _MouseY, 2);
			}
			count = 0;
			break;
		}
		break;
	}
}

PUBLIC void DrawCursor(int clearf)
{
	static t_8 flag = 0;
	static int oldx;
	static int oldy;
	out_byte(0x3CE, 3);
	out_byte(0x3CF, 0b00011000);
	int i;
	int j;
	t_8 color;
	for (j=0;j<12;j++)
	{
		for (i=0;i<12;i++)
		{
			color = ((i == j) || !i) ? 0x0F : 0x0F;
			if (_MouseEnable)
				WritePix(0x12, oldx+i, oldy+j, color);
			if (clearf)
				WritePix(0x12, _MouseX+i, _MouseY+j, color);
			if (i == j) break;
		}
	}
	for (i=0;i<13;i++)
	{
		j = 12;
		color = ((i > 4) || (i==0)) ? 0x0F : 0x0F;
		if (_MouseEnable)
			WritePix(0x12, oldx+i, oldy+j, color);
		if (clearf)
			WritePix(0x12, _MouseX+i, _MouseY+j, color);
	}
	for (j=13;j<18;j++)
	{
		for (i=0;i<6;i++)
		{
			color = (((i + j) == 17) || !i) ? 0x0F : 0x0F;
			if (_MouseEnable)
				WritePix(0x12, oldx+i, oldy+j, color);
			if (clearf)
				WritePix(0x12, _MouseX+i, _MouseY+j, color);
			if ((i + j) == 17) break;
		}
	}
	if (!_MouseEnable) _MouseEnable = 1;
	if (!clearf) _MouseEnable = 0;
	oldx = _MouseX;
	oldy = _MouseY;
	out_byte(0x3CE, 3);
	out_byte(0x3CF, 0);
}
/*======================================================================*
                           keyboard_read
 *======================================================================*/
PUBLIC void keyboard_read(TTY* p_tty)
{
	t_8	scan_code;
	t_bool	make;	/* TRUE : make  */
			/* FALSE: break */
	t_32	key = 0;/* ��һ����������ʾһ������ */
			/* ���磬��� Home �����£��� key ֵ��Ϊ������ keyboard.h �е� 'HOME'��*/
	t_32*	keyrow;	/* ָ�� keymap[] ��ĳһ�� */

	if(kb_in.count > 0){
		code_with_E0 = FALSE;
		scan_code = get_byte_from_kb_buf();

		/* ���濪ʼ����ɨ���� */
		if (scan_code == 0xE1) {
			int i;
			t_8 pausebreak_scan_code[] = {0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5};
			t_bool is_pausebreak = TRUE;
			for(i=1;i<6;i++){
				if (get_byte_from_kb_buf() != pausebreak_scan_code[i]) {
					is_pausebreak = FALSE;
					break;
				}
			}
			if (is_pausebreak) {
				key = PAUSEBREAK;
			}
		}
		else if (scan_code == 0xE0) {
			code_with_E0 = TRUE;
			scan_code = get_byte_from_kb_buf();

			/* PrintScreen ������ */
			if (scan_code == 0x2A) {
				code_with_E0 = FALSE;
				if ((scan_code = get_byte_from_kb_buf()) == 0xE0) {
					code_with_E0 = TRUE;
					if ((scan_code = get_byte_from_kb_buf()) == 0x37) {
						key = PRINTSCREEN;
						make = TRUE;
					}
				}
			}
			/* PrintScreen ���ͷ� */
			else if (scan_code == 0xB7) {
				code_with_E0 = FALSE;
				if ((scan_code = get_byte_from_kb_buf()) == 0xE0) {
					code_with_E0 = TRUE;
					if ((scan_code = get_byte_from_kb_buf()) == 0xAA) {
						key = PRINTSCREEN;
						make = FALSE;
					}
				}
			}
		} /* ������� PrintScreen�����ʱ scan_code Ϊ 0xE0 �������Ǹ�ֵ�� */
		if ((key != PAUSEBREAK) && (key != PRINTSCREEN)) {
			/* �����ж�Make Code ���� Break Code */
			make = (scan_code & FLAG_BREAK ? FALSE : TRUE);
			
			/* �ȶ�λ�� keymap �е��� */
			keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];

			column = 0;

			t_bool caps = shift_l || shift_r;
			if (caps_lock) {
				if ((keyrow[0] >= 'a') && (keyrow[0] <= 'z')){
					caps = !caps;
				}
			}
			if (caps) {
				column = 1;
			}

			if (code_with_E0) {
				column = 2;
			}

			key = keyrow[column];

			switch(key) {
			case SHIFT_L:
				shift_l	= make;
				break;
			case SHIFT_R:
				shift_r	= make;
				break;
			case CTRL_L:
				ctrl_l	= make;
				break;
			case CTRL_R:
				ctrl_r	= make;
				break;
			case ALT_L:
				alt_l	= make;
				break;
			case ALT_R:
				alt_l	= make;
				break;
			case CAPS_LOCK:
				if (make) {
					caps_lock   = !caps_lock;
					set_leds();
				}
				break;
			case NUM_LOCK:
				if (make) {
					num_lock    = !num_lock;
					set_leds();
				}
				break;
			case SCROLL_LOCK:
				if (make) {
					scroll_lock = !scroll_lock;
					set_leds();
				}
				break;
			default:
				break;
			}
		}

		if(make){ /* ���� Break Code */
			t_bool pad = FALSE;

			/* ���ȴ���С���� */
			if ((key >= PAD_SLASH) && (key <= PAD_9)) {
				pad = TRUE;
				switch(key) {	/* '/', '*', '-', '+', and 'Enter' in num pad  */
				case PAD_SLASH:
					key = '/';
					break;
				case PAD_STAR:
					key = '*';
					break;
				case PAD_MINUS:
					key = '-';
					break;
				case PAD_PLUS:
					key = '+';
					break;
				case PAD_ENTER:
					key = ENTER;
					break;
				default:	/* keys whose value depends on the NumLock */
					if (num_lock) {	/* '0' ~ '9' and '.' in num pad */
						if ((key >= PAD_0) && (key <= PAD_9)) {
							key = key - PAD_0 + '0';
						}
						else if (key == PAD_DOT) {
							key = '.';
						}
					}
					else{
						switch(key) {
						case PAD_HOME:
							key = HOME;
							break;
						case PAD_END:
							key = END;
							break;
						case PAD_PAGEUP:
							key = PAGEUP;
							break;
						case PAD_PAGEDOWN:
							key = PAGEDOWN;
							break;
						case PAD_INS:
							key = INSERT;
							break;
						case PAD_UP:
							key = UP;
							break;
						case PAD_DOWN:
							key = DOWN;
							break;
						case PAD_LEFT:
							key = LEFT;
							break;
						case PAD_RIGHT:
							key = RIGHT;
							break;
						case PAD_DOT:
							key = DELETE;
							break;
						default:
							break;
						}
					}
					break;
				}
			}
			key |= shift_l	? FLAG_SHIFT_L	: 0;
			key |= shift_r	? FLAG_SHIFT_R	: 0;
			key |= ctrl_l	? FLAG_CTRL_L	: 0;
			key |= ctrl_r	? FLAG_CTRL_R	: 0;
			key |= alt_l	? FLAG_ALT_L	: 0;
			key |= alt_r	? FLAG_ALT_R	: 0;
			key |= pad	? FLAG_PAD	: 0;

			in_process(p_tty, key);
		}
	}
}


/*======================================================================*
                           get_byte_from_kb_buf
 *======================================================================*/
PRIVATE t_8 get_byte_from_kb_buf()	/* �Ӽ��̻������ж�ȡ��һ���ֽ� */
{
	t_8	scan_code;

	while (kb_in.count <= 0) {}	/* �ȴ���һ���ֽڵ��� */

	disable_int();
	scan_code = *(kb_in.p_tail);
	kb_in.p_tail++;
	if (kb_in.p_tail == kb_in.buf + KB_IN_BYTES) {
		kb_in.p_tail = kb_in.buf;
	}
	kb_in.count--;
	enable_int();

#ifdef __TINIX_DEBUG__
	disp_color_str("[", MAKE_COLOR(WHITE,BLUE));
	disp_int(scan_code);
	disp_color_str("]", MAKE_COLOR(WHITE,BLUE));
#endif

	return scan_code;
}


/*======================================================================*
                                 kb_wait
 *======================================================================*/
PRIVATE void kb_wait()	/* �ȴ� 8042 �����뻺������ */
{
	t_8 kb_stat;

	do {
		kb_stat = in_byte(KB_CMD);
	} while (kb_stat & 0x02);
}


/*======================================================================*
                                 kb_ack
 *======================================================================*/
PRIVATE void kb_ack()
{
	t_8 kb_read;

	do {
		kb_read = in_byte(KB_DATA);
	} while (kb_read =! KB_ACK);
}


/*======================================================================*
                                 set_leds
 *======================================================================*/
PRIVATE void set_leds()
{
	t_8 leds = (caps_lock << 2) | (num_lock << 1) | scroll_lock;

	kb_wait();
	out_byte(KB_DATA, LED_CODE);
	kb_ack();

	kb_wait();
	out_byte(KB_DATA, leds);
	kb_ack();
}


