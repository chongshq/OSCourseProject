
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* EXTERN is defined as extern except in global.c */
#ifdef	GLOBAL_VARIABLES_HERE
#undef	EXTERN
#define	EXTERN
#endif

EXTERN	int		ticks;

EXTERN	int		disp_pos;
EXTERN	t_8		gdt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	DESCRIPTOR	gdt[GDT_SIZE];
EXTERN	t_8		idt_ptr[6];	// 0~15:Limit  16~47:Base
EXTERN	GATE		idt[IDT_SIZE];

EXTERN	t_32		k_reenter;

EXTERN	TSS		tss;
EXTERN	PROCESS*	p_proc_ready;

EXTERN	int		nr_current_console;

extern	PROCESS		proc_table[];
extern	int			task_proc_table[];
extern	char		task_stack[];
extern	TASK		task_whole_list[];
//extern	TASK		task_table[];
extern	t_pf_irq_handler	irq_table[];
//extern	TASK		user_proc_table[];
extern	TTY		tty_table[];
extern	CONSOLE		console_table[];
extern	unsigned int _portlist[];
extern	char _indexlist[];
extern	t_8 _VGAMemCopy[];
extern	int map[9][9];
EXTERN	t_8 _VGAMode;
EXTERN	unsigned int _TextCursorPos;
EXTERN	int _MouseX;
EXTERN	int _MouseY;
EXTERN	t_8 _MouseEnable;

extern	int Area[20][20];
extern	int Mask[20][20];
EXTERN int State;

/*
extern	t_pf_irq_handler	irq_table[];

extern	t_sys_call		sys_call_table[];
*/

