
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC void	out_byte(t_port port, t_8 value);
PUBLIC t_8	in_byte(t_port port);
//PUBLIC void	disable_int();
//PUBLIC void	enable_int();
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);
PUBLIC void disp_mode(char mode);
PUBLIC void asm_writepix(int x, int y, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC t_32	seg2phys(t_16 seg);
PUBLIC void	disable_irq(int irq);
PUBLIC void	enable_irq(int irq);

/* klib.c */
PUBLIC void	delay(int time);
PUBLIC char *	_itoa(char * str, int num);
PUBLIC t_bool _atoi(const char * str, int * pRet);
/* kernel.asm */
PUBLIC void	restart();
void sys_call();

/* main.c */
void ProMain();
void Zza();
void KillPro();
void GraphicMode();
void sys_shutdown(PROCESS * p_proc);
void Newprocess(int id);
int strcmp(char * src, char * dst);
void strtolower(char * src);
void DisPlay();
/* i8259.c */
PUBLIC void	put_irq_handler(int iIRQ, t_pf_irq_handler handler);
PUBLIC void	spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void milli_delay(int milli_sec);
PUBLIC void schedule();
PUBLIC void init_clock();

/* keyboard.c */
PUBLIC void keyboard_handler(int irq);
PUBLIC void init_keyboard();
PUBLIC void keyboard_read();
PUBLIC void init_mouse();
PUBLIC void mouse_handler(int irq);
PUBLIC void DrawCursor(int flag);
PUBLIC void WriteBigPix(int mode, int x, int y, int color, int pix);

/* tty.c */
PUBLIC void task_tty();
PUBLIC void in_process(TTY* p_tty, t_32 key);
PUBLIC int sys_write(PROCESS * p_proc, char * buf, int len);
PUBLIC char sys_getchar(PROCESS * p_proc);
PUBLIC void setecho(TTY * p_tty, t_bool flag);
PUBLIC void sys_echoon(PROCESS * p_proc);
PUBLIC void sys_echooff(PROCESS * p_proc);
PUBLIC void sys_accepton(PROCESS * p_proc);
PUBLIC void sys_acceptoff(PROCESS * p_proc);
PUBLIC void sys_flush(PROCESS * p_proc);

/* console.c */
PUBLIC t_bool is_current_console(CONSOLE * p_con);
PUBLIC void out_char(CONSOLE * p_con, char ch);
PUBLIC void out_char_b8000(char ch);
PUBLIC void init_screen(TTY *p_tty);
PUBLIC void init_high_screen();
PUBLIC void select_console(int nr_console);
PUBLIC int scroll_screen(CONSOLE * p_con, int direction);
PUBLIC int sys_clear(PROCESS * p_proc);
PUBLIC void sys_dispmode(PROCESS* p_proc, char mode, int params);
PUBLIC void SetVGAMode(int mode);
PUBLIC void WritePix(int mode, int x, int y, int color);
PUBLIC void Clean(int mode, t_8 color);
PUBLIC void WriteASCII(int mode, int x, int y, t_8 ch, int mul, int color);
PUBLIC void WriteString(int mode, int x, int y, char* buff, int size, int mul, int color);
PUBLIC void suduku();

/*readchar.c*/
PUBLIC char getchar();
void readacommand(char * pStr);


/************************************************************************/
/*                        以下是系统调用相关                            */
/************************************************************************/


/*------------*/
/* 系统级部分 */
/*------------*/

/* proc.c */
PUBLIC	int	sys_get_ticks();

/*------------*/
/* 用户级部分 */
/*------------*/
/* syscall.asm */
PUBLIC int get_ticks();
PUBLIC void write(char * buf, int len);
PUBLIC char getchar();
PUBLIC void shutdown();
PUBLIC void echoon();
PUBLIC void echooff();
PUBLIC void accepton();
PUBLIC void acceptoff();
PUBLIC void flush();
PUBLIC void dispmode(char mode, int params);
/* main.c */
void sys_shutdown(PROCESS * p_proc);

/*suduku.c*/
PUBLIC int Redraw(int x, int y, int flag);
PUBLIC int ClickDL(int x,int y,int clicktype);
PUBLIC void initDL();
PUBLIC void Deal(int x, int y, int re);
PUBLIC void Lose();
PUBLIC void DrawFlag(int x, int y, int mode);
PUBLIC void DrawDL(int x, int y);
PUBLIC void DrawInt(int x, int y, int Int);